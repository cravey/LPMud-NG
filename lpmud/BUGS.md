# LPmud Bugs

Canonical status tracker for known defects.

- Last reviewed: 2026-03-29
- Sources: legacy `BUGS`, [`SECURITY.md`](SECURITY.md), `ISSUES`

## Legend

- `FIXED`: confirmed fixed in this branch.
- `UNFIXED`: confirmed and still open.
- `UNVERIFIED`: reported historically but not yet reproduced/closed.
- Severity ratings: `HIGH`, `MEDIUM`, `LOW` indicate potential impact if real.

## Confirmed Unfixed Bugs

- [UNFIXED] query_weight() self-destruct recursion hazard
  If query_weight() destructs the current object, transfer/destruct paths can
  recurse until recursion limits trigger. Current behavior is still hazardous
  (typically "Too deep recursion" rather than immediate process crash).

- [UNFIXED] runtime/debug errors are not consistently mirrored to wizard logs
  Some compile/parser errors are mirrored, but many runtime/debug-only messages
  still do not reach wizard-facing logs.

- [UNFIXED][HIGH] restore_object() variable-name parse can write past fixed stack buffer
  In restore parsing, `object.c` copies the variable token into `char var[100]`
  using `strncpy(var, buff, space - buff)` and then writes `var[space - buff]`.
  A crafted/invalid line with a long token before the first space can make
  `space - buff` exceed 99, causing out-of-bounds stack write.

## Unverified Bug Reports

- [UNVERIFIED] "Search pattern /*/ in ed will hang"
  Not reproduced in current quick checks. Keep open as unverified until a
  dedicated in-driver ed reproduction confirms or disproves.

- [UNVERIFIED][MEDIUM] SIGHUP handler may call non-async-signal-safe shutdown path
  `backend.c` installs `startshutdowngame` directly as a signal handler. The
  handler chain likely touches non-async-signal-safe code and should be
  validated/reworked to a `sig_atomic_t` flag + main-loop shutdown handoff.

- [UNVERIFIED][MEDIUM] unvalidated numeric parsing in command/file input paths
  Several paths use `atoi()`/`sscanf("%d", ...)` without strict error/range
  checks (`main.c`, `object.c`, `parse.c`, `simulate.c`, `wiz_list.c`). This
  can silently accept malformed input or overflow/truncate values; needs a
  targeted audit and conversion to checked `strtol`-style parsing.

- [UNVERIFIED][MEDIUM] backend recovery setup ignores `setjmp()` return-path value
  `backend.c` calls `setjmp(error_recovery_context)` and ignores the return
  value. This may be intentional style, but recovery-path clarity is reduced
  and future edits can easily break first-pass vs `longjmp` behavior.

- [UNVERIFIED][LOW] integer-width arithmetic assumptions in timeout/size math
  Several expressions multiply/add in `int` before widening (`count_active.c`,
  `parse.c`, `simulate.c`). Current constants are small, but this pattern can
  hide overflow/truncation if bounds/constants grow or platform widths change.

- [UNVERIFIED][LOW] post-addition cast in `list_files()` capacity check
  `simulate.c` casts `(dlen + pos)` to `size_t` after the addition. If signed
  addition ever overflows, the cast is too late to preserve correctness.

- [UNVERIFIED][LOW] `tail()` offset arithmetic relies on implicit `int` math
  `simulate.c` computes `offset = st.st_size - 54 * 20`; constants currently
  keep this safe, but the pattern relies on implicit `int` arithmetic before
  conversion to `off_t`.

## Fixed Security Bugs

- [FIXED] say()/tell_room() missing NUL termination after strncpy.
- [FIXED] list_files() out-of-bounds access on short names and truncation safety.
- [FIXED] list_files() directory handle leak on early return.
- [FIXED] restore_object() early-return leaks and short-name indexing guard.
- [FIXED] ACCESS.DENY parser hardening and getpeername length initialization.
- [FIXED] startup hard-fail when hostname is unresolved.
- [FIXED] remote memory corruption in socket input buffering (newline expansion overflow).
- [FIXED] privileged efuns missing driver-level authorization gates.
- [FIXED] command injection in create_wizard() via system().
- [FIXED] ed() initial file open path bypass of standard file-name mediation.
- [FIXED] listener binds all interfaces by default (now loopback by default).
- [FIXED] parse.c UB paths in plural/name formatting.
- [FIXED] transfer() container policy bypass via can_put_and_get() guard logic (SEC-001).
- [FIXED] wizard save_object() namespace boundary bypass (prefix-match weakness, SEC-002).

## Fixed CI/Static-Analysis Bugs

- [FIXED] H-03 null-safe swap area growth copy path in swap.c::ass_size.
- [FIXED] H-04 defined L_BLOCK load path when no block payload is present.
- [FIXED] M-13 precompiler fork/dup2 control-flow hardening in simulate.c.
- [FIXED] M-08 null-safe command error reporting in simulate.c.
- [FIXED] M-06/M-07 stream-state handling improvements in simulate.c::tail.
- [FIXED] M-10/M-11 stream-state handling improvements in ed.c::egets.
- [FIXED] M-05 null guard for sscanf format string in interpret.c.
- [FIXED] M-12 safer adjective parser loop guards in parse.c.
- [FIXED] M-14 removed yacc shift/reduce conflict by explicit if/else precedence in lang.y.
- [FIXED] Previously resolved E-01..E-14.
- [FIXED] Previously resolved M-01 copy_svalue() undefined return path.
- [FIXED] Previously resolved M-02/M-03/M-04 current_object null-deref guards in interpreter.

## Fixed Bugs From Repository History

- [FIXED] player_parser() control-flow bug in simulate.c (mis-scoped branch/continue path causing command-dispatch logic errors).
- [FIXED] test runner scripts relied on broken rg invocation on some environments; corrected to portable grep usage.

## Non-Bug / Technical-Debt / Feature Requests

- struct lnode model remains cast-heavy (cleanup/design debt).
- predefined identifier lookup is not hash-based (cleanup/perf debt).
- static string_space table still exists (cleanup debt).
- malloc.c free-list recombination scheduling note (legacy MALLOC=malloc context).
- "programming language should be better documented" (documentation quality item).
- "too much inflation in the game" (mudlib gameplay/economy concern).
- add TFTP protocol for wizard file transfer (feature request, not a bug).
- "all calls to xalloc() should be replaced ..." (historical stale note).
