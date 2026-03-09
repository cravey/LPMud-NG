#!/usr/bin/env python3
"""Run a targeted mutation pass against driver guards and classify survivors.

The pass runs each mutant in an isolated source copy and compares its
`test-mudlib` failure set with the baseline failure set from the unmutated
driver. Any additional failures (or crashes/no summary) are treated as killed.
"""

from __future__ import annotations

import argparse
import json
import re
import shutil
import subprocess
import tempfile
from dataclasses import dataclass, asdict
from datetime import datetime
from pathlib import Path
from typing import Dict, List, Set


@dataclass
class Mutation:
    mutation_id: str
    file: str
    search: str
    replace: str
    description: str


@dataclass
class SuiteResult:
    exit_code: int
    total: int
    passed: int
    failed: int
    fail_ids: Set[str]
    summary_line: str
    report_dir: str


MUTATIONS: List[Mutation] = [
    Mutation(
        mutation_id="MUT-001",
        file="interpret.c",
        search='if (arg == 0 || arg->type != T_STRING)\n\t    error("Bad argument to find_player\\n");',
        replace='if (arg == 0 || arg->type == T_STRING)\n\t    error("Bad argument to find_player\\n");',
        description="Invert find_player argument-type guard.",
    ),
    Mutation(
        mutation_id="MUT-002",
        file="interpret.c",
        search='if (arg == 0 || arg->type != T_STRING)\n\t    error("Bad argument to find_living\\n");',
        replace='if (arg == 0 || arg->type == T_STRING)\n\t    error("Bad argument to find_living\\n");',
        description="Invert find_living argument-type guard.",
    ),
    Mutation(
        mutation_id="MUT-003",
        file="interpret.c",
        search='if (n < 0 || n > MAX_ARRAY_SIZE)\n\terror("Illegal array size.\\n");',
        replace='if (n < 0 && n > MAX_ARRAY_SIZE)\n\terror("Illegal array size.\\n");',
        description="Weaken allocate_array bounds guard (`||` -> `&&`).",
    ),
    Mutation(
        mutation_id="MUT-004",
        file="interpret.c",
        search='if (arg1->type != T_NUMBER || arg2->type != T_NUMBER) {\n\t    error("Bad type on arg to \'|\'\\n");',
        replace='if (arg1->type != T_NUMBER && arg2->type != T_NUMBER) {\n\t    error("Bad type on arg to \'|\'\\n");',
        description="Weaken bitwise-or operand guard (`||` -> `&&`).",
    ),
    Mutation(
        mutation_id="MUT-005",
        file="simulate.c",
        search='if (weight && weight->type != T_NUMBER)\n\terror("Bad type the weight of object in transfer()\\n");',
        replace='if (weight && weight->type == T_NUMBER)\n\terror("Bad type the weight of object in transfer()\\n");',
        description="Invert transfer() query_weight() type guard.",
    ),
]


SUMMARY_RE = re.compile(r"TEST_SUMMARY total=(\d+) pass=(\d+) fail=(\d+) duration=(\d+)s")
FAIL_ID_RE = re.compile(r"\[FAIL\]\[([A-Z0-9-]+)\]")


def run_cmd(cmd: List[str], cwd: Path, timeout_s: int, log_path: Path) -> int:
    proc = subprocess.run(
        cmd,
        cwd=str(cwd),
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
        timeout=timeout_s,
        check=False,
    )
    log_path.write_text(proc.stdout)
    return proc.returncode


def copy_source_tree(src_root: Path, dst_root: Path) -> None:
    ignore = shutil.ignore_patterns(
        ".git",
        "mudlib",
        "tests/reports",
        "*.o",
        "parse",
        ".build_mode",
        "lexical.c",
        "lang.c",
        "y.tab.h",
    )
    shutil.copytree(src_root, dst_root, ignore=ignore)


def latest_suite_report(root: Path) -> Path | None:
    reports_dir = root / "tests" / "reports"
    if not reports_dir.exists():
        return None
    candidates = sorted(reports_dir.glob("test_mudlib_suite_*"), key=lambda p: p.stat().st_mtime, reverse=True)
    return candidates[0] if candidates else None


def parse_suite_result(root: Path, exit_code: int) -> SuiteResult | None:
    report = latest_suite_report(root)
    if not report:
        return None
    session = report / "session.log"
    if not session.exists():
        return None
    text = session.read_text(errors="ignore")
    m = SUMMARY_RE.search(text)
    if not m:
        return None
    fail_ids = set(FAIL_ID_RE.findall(text))
    return SuiteResult(
        exit_code=exit_code,
        total=int(m.group(1)),
        passed=int(m.group(2)),
        failed=int(m.group(3)),
        fail_ids=fail_ids,
        summary_line=m.group(0),
        report_dir=str(report),
    )


def apply_mutation(root: Path, mutation: Mutation) -> bool:
    path = root / mutation.file
    text = path.read_text(errors="ignore")
    if mutation.search not in text:
        return False
    mutated = text.replace(mutation.search, mutation.replace, 1)
    if mutated == text:
        return False
    path.write_text(mutated)
    return True


def run_suite(root: Path, logs_dir: Path, timeout_s: int) -> SuiteResult | None:
    logs_dir.mkdir(parents=True, exist_ok=True)
    exit_code = run_cmd(
        ["./tests/run_test_mudlib_suite.sh"],
        cwd=root,
        timeout_s=timeout_s,
        log_path=logs_dir / "run_test_mudlib_suite.log",
    )
    result = parse_suite_result(root, exit_code)
    if result:
        (logs_dir / "summary.txt").write_text(result.summary_line + "\n")
        (logs_dir / "fail_ids.txt").write_text("\n".join(sorted(result.fail_ids)) + ("\n" if result.fail_ids else ""))
    return result


def select_mutations(mutations: List[Mutation], allow_ids: Set[str] | None, limit: int | None) -> List[Mutation]:
    out: List[Mutation] = []
    for m in mutations:
        if allow_ids and m.mutation_id not in allow_ids:
            continue
        out.append(m)
    if limit is not None and limit >= 0:
        out = out[:limit]
    return out


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source-root", default=".", help="Root of lpmud source tree.")
    parser.add_argument(
        "--output-dir",
        default=None,
        help="Output report directory. Default: tests/reports/driver_mutation_suite_<timestamp>",
    )
    parser.add_argument(
        "--mutations",
        default=None,
        help="Comma-separated mutation IDs to run (default: all).",
    )
    parser.add_argument("--limit", type=int, default=None, help="Run only the first N selected mutations.")
    parser.add_argument("--timeout", type=int, default=240, help="Timeout seconds per suite run.")
    args = parser.parse_args()

    src_root = Path(args.source_root).resolve()
    stamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    if args.output_dir:
        output_dir = Path(args.output_dir).resolve()
    else:
        output_dir = src_root / "tests" / "reports" / f"driver_mutation_suite_{stamp}"
    output_dir.mkdir(parents=True, exist_ok=True)

    allow_ids = None
    if args.mutations:
        allow_ids = {x.strip() for x in args.mutations.split(",") if x.strip()}
    selected = select_mutations(MUTATIONS, allow_ids, args.limit)

    if not selected:
        (output_dir / "SUMMARY.txt").write_text("No mutations selected.\n")
        print(f"wrote: {output_dir / 'SUMMARY.txt'}")
        return 0

    run_rows: List[Dict[str, object]] = []
    with tempfile.TemporaryDirectory(prefix="lpmud-mutation-pass.") as td:
        td_path = Path(td)
        baseline_root = td_path / "baseline"
        copy_source_tree(src_root, baseline_root)

        baseline_logs = output_dir / "baseline"
        baseline = run_suite(baseline_root, baseline_logs, timeout_s=args.timeout)
        if not baseline:
            (output_dir / "SUMMARY.txt").write_text(
                "Baseline suite did not produce a parseable TEST_SUMMARY.\n"
            )
            print(f"wrote: {output_dir / 'SUMMARY.txt'}")
            return 2

        for mutation in selected:
            mutant_root = td_path / mutation.mutation_id
            copy_source_tree(baseline_root, mutant_root)
            logs = output_dir / mutation.mutation_id
            logs.mkdir(parents=True, exist_ok=True)

            applied = apply_mutation(mutant_root, mutation)
            if not applied:
                row = {
                    "mutation_id": mutation.mutation_id,
                    "description": mutation.description,
                    "status": "error_not_applied",
                    "reason": "search pattern not found",
                }
                run_rows.append(row)
                (logs / "status.txt").write_text("error_not_applied\n")
                continue

            result = run_suite(mutant_root, logs, timeout_s=args.timeout)
            if not result:
                row = {
                    "mutation_id": mutation.mutation_id,
                    "description": mutation.description,
                    "status": "killed_no_summary",
                    "reason": "suite run did not produce TEST_SUMMARY",
                }
                run_rows.append(row)
                (logs / "status.txt").write_text("killed_no_summary\n")
                continue

            killed = (result.failed != baseline.failed) or (result.fail_ids != baseline.fail_ids)
            row = {
                "mutation_id": mutation.mutation_id,
                "description": mutation.description,
                "status": "killed" if killed else "survived",
                "summary_line": result.summary_line,
                "fail_ids": sorted(result.fail_ids),
                "baseline_fail_ids": sorted(baseline.fail_ids),
                "exit_code": result.exit_code,
                "report_dir": result.report_dir,
            }
            run_rows.append(row)
            (logs / "status.txt").write_text(("killed\n" if killed else "survived\n"))

    killed_count = sum(1 for r in run_rows if str(r.get("status", "")).startswith("killed"))
    survived_count = sum(1 for r in run_rows if r.get("status") == "survived")
    error_count = sum(1 for r in run_rows if str(r.get("status", "")).startswith("error_"))

    payload = {
        "generated_at": datetime.now().isoformat(timespec="seconds"),
        "baseline": {
            "summary_line": baseline.summary_line if "baseline" in locals() and baseline else "",
            "fail_ids": sorted(baseline.fail_ids) if "baseline" in locals() and baseline else [],
            "total": baseline.total if "baseline" in locals() and baseline else 0,
            "passed": baseline.passed if "baseline" in locals() and baseline else 0,
            "failed": baseline.failed if "baseline" in locals() and baseline else 0,
        },
        "totals": {
            "selected": len(selected),
            "killed": killed_count,
            "survived": survived_count,
            "errors": error_count,
        },
        "mutations": [asdict(m) for m in selected],
        "results": run_rows,
    }
    (output_dir / "mutation_results.json").write_text(json.dumps(payload, indent=2) + "\n")

    lines = []
    lines.append("# Driver Mutation Pass")
    lines.append("")
    lines.append(f"Generated: {datetime.now().isoformat(timespec='seconds')}")
    lines.append("")
    if "baseline" in locals() and baseline:
        lines.append("## Baseline")
        lines.append("")
        lines.append(f"- Summary: `{baseline.summary_line}`")
        lines.append(f"- Fail IDs: `{', '.join(sorted(baseline.fail_ids)) if baseline.fail_ids else '(none)'}`")
        lines.append("")
    lines.append("## Totals")
    lines.append("")
    lines.append(f"- Selected mutants: **{len(selected)}**")
    lines.append(f"- Killed: **{killed_count}**")
    lines.append(f"- Survived: **{survived_count}**")
    lines.append(f"- Errors: **{error_count}**")
    lines.append("")
    lines.append("## Results")
    lines.append("")
    for row in run_rows:
        mid = row.get("mutation_id", "?")
        status = row.get("status", "unknown")
        desc = row.get("description", "")
        lines.append(f"- `{mid}` `{status}` {desc}")
        if "summary_line" in row:
            lines.append(f"  - summary: `{row['summary_line']}`")
        if "fail_ids" in row:
            fail_ids = row["fail_ids"]
            fail_text = ", ".join(fail_ids) if fail_ids else "(none)"
            lines.append(f"  - fail_ids: `{fail_text}`")
    lines.append("")
    (output_dir / "mutation_results.md").write_text("\n".join(lines) + "\n")

    (output_dir / "SUMMARY.txt").write_text(
        "Driver mutation pass completed.\n\n"
        f"Output dir: {output_dir}\n"
        f"Selected: {len(selected)}\n"
        f"Killed: {killed_count}\n"
        f"Survived: {survived_count}\n"
        f"Errors: {error_count}\n"
    )

    print(f"wrote: {output_dir / 'mutation_results.md'}")
    print(f"wrote: {output_dir / 'mutation_results.json'}")
    print(f"wrote: {output_dir / 'SUMMARY.txt'}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
