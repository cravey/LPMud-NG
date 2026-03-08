#!/usr/bin/env python3
"""Static mudlib analysis:
- Monster difficulty and weapon-drop strength heuristics.
- LPC feature usage inventory.

This script is intentionally regex/heuristic based; it is fast and robust
enough for broad mudlib auditing without requiring a full LPC parser.
"""

from __future__ import annotations

import argparse
import json
import re
import statistics
from dataclasses import dataclass, field
from pathlib import Path
from typing import Dict, List, Optional, Tuple


RE_CLONE = re.compile(
    r'(?P<var>\w+)\s*=\s*clone_object\(\s*"(?P<path>[^"]+)"\s*\)\s*;',
    re.MULTILINE,
)
RE_CALL_OTHER = re.compile(
    r'call_other\(\s*(?P<var>\w+)\s*,\s*"(?P<func>[^"]+)"(?:\s*,\s*(?P<arg>[^)]*?))?\)\s*;',
    re.MULTILINE | re.DOTALL,
)
RE_MOVE_OBJECT = re.compile(
    r"move_object\(\s*(?P<src>\w+)\s*,\s*(?P<dst>[\w]+|this_object\(\))\s*\)\s*;",
    re.MULTILINE,
)
RE_STRING_LITERAL = re.compile(r'^\s*"([^"]+)"\s*$')
RE_INT = re.compile(r"-?\d+")


FEATURE_PATTERNS = {
    "inherit": re.compile(r"^\s*inherit\s+\"[^\"]+\"", re.MULTILINE),
    "arrays_literal": re.compile(r"\(\{"),
    "allocate": re.compile(r"\ballocate\s*\("),
    "call_other": re.compile(r"\bcall_other\s*\("),
    "clone_object": re.compile(r"\bclone_object\s*\("),
    "move_object": re.compile(r"\bmove_object\s*\("),
    "add_action": re.compile(r"\badd_action\s*\("),
    "input_to": re.compile(r"\binput_to\s*\("),
    "call_out": re.compile(r"\bcall_out\s*\("),
    "set_heart_beat": re.compile(r"\bset_heart_beat\s*\("),
    "sscanf": re.compile(r"\bsscanf\s*\("),
    "catch": re.compile(r"\bcatch\s*\("),
    "for_loop": re.compile(r"\bfor\s*\("),
    "while_loop": re.compile(r"\bwhile\s*\("),
    "switch": re.compile(r"\bswitch\s*\("),
    "preprocessor": re.compile(r"^\s*#(define|ifdef|ifndef|if|elif|else|endif)\b", re.MULTILINE),
}


@dataclass
class Entity:
    file: str
    line: int
    var: str
    clone_path: str
    kind: str
    properties: Dict[str, str] = field(default_factory=dict)

    def get_int_property(self, key: str) -> Optional[int]:
        raw = self.properties.get(key)
        if raw is None:
            return None
        raw = raw.strip()
        if re.fullmatch(r"-?\d+", raw):
            try:
                return int(raw)
            except ValueError:
                return None
        # Heuristic: use first integer as lower bound / baseline.
        m = RE_INT.search(raw)
        if not m:
            return None
        try:
            return int(m.group(0))
        except ValueError:
            return None

    def get_string_property(self, key: str) -> Optional[str]:
        raw = self.properties.get(key)
        if raw is None:
            return None
        m = RE_STRING_LITERAL.match(raw.strip())
        if m:
            return m.group(1)
        return None


def line_at(text: str, pos: int) -> int:
    return text.count("\n", 0, pos) + 1


def classify_clone(path: str) -> str:
    p = path.lower()
    if p.endswith("obj/monster") or p.endswith("obj/monster.c") or "/monster" in p:
        return "monster"
    if p.endswith("obj/weapon") or p.endswith("obj/weapon.c") or "/weapon" in p:
        return "weapon"
    return "other"


def level_band(level: Optional[int]) -> str:
    if level is None:
        return "unknown"
    if level <= 3:
        return "trivial(1-3)"
    if level <= 8:
        return "low(4-8)"
    if level <= 14:
        return "mid(9-14)"
    if level <= 20:
        return "high(15-20)"
    return "extreme(21+)"


def scan_entities(mudlib_root: Path) -> Tuple[List[Entity], List[Tuple[Entity, Entity, str, int]]]:
    entities: List[Entity] = []
    drops: List[Tuple[Entity, Entity, str, int]] = []

    for path in sorted(mudlib_root.rglob("*.c")):
        try:
            text = path.read_text(encoding="utf-8", errors="ignore")
        except OSError:
            continue

        rel = str(path.relative_to(mudlib_root.parent))
        active: Dict[str, Entity] = {}
        events = []

        for m in RE_CLONE.finditer(text):
            events.append(("clone", m.start(), m))
        for m in RE_CALL_OTHER.finditer(text):
            events.append(("call_other", m.start(), m))
        for m in RE_MOVE_OBJECT.finditer(text):
            events.append(("move_object", m.start(), m))
        events.sort(key=lambda t: t[1])

        for event_type, _, m in events:
            if event_type == "clone":
                var = m.group("var")
                clone_path = m.group("path")
                ent = Entity(
                    file=rel,
                    line=line_at(text, m.start()),
                    var=var,
                    clone_path=clone_path,
                    kind=classify_clone(clone_path),
                )
                entities.append(ent)
                active[var] = ent
            elif event_type == "call_other":
                var = m.group("var")
                if var not in active:
                    continue
                func = m.group("func")
                arg = m.group("arg")
                active[var].properties[func] = (arg or "").strip()
            elif event_type == "move_object":
                src = m.group("src")
                dst = m.group("dst")
                if src not in active:
                    continue
                src_ent = active[src]
                if src_ent.kind != "weapon":
                    continue
                if dst in active and active[dst].kind == "monster":
                    drops.append((active[dst], src_ent, rel, line_at(text, m.start())))

    return entities, drops


def feature_inventory(mudlib_root: Path) -> Dict[str, dict]:
    files = sorted(mudlib_root.rglob("*.c"))
    summary: Dict[str, dict] = {}
    for name, pat in FEATURE_PATTERNS.items():
        per_file = []
        total_hits = 0
        for p in files:
            try:
                text = p.read_text(encoding="utf-8", errors="ignore")
            except OSError:
                continue
            hits = len(list(pat.finditer(text)))
            if hits:
                per_file.append(
                    {"file": str(p.relative_to(mudlib_root.parent)), "hits": hits}
                )
                total_hits += hits
        per_file.sort(key=lambda x: x["hits"], reverse=True)
        summary[name] = {
            "total_hits": total_hits,
            "files_with_hits": len(per_file),
            "top_files": per_file[:10],
        }
    summary["_meta"] = {"total_c_files": len(files)}
    return summary


def build_monster_weapon_report(entities: List[Entity], drops) -> dict:
    monsters = [e for e in entities if e.kind == "monster"]
    weapons = [e for e in entities if e.kind == "weapon"]

    monster_rows = []
    for m in monsters:
        lvl = m.get_int_property("set_level")
        wc = m.get_int_property("set_wc")
        ac = m.get_int_property("set_ac")
        hp = m.get_int_property("set_hp")
        name = m.get_string_property("set_name")
        monster_rows.append(
            {
                "name": name or m.var,
                "file": m.file,
                "line": m.line,
                "level": lvl,
                "weapon_class": wc,
                "armour_class": ac,
                "hit_points": hp,
                "difficulty_band": level_band(lvl),
                "clone_path": m.clone_path,
            }
        )

    drop_rows = []
    for mon, weap, rel, line in drops:
        drop_rows.append(
            {
                "monster_name": mon.get_string_property("set_name") or mon.var,
                "monster_file": mon.file,
                "monster_level": mon.get_int_property("set_level"),
                "monster_band": level_band(mon.get_int_property("set_level")),
                "weapon_name": weap.get_string_property("set_name") or weap.var,
                "weapon_class": weap.get_int_property("set_class"),
                "weapon_value": weap.get_int_property("set_value"),
                "source_file": rel,
                "source_line": line,
            }
        )

    levels = [r["level"] for r in monster_rows if isinstance(r["level"], int)]
    wc_values = [r["weapon_class"] for r in monster_rows if isinstance(r["weapon_class"], int)]
    drop_wc = [r["weapon_class"] for r in drop_rows if isinstance(r["weapon_class"], int)]

    band_counts: Dict[str, int] = {}
    for r in monster_rows:
        band_counts[r["difficulty_band"]] = band_counts.get(r["difficulty_band"], 0) + 1

    drop_by_band: Dict[str, Dict[str, float]] = {}
    for band in sorted(set(r["monster_band"] for r in drop_rows)):
        vals = [r["weapon_class"] for r in drop_rows if r["monster_band"] == band and isinstance(r["weapon_class"], int)]
        if vals:
            drop_by_band[band] = {
                "count": len(vals),
                "avg_weapon_class": round(sum(vals) / len(vals), 2),
                "max_weapon_class": max(vals),
                "min_weapon_class": min(vals),
            }
        else:
            drop_by_band[band] = {"count": 0}

    strongest_drops = sorted(
        [r for r in drop_rows if isinstance(r["weapon_class"], int)],
        key=lambda r: (r["weapon_class"], r["monster_level"] or -1),
        reverse=True,
    )[:30]

    hardest_monsters = sorted(
        [r for r in monster_rows if isinstance(r["level"], int)],
        key=lambda r: (r["level"], r["weapon_class"] or -1, r["armour_class"] or -1, r["hit_points"] or -1),
        reverse=True,
    )[:40]

    return {
        "summary": {
            "monster_clones_detected": len(monsters),
            "weapon_clones_detected": len(weapons),
            "monster_weapon_drop_links_detected": len(drop_rows),
            "difficulty_band_counts": band_counts,
            "level_stats": {
                "count_numeric": len(levels),
                "min": min(levels) if levels else None,
                "max": max(levels) if levels else None,
                "avg": round(sum(levels) / len(levels), 2) if levels else None,
                "median": statistics.median(levels) if levels else None,
            },
            "monster_wc_stats": {
                "count_numeric": len(wc_values),
                "min": min(wc_values) if wc_values else None,
                "max": max(wc_values) if wc_values else None,
                "avg": round(sum(wc_values) / len(wc_values), 2) if wc_values else None,
            },
            "drop_weapon_class_stats": {
                "count_numeric": len(drop_wc),
                "min": min(drop_wc) if drop_wc else None,
                "max": max(drop_wc) if drop_wc else None,
                "avg": round(sum(drop_wc) / len(drop_wc), 2) if drop_wc else None,
            },
            "drop_weapon_class_by_monster_band": drop_by_band,
        },
        "hardest_monsters": hardest_monsters,
        "strongest_detected_drops": strongest_drops,
        "all_monsters": monster_rows,
        "all_drops": drop_rows,
    }


def render_balance_md(data: dict) -> str:
    lines = []
    summary = data["summary"]
    lines.append("# Mudlib Monster Difficulty and Weapon-Drop Review")
    lines.append("")
    lines.append("## Summary")
    lines.append(
        f"- Monster clones detected: **{summary['monster_clones_detected']}**"
    )
    lines.append(
        f"- Weapon clones detected: **{summary['weapon_clones_detected']}**"
    )
    lines.append(
        f"- Monster->weapon drop links detected: **{summary['monster_weapon_drop_links_detected']}**"
    )
    ls = summary["level_stats"]
    lines.append(
        f"- Monster level range (numeric): **{ls['min']}..{ls['max']}**, avg **{ls['avg']}**, median **{ls['median']}**"
    )
    lines.append("")
    lines.append("## Difficulty Bands")
    for band, count in sorted(summary["difficulty_band_counts"].items()):
        lines.append(f"- {band}: {count}")
    lines.append("")
    lines.append("## Strongest Weapon Drops (Heuristic)")
    for r in data["strongest_detected_drops"][:20]:
        lines.append(
            f"- {r['weapon_name']} (wc={r['weapon_class']}, value={r['weapon_value']})"
            f" from {r['monster_name']} (lvl={r['monster_level']}, band={r['monster_band']})"
            f" at `{r['source_file']}:{r['source_line']}`"
        )
    lines.append("")
    lines.append("## Hardest Monsters (Heuristic)")
    for r in data["hardest_monsters"][:25]:
        lines.append(
            f"- {r['name']} lvl={r['level']} wc={r['weapon_class']} ac={r['armour_class']} hp={r['hit_points']}"
            f" at `{r['file']}:{r['line']}`"
        )
    lines.append("")
    lines.append("## Notes")
    lines.append("- This is regex-based and may miss custom inheritance-heavy setups.")
    lines.append("- Levels/wc/ac/hp from non-literal expressions are approximated by first integer token.")
    return "\n".join(lines) + "\n"


def render_features_md(data: dict) -> str:
    lines = []
    lines.append("# Mudlib LPC Feature Inventory")
    lines.append("")
    lines.append(f"- C source files scanned: **{data['_meta']['total_c_files']}**")
    lines.append("")
    for feat in sorted(k for k in data.keys() if not k.startswith("_")):
        row = data[feat]
        lines.append(f"## {feat}")
        lines.append(
            f"- total_hits: {row['total_hits']}, files_with_hits: {row['files_with_hits']}"
        )
        for top in row["top_files"][:8]:
            lines.append(f"- {top['file']}: {top['hits']}")
        lines.append("")
    return "\n".join(lines)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--mudlib-root",
        default="mudlib",
        help="Path to mudlib root directory (default: mudlib)",
    )
    parser.add_argument(
        "--output-dir",
        default="tests/reports",
        help="Directory for generated report files (default: tests/reports)",
    )
    args = parser.parse_args()

    mudlib_root = Path(args.mudlib_root)
    out_dir = Path(args.output_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    entities, drops = scan_entities(mudlib_root)
    balance = build_monster_weapon_report(entities, drops)
    features = feature_inventory(mudlib_root)

    (out_dir / "monster_weapon_balance.json").write_text(
        json.dumps(balance, indent=2, sort_keys=True), encoding="utf-8"
    )
    (out_dir / "monster_weapon_balance.md").write_text(
        render_balance_md(balance), encoding="utf-8"
    )
    (out_dir / "lpc_feature_usage.json").write_text(
        json.dumps(features, indent=2, sort_keys=True), encoding="utf-8"
    )
    (out_dir / "lpc_feature_usage.md").write_text(
        render_features_md(features), encoding="utf-8"
    )
    print(f"Wrote reports to {out_dir}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
