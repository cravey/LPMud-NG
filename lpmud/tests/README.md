# Test Tooling (`lpmud/tests`)

## Summary

This directory contains automated tooling for deep LPmud driver validation:

- default engine suite execution (`test-mudlib`)
- branch-mining target generation
- NG vs orig differential comparison
- isolated fuzzing runs
- targeted mutation passes
- current engine-suite baseline on this branch: `225/225` pass (`SEC-001`, `SEC-002` fixed)

Default wrappers are designed for non-interactive use.

## Usage (Top)

Run from `lpmud/`.

### Standard engine suite

```bash
./tests/run_test_mudlib_suite.sh
```

### SSL variant

```bash
./tests/run_test_mudlib_ssl_suite.sh
```

### Differential (NG vs orig)

```bash
./tests/run_driver_differential_suite.sh
```

### Branch mining report (`error()/fatal()` + efun surface)

```bash
python3 tests/mine_driver_targets.py --source-root .
```

### Isolated fuzzing pass

```bash
./tests/run_driver_fuzz_suite.sh
```

Optional knobs:

```bash
FUZZ_SEEDS=1337,7331 FUZZ_ITERATIONS=500 FUZZ_MAX_PAYLOAD_LEN=2048 ./tests/run_driver_fuzz_suite.sh
```

### Mutation pass

```bash
./tests/run_driver_mutation_suite.sh
```

Optional knobs:

```bash
./tests/run_driver_mutation_suite.sh --mutations MUT-001,MUT-004 --limit 2 --timeout 240
```

## Interaction Expectations

- Wrappers in this directory are automated.
- Manual `nc` runs must follow the exact token sequence documented in [test-mudlib README](../test-mudlib/README.md).

## Method Mapping

1. `error()/fatal()` branch mining
- Script: `tests/mine_driver_targets.py`
- Outputs: `driver_error_branches.md`, `driver_error_branches.json`
- Default output path: `tests/reports/branch_mining_<timestamp>/`

2. Efun contract matrix and driver/LPC surface assertions
- Executed by `runtests` in `test-mudlib` (`CON-*`, `OPR-*`, `PAR-*`, `SCN-*`, `BRM-*`)
- Includes quarantined probes to avoid unstable legacy crash/hang behavior in default suite

3. Differential behavior testing (NG vs orig)
- Script: `tests/run_driver_differential_suite.sh`
- Outputs: per-driver status maps + `diff.tsv`
- Requires host-runnable `ORIG_PARSE_BIN`

4. Coverage-guided fuzzing (isolated)
- Wrapper: `tests/run_driver_fuzz_suite.sh`
- Worker: `tests/fuzz_driver_surface.py`
- Output path: `tests/reports/driver_fuzz_suite_<timestamp>/`
- Reports include disconnect/crash-suspected signal and anomaly lines

5. Mutation testing
- Wrapper: `tests/run_driver_mutation_suite.sh`
- Worker: `tests/run_driver_mutation_suite.py`
- Output path: `tests/reports/driver_mutation_suite_<timestamp>/`
- Compares each mutant against baseline fail-ID set to classify `killed` vs `survived`

## Additional Tooling

- `analyze_mudlib.py`: static mudlib analysis reports.
- `run_engine_functional_suite.sh` + `nc_engine_suite.exp`: classic mudlib end-to-end functional checks.
- `test_mudlib_suite.exp`: expect harness used by dedicated test-mudlib suites.

## Prerequisites

- `make`
- `python3`
- `expect`
- `nc`
- `openssl` (SSL suite)
- `lsof`
- `uuidgen`

## Outputs

All wrappers write timestamped artifacts under `tests/reports/`.

Common artifacts:

- `SUMMARY.txt`
- build logs (`make_parse*.log`)
- driver logs (`parse.log`)
- harness/session logs (`expect.log`, `session.log`, fuzz/mutation reports)

## Known Limitations

- `lpmud-orig` prebuilt binaries may be architecture-incompatible on modern hosts.
- Some crash-prone probes are intentionally quarantined from default `runtests` and must be run in isolation.
- In restricted sandboxes, local bind on `127.0.0.1:2000` may fail.
