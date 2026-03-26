# Test Tooling (`lpmud/tests`)

## Scope

This directory contains the test runners, expect harnesses, and analysis/fuzz/mutation utilities used to validate the driver and mudlib behavior around `parse`.

All generated artifacts are written under `lpmud/tests/reports/` in timestamped run directories.

---

## Directory Contents

| Path | Type | Purpose | Primary Inputs | Primary Outputs |
|---|---|---|---|---|
| `run_test_mudlib_suite.sh` | shell runner | Main dedicated `test-mudlib` integration suite. Builds `parse`, runs driver on isolated copied `test-mudlib`, executes `runtests`. | `test-mudlib/`, `test_mudlib_suite.exp` | `tests/reports/test_mudlib_suite_<ts>/` |
| `run_test_mudlib_ssl_suite.sh` | shell runner | TLS variant of the same suite. Builds with `USE_SSL=1`, generates ephemeral self-signed cert, runs harness with OpenSSL client. | `test-mudlib/`, `test_mudlib_suite.exp`, OpenSSL libs | `tests/reports/test_mudlib_ssl_suite_<ts>/` |
| `run_engine_functional_suite.sh` | shell runner | Classic mudlib end-to-end checks (new + existing player flows) plus static mudlib analysis. | `mudlib/`, `nc_engine_suite.exp`, `analyze_mudlib.py` | `tests/reports/engine_suite_<ts>/` |
| `run_driver_differential_suite.sh` | shell runner | Runs the dedicated suite against NG and orig parse binaries, compares PASS/FAIL ID maps. | NG parse, orig parse, `test_mudlib_suite.exp` | `tests/reports/driver_diff_suite_<ts>/diff.tsv` |
| `run_driver_fuzz_suite.sh` | shell runner | Isolated command-surface fuzz runs against live driver. | `fuzz_driver_surface.py`, `test-mudlib/` | `tests/reports/driver_fuzz_suite_<ts>/` |
| `run_driver_mutation_suite.sh` | shell wrapper | Thin wrapper for mutation orchestrator. | `run_driver_mutation_suite.py` | Delegates to Python runner output |
| `run_driver_mutation_suite.py` | python runner | Applies targeted source mutations in isolated copies, reruns dedicated suite, classifies killed/survived mutants. | `run_test_mudlib_suite.sh` | `tests/reports/driver_mutation_suite_<ts>/` |
| `fuzz_driver_surface.py` | python worker | Randomized interactive socket fuzzer with anomaly/crash-suspected reporting. | live driver on host/port | per-seed `.json` and transcript logs |
| `analyze_mudlib.py` | python analysis | Static mudlib heuristic reports (monster/weapon balance + LPC feature usage). | `mudlib/` source tree | markdown/json reports in chosen output dir |
| `mine_driver_targets.py` | python analysis | Heuristic mining of `error()/fatal()` branches + inferred LPC surface coverage. | driver `.c` files + test-mudlib sources | `driver_error_branches.md/.json` |
| `test_mudlib_suite.exp` | expect harness | Logs in and runs `runtests`; tracks FAIL lines; emits `SUITE_RESULT`. Supports `nc` or `openssl s_client`. | running driver on TCP socket | exit code + transcript side effects |
| `nc_engine_suite.exp` | expect harness | Interactive gameplay/session checks for classic mudlib (new/existing scenarios). | running driver via `nc` | exit code + transcript side effects |
| `reports/` | artifact dir | Timestamped output directories created by runners. | n/a | run logs, summaries, generated reports |

---

## How The Pieces Relate

Shared dedicated-suite harness chain:

1. `run_test_mudlib_suite.sh` -> `test_mudlib_suite.exp`
2. `run_test_mudlib_ssl_suite.sh` -> `test_mudlib_suite.exp` (`--client openssl`)
3. `run_driver_differential_suite.sh` -> `test_mudlib_suite.exp` twice (`ng`, `orig`)
4. `run_driver_mutation_suite.py` -> repeated `run_test_mudlib_suite.sh` runs -> `test_mudlib_suite.exp`

Classic mudlib chain:

1. `run_engine_functional_suite.sh` -> `analyze_mudlib.py` (static report)
2. `run_engine_functional_suite.sh` -> `nc_engine_suite.exp` (new + existing interactive sessions)

Fuzz chain:

1. `run_driver_fuzz_suite.sh` starts `parse` with isolated `test-mudlib`
2. invokes `fuzz_driver_surface.py` per seed
3. aggregates per-seed JSON/log artifacts into `SUMMARY.txt`

Independent static mining:

1. `mine_driver_targets.py` scans driver source and test source heuristically
2. does not require launching the driver

---

## Relationship To `lpmud/Makefile`

1. `make mudlibtest` calls `./tests/run_test_mudlib_suite.sh`, then prints the generated `SUMMARY.txt` when present.
2. `make ci-warnings`, `make ci-sanitize`, `make ci-analyze`, and `make ci` do compiler/analyzer checks only; they do not call scripts from this directory.
3. `make clean` removes `tests/reports/*` (along with build artifacts), so prior test run artifacts are cleared.

---

## Typical Entry Points

Run from `lpmud/`:

```bash
./tests/run_test_mudlib_suite.sh
./tests/run_test_mudlib_ssl_suite.sh
./tests/run_engine_functional_suite.sh
./tests/run_driver_differential_suite.sh
./tests/run_driver_fuzz_suite.sh
./tests/run_driver_mutation_suite.sh
python3 tests/mine_driver_targets.py --source-root .
python3 tests/analyze_mudlib.py --mudlib-root mudlib --output-dir tests/reports/static_only
```

Equivalent Make entry point for the primary dedicated suite:

```bash
make mudlibtest
```

---

## Important Runtime Requirements

1. Shell/tooling: `bash`, `make`, `python3`.
2. Network harness: `expect`, `nc`.
3. Script utilities used by runners: `grep`, `sed`, `awk`, `tail`, `lsof`, `uuidgen`, `mktemp`, `openssl` (SSL suite).
4. Differential suite requires two parse binaries (`NG_PARSE_BIN` and `ORIG_PARSE_BIN`, or `ORIG_LPMUD_ROOT` with `parse`).
5. Local bind/connect access to `127.0.0.1:2000` is required for network suites.

---

## Notes

1. Runners use isolated temporary mudlib runtime copies so they do not mutate the source mudlib/test-mudlib trees.
2. TLS suite generates short-lived cert/key material under its own report directory (`.../tls/`), not in source control.
3. In restricted sandboxes, binding localhost ports may fail; run outside restrictive sandboxing when needed.
