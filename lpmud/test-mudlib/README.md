# Engine Test Mudlib

Dedicated mudlib for validating LPmud driver/LPC engine behavior with deterministic, low-noise output.

## Goals

- Keep world content minimal and deterministic.
- Exercise core LPC features and efuns through one command: `runtests`.
- Emit explicit `[PASS]`/`[FAIL]` lines plus a machine-parsable `TEST_SUMMARY`.
- Favor driver/engine coverage over gameplay content coverage.

## Requirements

- `make`
- `expect`
- `nc`
- `lsof`
- `uuidgen`

## Recommended: Automated Run

From repository root:

```bash
cd lpmud
./tests/run_test_mudlib_suite.sh
```

What this script does:

1. Builds `parse`.
2. Copies `test-mudlib` into a temporary isolated runtime mudlib.
3. Starts `parse` with `MUD_LIB` pointing to that runtime copy.
4. Connects via `nc` and runs `runtests`.
5. Responds to the async `input_to` prompt automatically.
6. Writes logs under `tests/reports/test_mudlib_suite_<timestamp>/`.

Pass condition:

- `TEST_SUMMARY total=... pass=... fail=0 ...`
- `[RESULT] PASS all engine checks succeeded.`

## Manual Run

Terminal 1:

```bash
cd lpmud
make parse
MUD_LIB=./test-mudlib MUD_BIND_ADDR=127.0.0.1 ./parse
```

Terminal 2:

```bash
nc localhost 2000
```

Then:

1. Enter a lowercase login name.
2. Run `runtests`.
3. When prompted with `[ACTION][ASYNC-003] Enter token: ACKNOWLEDGE`, type `ACKNOWLEDGE`.
4. Verify `TEST_SUMMARY` ends with `fail=0`.

## Expected Interactive Behavior

- World has two rooms:
  - `room/church` with exit `east`.
  - `room/lab` with exit `west`.
- `look at sign` and `look at console` work.
- `read sign`, `use console`, and `consider console` returning `What ?` is expected in this minimal mudlib (verbs intentionally not implemented).

## Notes On CMD-001 Movement Check

- `CMD-001` validates `command()` dispatch for room exits.
- Harness now normalizes the starting room before running `command("east")`, so `runtests` is valid from either church or lab.
- If it fails, detail includes precondition or source room context.

## Current Coverage Areas

- The current suite executes **47** checks.
- Core language behavior:
  - Arithmetic, loops, recursion, arrays, type predicates, string ops, `sscanf`, `random`, `time`/`ctime`.
- Error behavior:
  - `catch` runtime errors and `throw` value propagation.
- Player/living/network lookup:
  - `users`, `find_player`, `find_living`, `query_ip_number`.
- Object lifecycle and inventory:
  - `clone_object`, `find_object`, `previous_object`, `present`, `transfer`, `first_inventory`, `next_inventory`, destruct lifecycle.
- Command/action dispatch:
  - Room exits via `command`, player verbs via `add_action`, command-context `environment(this_player())` probes.
- Filesystem efuns:
  - `save_object`, `restore_object`, `write_file`, `file_size`, `cat`, `mkdir`, `rmdir`, `rm`.
- Messaging primitives:
  - `tell_object`, `tell_room`, `say`.
- Light and bit efuns:
  - `set_light`, `set_bit`, `clear_bit`, `test_bit`.
- Async behavior:
  - `call_out`, `remove_call_out`, multi-callout ordering, `heart_beat`, `input_to`.

## Remaining Gaps

Potential high-value next steps:

- Privileged/admin efuns (`snoop`, `ed`, shutdown paths) in controlled harnesses.
- `parse_command` grammar edge cases with richer noun/adjective rules.
- Multi-user race/ordering scenarios under simultaneous interactive sessions.
- Negative path tests for mudlib policy hooks (`valid_read`, `valid_write`) under stricter access configurations.

## Troubleshooting

- `bind: Operation not permitted`:
  - Environment blocked listening socket binds. Run outside restricted sandbox or use approved test wrapper.
- Port already in use:
  - Free `localhost:2000` before starting (`lsof -nP -iTCP:2000 -sTCP:LISTEN`).
- Timeouts in async phase:
  - Ensure token prompt is answered exactly with the displayed token.
