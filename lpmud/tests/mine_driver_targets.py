#!/usr/bin/env python3
"""Mine LPC-driver error/fatal branches and efun coverage targets.

Produces:
- driver_error_branches.json
- driver_error_branches.md

The report is heuristic: it marks likely LPC-surface branches and likely
coverage by matching inferred API names against current test-mudlib sources.
"""

from __future__ import annotations

import argparse
import json
import re
from dataclasses import dataclass, asdict
from datetime import datetime
from pathlib import Path
from typing import Dict, List


SURFACE_FILES = {"interpret.c", "parse.c", "simulate.c", "object.c"}


@dataclass
class Branch:
    branch_id: str
    file: str
    line: int
    level: str
    function: str
    message: str
    inferred_api: str
    surface_reachable: bool
    likely_covered: bool


def find_function_context(lines: List[str], idx: int) -> str:
    sig = re.compile(r"^\s*([A-Za-z_][A-Za-z0-9_]*)\s*\([^;]*\)\s*\{?\s*$")
    for i in range(idx, -1, -1):
        m = sig.match(lines[i])
        if m:
            return m.group(1)
    return "unknown"


def infer_api_from_message(msg: str) -> str:
    patterns = [
        r"to ([A-Za-z_][A-Za-z0-9_]*)\(",
        r"to ([A-Za-z_][A-Za-z0-9_]*)\.",
        r"^([A-Za-z_][A-Za-z0-9_]*)\(",
        r"Bad (?:first|second|third) argument to ([A-Za-z_][A-Za-z0-9_]*)",
        r"Wrong type arg [12] to ([A-Za-z_][A-Za-z0-9_]*)",
        r"query_idle\(\)",
        r"parse_command",
    ]
    for pat in patterns:
        m = re.search(pat, msg)
        if m:
            if m.groups():
                return m.group(1)
            if "query_idle" in pat:
                return "query_idle"
            if "parse_command" in pat:
                return "parse_command"
    return ""


def parse_branches(source_root: Path, test_code: str) -> List[Branch]:
    branches: List[Branch] = []
    branch_idx = 1
    for cfile in sorted(source_root.glob("*.c")):
        lines = cfile.read_text(errors="ignore").splitlines()
        for lineno, line in enumerate(lines, start=1):
            m = re.search(r"\b(error|fatal)\s*\(\s*\"([^\"]*)\"", line)
            if not m:
                continue
            level = m.group(1)
            msg = m.group(2)
            fn = find_function_context(lines, lineno - 1)
            api = infer_api_from_message(msg)
            surface = cfile.name in SURFACE_FILES and (
                bool(api) or "Bad " in msg or "Wrong " in msg or "Illegal " in msg
            )
            covered = bool(api) and re.search(rf"\b{re.escape(api)}\s*\(", test_code) is not None
            branches.append(
                Branch(
                    branch_id=f"BRN-{branch_idx:04d}",
                    file=cfile.name,
                    line=lineno,
                    level=level,
                    function=fn,
                    message=msg,
                    inferred_api=api,
                    surface_reachable=surface,
                    likely_covered=covered,
                )
            )
            branch_idx += 1
    return branches


def read_predefs(source_root: Path) -> List[str]:
    lexical = (source_root / "lexical.l").read_text(errors="ignore")
    block_m = re.search(
        r"static struct keyword \{.*?\} predefs\[\] = \{(.*?)\n\};\n#define NELEM",
        lexical,
        flags=re.S,
    )
    if not block_m:
        return []
    block = block_m.group(1)
    return sorted(set(re.findall(r'\{\s*"([a-z_]+)"\s*,\s*F_[A-Z0-9_]+\s*,\s*\}', block)))


def render_report(
    branches: List[Branch], predefs: List[str], test_code: str, output_dir: Path
) -> None:
    total = len(branches)
    surface = [b for b in branches if b.surface_reachable]
    surface_covered = [b for b in surface if b.likely_covered]
    surface_uncovered = [b for b in surface if not b.likely_covered]

    covered_predefs = [p for p in predefs if re.search(rf"\b{re.escape(p)}\s*\(", test_code)]
    missing_predefs = [p for p in predefs if p not in covered_predefs]

    md_lines = []
    md_lines.append("# Driver Branch-Mining Report")
    md_lines.append("")
    md_lines.append(f"Generated: {datetime.now().isoformat(timespec='seconds')}")
    md_lines.append("")
    md_lines.append("## Summary")
    md_lines.append("")
    md_lines.append(f"- Total `error()/fatal()` branches mined: **{total}**")
    md_lines.append(f"- Likely LPC-surface branches: **{len(surface)}**")
    md_lines.append(f"- Likely covered LPC-surface branches: **{len(surface_covered)}**")
    md_lines.append(f"- Likely uncovered LPC-surface branches: **{len(surface_uncovered)}**")
    md_lines.append(f"- Efun predefs in driver: **{len(predefs)}**")
    md_lines.append(f"- Efun predefs referenced by test-mudlib sources: **{len(covered_predefs)}**")
    md_lines.append(f"- Efun predefs still missing from test sources: **{len(missing_predefs)}**")
    md_lines.append("")
    md_lines.append("## Top Uncovered LPC-Surface Branches")
    md_lines.append("")
    for b in surface_uncovered[:120]:
        api = b.inferred_api if b.inferred_api else "n/a"
        md_lines.append(
            f"- `{b.branch_id}` `{b.file}:{b.line}` `{b.function}` api=`{api}` msg=`{b.message}`"
        )
    md_lines.append("")
    md_lines.append("## Missing Efun Predefs")
    md_lines.append("")
    for name in missing_predefs:
        md_lines.append(f"- `{name}`")
    md_lines.append("")

    (output_dir / "driver_error_branches.md").write_text("\n".join(md_lines) + "\n")
    payload: Dict[str, object] = {
        "generated_at": datetime.now().isoformat(timespec="seconds"),
        "summary": {
            "total_branches": total,
            "surface_branches": len(surface),
            "surface_covered": len(surface_covered),
            "surface_uncovered": len(surface_uncovered),
            "predefs_total": len(predefs),
            "predefs_covered": len(covered_predefs),
            "predefs_missing": len(missing_predefs),
        },
        "missing_predefs": missing_predefs,
        "branches": [asdict(b) for b in branches],
    }
    (output_dir / "driver_error_branches.json").write_text(json.dumps(payload, indent=2) + "\n")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--source-root",
        default=".",
        help="Driver source root (directory containing interpret.c, parse.c, ...).",
    )
    parser.add_argument(
        "--test-sources",
        nargs="+",
        default=[
            "test-mudlib/obj/test_runner.c",
            "test-mudlib/obj/player.c",
        ],
        help="LPC test sources for heuristic coverage matching.",
    )
    parser.add_argument(
        "--output-dir",
        default=None,
        help="Output directory. Default: tests/reports/branch_mining_<timestamp>",
    )
    args = parser.parse_args()

    source_root = Path(args.source_root).resolve()
    if args.output_dir:
        output_dir = Path(args.output_dir).resolve()
    else:
        stamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        output_dir = source_root / "tests" / "reports" / f"branch_mining_{stamp}"
    output_dir.mkdir(parents=True, exist_ok=True)

    test_chunks = []
    for src in args.test_sources:
        p = source_root / src
        if p.exists():
            test_chunks.append(p.read_text(errors="ignore"))
    test_code = "\n".join(test_chunks)

    branches = parse_branches(source_root, test_code)
    predefs = read_predefs(source_root)
    render_report(branches, predefs, test_code, output_dir)

    print(f"wrote: {output_dir / 'driver_error_branches.md'}")
    print(f"wrote: {output_dir / 'driver_error_branches.json'}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
