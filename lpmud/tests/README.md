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
| `test_user.sh` | shell helper | Generates throwaway test login names accepted by test mudlib constraints (`a-z`, short length) without requiring `uuidgen`. | `/dev/urandom`, `head`, `od`, shell utilities | emitted username string |
| `run_driver_differential_suite.sh` | shell runner | Runs the dedicated suite against NG and orig parse binaries, compares PASS/FAIL ID maps. | NG parse, orig parse, `test_mudlib_suite.exp` | `tests/reports/driver_diff_suite_<ts>/diff.tsv` |
| `run_driver_fuzz_suite.sh` | shell runner | Isolated command-surface fuzz runs against live driver. | `fuzz_driver_surface.py`, `test-mudlib/` | `tests/reports/driver_fuzz_suite_<ts>/` |
| `run_driver_security_suite.sh` | shell runner | Driver/network security regressions not suitable for in-process LPC-only coverage (bind defaults, ACCESS.DENY robustness, newline payload resilience). | `parse`, `test-mudlib/`, `nc`, `lsof` | `tests/reports/driver_security_suite_<ts>/` |
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
2. `make ci-warnings`, `make ci-sanitize`, `make ci-analyze`, `make ci-tidy`, and `make ci` do compiler/analyzer checks only; they do not call scripts from this directory.
3. `make clean` removes `tests/reports/*` (along with build artifacts), so prior test run artifacts are cleared.

---

## CI vs Runtime Coverage Matrix

| Command | Coverage type | Runs `parse` process | Uses network/expect | Notes |
|---|---|---|---|---|
| `make ci-warnings` | compiler warning gate | no | no | strict warning policy |
| `make ci-sanitize` | sanitizer compile/link gate | no | no | checks ASAN/UBSAN build configuration |
| `make ci-analyze` | static analysis | no | no | `clang --analyze` |
| `make ci-tidy` | calibrated static lint | no | no | `clang-tidy` check set tuned for actionable findings |
| `make ci` | aggregate CI compile/analyze gates | no | no | does not execute gameplay/runtime flows |
| `make mudlibtest` | dedicated behavioral runtime suite | yes | yes | primary regression command for driver behavior |
| `make securitytest` | driver/network security integration suite | yes | yes | covers startup/bind/admission/payload regressions outside LPC-only scope |
| `./tests/run_engine_functional_suite.sh` | classic mudlib functional flow | yes | yes | gameplay/session checks on `mudlib/` |
| `./tests/run_test_mudlib_ssl_suite.sh` | dedicated suite under TLS | yes | yes | validates SSL-enabled runtime path |

Use `make ci` + `make ci-tidy` plus at least one runtime suite (typically `make mudlibtest`) for balanced confidence.

---

## Typical Entry Points

Run from `lpmud/`:

```bash
./tests/run_test_mudlib_suite.sh
./tests/run_test_mudlib_ssl_suite.sh
./tests/run_engine_functional_suite.sh
./tests/run_driver_differential_suite.sh
./tests/run_driver_fuzz_suite.sh
./tests/run_driver_security_suite.sh
./tests/run_driver_mutation_suite.sh
python3 tests/mine_driver_targets.py --source-root .
python3 tests/analyze_mudlib.py --mudlib-root mudlib --output-dir tests/reports/static_only
```

Equivalent Make entry point for the primary dedicated suite:

```bash
make mudlibtest
make securitytest
```

---

## Important Runtime Requirements

1. Shell/tooling: `bash`, `make`, `python3`.
2. Network harness: `expect`, `nc`.
3. Script utilities used by runners: `grep`, `sed`, `awk`, `tail`, `lsof`, `head`, `od`, `mktemp`, `openssl` (SSL suite). (`uuidgen` is no longer required.)
4. Differential suite requires two parse binaries (`NG_PARSE_BIN` and `ORIG_PARSE_BIN`, or `ORIG_LPMUD_ROOT` with `parse`).
5. Local bind/connect access to `127.0.0.1:2000` is required for network suites.

---

## Failure Triage By Symptom

| Symptom | Most likely cause(s) | First checks |
|---|---|---|
| `bind: Operation not permitted` or cannot listen on `localhost:2000` | restricted sandbox/permissions, existing listener | run outside restricted sandbox; run `lsof -nP -iTCP:2000 -sTCP:LISTEN` |
| `expect` timeout waiting for prompt/output | driver failed to boot, wrong mudlib root, prompt mismatch | inspect `parse.log`, `make_parse.log`, and `session.log`; confirm `MUD_LIB` points to intended runtime copy |
| suite exits early after compile step | build failure or tool missing | read `make_parse.log`; verify required tools in this doc are present |
| `Illegal character '?'` / `Illegal character ':'` in `test_runner.c` | this LPC grammar does not support C-style ternary (`?:`), so lexer/parser reject those tokens and later statements may produce cascading errors like `Illegal LHS` | inspect `parse.log`, replace ternary expression with explicit `if/else` assignment |
| SSL suite handshake failure | missing/mismatched cert/key, cert trust mismatch, SSL not enabled in build | check `MUD_SSL_*` env vars, inspect `expect.log`, verify cert/key pair and run `openssl s_client` probe |
| differential suite cannot compare runs | missing `ORIG_PARSE_BIN`/`NG_PARSE_BIN` inputs | set `NG_PARSE_BIN` and either `ORIG_PARSE_BIN` or `ORIG_LPMUD_ROOT` |
| fuzz run marks crash-suspected artifacts | driver exited or dropped connection under fuzz input | inspect per-seed JSON and transcript logs in run directory, then reproduce that seed in isolation |

---

## Notes

1. Runners use isolated temporary mudlib runtime copies so they do not mutate the source mudlib/test-mudlib trees.
2. TLS suite generates short-lived cert/key material under its own report directory (`.../tls/`), not in source control.
3. In restricted sandboxes, binding localhost ports may fail; run outside restrictive sandboxing when needed.
