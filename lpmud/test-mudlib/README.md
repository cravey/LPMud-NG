# Engine Test Mudlib

## Summary

- Purpose: deterministic, driver-focused LPC validation using one command: `runtests`.
- Suite size and PASS/FAIL totals intentionally are not hard-coded here; they change as probes evolve.
- Security regression probes include `SEC-001` and `SEC-002`.
- Recommended execution: automated wrappers in `lpmud/tests/` (non-interactive).

To get current results, run:

```bash
make mudlibtest
```

Then inspect the generated `tests/reports/test_mudlib_suite_<timestamp>/SUMMARY.txt`.

---

## Usage (Quick Start)

From `lpmud/`:

```bash
./tests/run_test_mudlib_suite.sh
```

Also available:

```bash
./tests/run_test_mudlib_ssl_suite.sh
./tests/run_driver_differential_suite.sh
./tests/run_driver_fuzz_suite.sh
./tests/run_driver_mutation_suite.sh
python3 tests/mine_driver_targets.py --source-root .
```

---

## Required User Interaction (Manual `nc` Runs)

Automated wrappers answer prompts for you. Manual sessions require exact token input:

1. At `Login name (lowercase letters):` enter only lowercase `a-z` (max 11 chars).
2. Run `runtests`.
3. Respond exactly in this order:
   1. `RACE-001` prompt: type `raceok` (lowercase, case-sensitive).
   2. `NOECHO-001` prompt: type `SILENTTOKEN` (UPPERCASE, case-sensitive).
   3. `ASYNC-003` prompt: type `ACKNOWLEDGE` (UPPERCASE, case-sensitive).

Rules:

- Tokens are case-sensitive.
- Type exactly the shown token, no quotes.
- Press Enter after each token.
- `NOECHO-001` input is hidden (`input_to(..., 1)`), so characters will not echo.

---

## Manual Run

Terminal 1:

```bash
cd lpmud
make parse
MUD_BIND_ADDR=127.0.0.1 MUD_LIB=./test-mudlib ./parse
```

Terminal 2:

```bash
nc localhost 2000
```

Then follow the interaction requirements above.

---

## Coverage Overview

Suite areas covered by default `runtests`:

- Core LPC behavior: arithmetic, recursion, arrays, type predicates, string ops, `sscanf`, random/time.
- Efun contracts: broad bad-type/bad-context matrix (`CON-001`..`CON-096`).
- Operator contracts: arithmetic/bitwise/assignment guard matrix (`OPR-001`..`OPR-025`).
- Parser/input contracts: `parse_command` + `sscanf` matrix (`PAR-*`, `SCN-*`).
- Branch-mined abstractions: additional portable checks (`BRM-001`..`BRM-008`).
- Object/inventory lifecycle: clone/find/destruct/move/transfer edge behavior.
- Security/privilege probes: low-level `ed/snoop/shutdown` denial and path boundary checks.
- Async behavior: `call_out`, `remove_call_out`, ordering, `heart_beat`, `input_to`, `input_to(noecho)`, race handling.

---

## Quarantined Hazard Probes

These are intentionally excluded from default `runtests` because older drivers may hang/crash:

- `explodehazard`
- `weightdestructhazard`
- `commandlenhazard`
- `pathcharhazard`
- hard-link-only efuns (`people`, `localcmd`, `add_adj`, `add_subst`, `regcomp`, `regexec`) are tracked as quarantined contract checks

Run hazard probes only in isolated processes/sessions.

---

## Driver vs Test-Mudlib Crash Attribution

Use this triage order:

1. Reproduce with a minimal single probe in a fresh process.
2. Check compile/runtime diagnostics first (`parse.log`, object compile errors, `catch()` behavior).
3. Re-run through wrappers to rule out manual-input mistakes.
4. Compare behavior across drivers with `run_driver_differential_suite.sh` when possible.
5. If a probe can kill the process, keep it quarantined and out of default `runtests`.

---

## Notes

- Runtime currently prefers `.i` in this mudlib layout; keep `.c` and `.i` synchronized when editing runner/player objects.
- Differential runs against `lpmud-orig` require a host-compatible original binary.

---

## Troubleshooting

- `bind: Operation not permitted`:
  - Run wrappers outside restricted sandbox.
- Port conflict on `localhost:2000`:
  - Check listeners with `lsof -nP -iTCP:2000 -sTCP:LISTEN`.
- Async timeouts:
  - Verify token order and exact case-sensitive input.
