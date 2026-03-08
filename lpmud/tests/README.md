# Test Tooling (`lpmud/tests`)

This directory contains automation for driver-centric validation and mudlib analysis.

## Contents

- `analyze_mudlib.py`
  - Static heuristic scanner for:
    - monster difficulty and weapon-drop relationships
    - LPC language feature usage inventory
  - Writes Markdown + JSON reports.
- `run_engine_functional_suite.sh`
  - Runs static analysis on the classic `mudlib/`.
  - Boots `parse` with an isolated runtime mudlib copy.
  - Executes two end-to-end network sessions through `nc` + `expect`.
- `nc_engine_suite.exp`
  - Expect script used by `run_engine_functional_suite.sh`.
  - Covers new-player and existing-player flows in the classic mudlib.
- `run_test_mudlib_suite.sh`
  - Boots `parse` against `test-mudlib/`.
  - Executes the dedicated LPC engine suite via `runtests`.
- `test_mudlib_suite.exp`
  - Expect script for the dedicated `test-mudlib` suite.
  - Captures `[FAIL]` lines and enforces `fail=0`.
- `reports/`
  - Timestamped run artifacts from the scripts above.

## Prerequisites

- `make`
- `python3`
- `expect`
- `nc`
- `lsof`
- `uuidgen`

## Run Static Mudlib Analysis Only

From `lpmud/`:

```bash
python3 tests/analyze_mudlib.py \
  --mudlib-root mudlib \
  --output-dir tests/reports/static_only
```

Expected outputs:

- `tests/reports/static_only/monster_weapon_balance.md`
- `tests/reports/static_only/lpc_feature_usage.md`
- `tests/reports/static_only/monster_weapon_balance.json`
- `tests/reports/static_only/lpc_feature_usage.json`

## Run Classic Mudlib Functional Suite

From `lpmud/`:

```bash
./tests/run_engine_functional_suite.sh
```

Expected outputs:

- `tests/reports/engine_suite_<timestamp>/SUMMARY.txt`
- session transcripts and parse/build logs
- static analysis reports under `tests/reports/engine_suite_<timestamp>/static/`

## Run Dedicated Engine Test Mudlib Suite

From `lpmud/`:

```bash
./tests/run_test_mudlib_suite.sh
```

Expected outputs:

- `tests/reports/test_mudlib_suite_<timestamp>/SUMMARY.txt`
- `expect.log`, `session.log`, `parse.log`, `make_parse.log`
- `TEST_SUMMARY ... fail=0` in logs when successful

## Notes

- Both run scripts create isolated runtime mudlib copies under temporary directories.
- Test scripts bind to `127.0.0.1:2000` by default.
- Existing listeners on port `2000` are terminated by the wrappers before launch.
