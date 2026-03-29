# LPmud Driver Security Review

- **Date:** 2026-02-21
- **Last Updated:** 2026-03-29 (includes ci-tidy follow-up triage and open security-adjacent issues under investigation)


## Scope
- Driver/runtime C sources in repo root (not mudlib LPC gameplay logic).
- Focus: memory safety, privilege boundaries, file/path safety, and remote attack surface.

## Fixed In Current Branch

### 1. say()/tell_room() missing NUL termination after strncpy
- **Status:** Fixed
- **Risk class:** Memory safety / possible stack over-read in message handling.
- **What this vulnerability meant:**
  - Message buffers copied with `strncpy` were not always explicitly NUL-terminated.
  - Later string operations could read beyond intended bounds into adjacent memory.
- **How an attacker could exploit it (pre-fix):**
  - Send crafted long chat/message payloads to trigger over-read behavior in message formatting paths.
  - Convert routine messaging into a crash vector (denial of service), with possible accidental memory disclosure in edge cases.
- **Practical attacker prerequisites:**
  - Ability to send message content that reaches `say()`/`tell_room()` paths.
- **Security impact:**
  - Process instability/crash risk, plus potential out-of-bounds read side effects.
- **Fix references:**
  - simulate.c:905
  - simulate.c:906
  - simulate.c:909
  - simulate.c:910

### 2. list_files() out-of-bounds access on short names and truncation safety
- **Status:** Fixed
- **Risk class:** Memory safety / undefined behavior.
- **What this vulnerability meant:**
  - Filename handling in `list_files()` used unsafe indexing/truncation assumptions for short or edge-length names.
  - This could trigger out-of-bounds access or undefined behavior during directory listing.
- **How an attacker could exploit it (pre-fix):**
  - Create specially shaped filenames in a directory that gets listed by LPC/admin flows.
  - Trigger listing repeatedly to crash the driver or produce corrupted output paths.
- **Practical attacker prerequisites:**
  - Ability to influence filenames in a listed directory and trigger `list_files()` behavior.
- **Security impact:**
  - Driver crash risk and integrity/correctness issues in file listing output.
- **Fix references:**
  - simulate.c:1150
  - simulate.c:1183
  - simulate.c:1227

### 3. list_files() directory handle leak on early return
- **Status:** Fixed
- **Risk class:** Resource leak / availability degradation.
- **What this vulnerability meant:**
  - Some error/early-return branches in `list_files()` skipped `closedir()`.
  - Repeated hits leaked file descriptors over time.
- **How an attacker could exploit it (pre-fix):**
  - Repeatedly invoke list operations along leaking branches.
  - Exhaust process file descriptors and degrade service until failures or crash.
- **Practical attacker prerequisites:**
  - Ability to trigger directory listing operations frequently.
- **Security impact:**
  - Denial of service via resource exhaustion.
- **Fix references:**
  - simulate.c:1165
  - simulate.c:1166
  - simulate.c:1237

### 4. restore_object() early-return leaks and short-name indexing guard
- **Status:** Fixed
- **Risk class:** Resource leak + potential out-of-bounds read.
- **What this vulnerability meant:**
  - `restore_object()` had early exits that leaked allocated buffers/file handles.
  - Short-name extension handling had unsafe indexing assumptions.
- **How an attacker could exploit it (pre-fix):**
  - Trigger repeated restore failures (malformed or missing files) to leak resources.
  - Feed boundary-case paths to exercise unsafe indexing and destabilize restore flow.
- **Practical attacker prerequisites:**
  - Ability to invoke restore paths with attacker-influenced file names/content.
- **Security impact:**
  - Gradual resource exhaustion and potential crash/undefined behavior in restore handling.
- **Fix references:**
  - object.c:116
  - object.c:127
  - object.c:133
  - object.c:136
  - object.c:139
  - object.c:179
  - object.c:181

### 5. ACCESS.DENY parser hardening and getpeername length initialization
- **Status:** Fixed
- **Risk class:** Parser robustness / undefined behavior risk.
- **What this vulnerability meant:**
  - ACCESS.DENY parsing accepted weak/malformed cases, and socket peer-name length handling was unsafe.
  - Uninitialized/incorrect length state around `getpeername()` could lead to undefined behavior.
- **How an attacker could exploit it (pre-fix):**
  - Abuse malformed ACL entries or edge-case connection metadata to cause unpredictable access-control behavior.
  - In worst case, convert parser/UB behavior into availability issues.
- **Practical attacker prerequisites:**
  - Ability to influence ACCESS.DENY content and/or repeatedly exercise connection admission checks.
- **Security impact:**
  - Access-control reliability issues and potential denial-of-service conditions.
- **Fix references:**
  - comm1.c:597
  - comm1.c:621
  - comm1.c:627
  - comm1.c:630
  - comm1.c:662

### 6. Startup hard-fail when hostname is unresolved
- **Status:** Fixed
- **Risk class:** Availability / deployment fragility.
- **What this vulnerability meant:**
  - Driver startup depended on successful hostname resolution and could hard-fail when resolution was unavailable.
- **How an attacker could exploit it (pre-fix):**
  - Influence DNS/resolver conditions to force startup failure after restart.
  - Turn transient resolver outages into durable service downtime.
- **Practical attacker prerequisites:**
  - Ability to affect resolver/network conditions (or an unreliable host resolver environment).
- **Security impact:**
  - Service unavailability during boot/restart windows.
- **Fix references:**
  - comm1.c:71

### 7. Remote memory corruption in socket input buffering (newline expansion overflow)
- **Status:** Fixed
- **Risk class:** Critical memory safety.
- **What this vulnerability meant:**
  - Input normalization expanded newlines without correctly enforcing remaining per-user buffer capacity.
  - Remote client input could overflow socket/user buffers during line processing.
- **How an attacker could exploit it (pre-fix):**
  - Send crafted long interactive payloads that maximize expansion behavior.
  - Corrupt memory and force crashes; depending on build/allocator/platform, potential code-execution conditions existed.
- **Practical attacker prerequisites:**
  - Network reachability to the listener and ability to send raw interactive input.
- **Security impact:**
  - Critical remote memory-corruption exposure (DoS; possible RCE class).
- **Fix summary:** input normalization is now bounded by remaining per-user buffer space; overflow attempts stop appending and connection is marked for close.
- **Fix references:**
  - comm1.c:246
  - comm1.c:254
  - comm1.c:355
  - comm1.c:421

### 8. Privileged efuns missing driver-level authorization gates
- **Status:** Fixed
- **Risk class:** Critical privilege boundary.
- **What this vulnerability meant:**
  - Sensitive efuns (`snoop`, `shutdown`, `ed`, wizard creation path) lacked strict, centralized driver-level authorization checks.
  - Safety depended too heavily on mudlib-side assumptions.
- **How an attacker could exploit it (pre-fix):**
  - Invoke privileged efuns from unauthorized LPC contexts.
  - Abuse `snoop` for credential/session surveillance, `shutdown` for DoS, and `ed`/wizard flows for unauthorized administrative actions.
- **Practical attacker prerequisites:**
  - Ability to execute LPC in a context that can reach the relevant efuns/commands.
- **Security impact:**
  - Privilege escalation and administrative boundary compromise.
- **Fix summary:** added explicit level checks for `snoop`, `shutdown`, and `ed`; `create_wizard` restricted to trusted caller object.
- **Fix references:**
  - simulate.c:860
  - simulate.c:1394
  - simulate.c:1409
  - simulate.c:1422
  - simulate.c:1475

### 9. Command injection in create_wizard() via system()
- **Status:** Fixed
- **Risk class:** Critical host command execution.
- **What this vulnerability meant:**
  - `create_wizard()` built shell commands from attacker-influenced input and executed them via `system()`.
- **How an attacker could exploit it (pre-fix):**
  - Supply wizard names containing shell metacharacters.
  - Break out of intended command semantics and execute arbitrary host shell commands.
- **Practical attacker prerequisites:**
  - Ability to reach the wizard-creation path with controlled name input.
- **Security impact:**
  - Host-level command execution under driver process privileges.
- **Fix summary:** removed `system()` and shell command construction; replaced with direct file append via stdio and strict owner validation.
- **Fix references:**
  - simulate.c:796
  - simulate.c:801
  - simulate.c:835

### 10. ed() initial file open path bypass of standard file-name mediation
- **Status:** Fixed
- **Risk class:** High path policy bypass.
- **What this vulnerability meant:**
  - `ed(file)` did not consistently enforce `check_file_name()` / `legal_path()` on initial open.
  - This weakened standard mudlib file policy mediation on editor entry.
- **How an attacker could exploit it (pre-fix):**
  - Open files outside authorized policy boundaries.
  - Use editor flows to read/modify unexpected files if mudlib relied on mediation hooks for enforcement.
- **Practical attacker prerequisites:**
  - Ability to invoke `ed()` with controlled path input.
- **Security impact:**
  - Unauthorized file access and potential data tampering.
- **Fix summary:** `ed(file)` now passes through `check_file_name()` and `legal_path()` before opening.
- **Fix references:**
  - simulate.c:1427
  - simulate.c:1428
  - simulate.c:1431

### 11. Listener binds all interfaces by default
- **Status:** Fixed
- **Risk class:** Medium exposure amplifier.
- **What this vulnerability meant:**
  - Default listener behavior exposed the service on all interfaces unless explicitly restricted.
  - Operators could unintentionally publish a local/testing instance to external networks.
- **How an attacker could exploit it (pre-fix):**
  - Connect remotely to unintended exposed instances.
  - Chain with weak passwords or other bugs to move from accidental exposure to compromise.
- **Practical attacker prerequisites:**
  - Network path to exposed host/port.
- **Security impact:**
  - Expanded remote attack surface and higher probability of external abuse.
- **Fix summary:** default bind address changed to loopback (`127.0.0.1`) with explicit opt-in to all interfaces via `MUD_BIND_ADDR=*` or `0.0.0.0`.
- **Fix references:**
  - comm1.c:76
  - comm1.c:77
  - comm1.c:79

### 12. parse.c UB paths in plural/name formatting
- **Status:** Fixed
- **Risk class:** Medium stability/correctness; low direct integrity impact.
- **What this vulnerability meant:**
  - Plural/name formatting logic in parser code had undefined-behavior paths under edge input/state combinations.
- **How an attacker could exploit it (pre-fix):**
  - Feed crafted parser text patterns to trigger unstable formatting behavior.
  - Primarily turn parser paths into crash/instability vectors rather than privilege escalation.
- **Practical attacker prerequisites:**
  - Ability to submit parser-relevant interactive text.
- **Security impact:**
  - Reliability degradation and possible denial-of-service by malformed input pressure.
- **Fix references:**
  - parse.c:1291
  - parse.c:1294
  - parse.c:1364

### 13. transfer() container policy bypass via can_put_and_get() guard logic
- **Status:** Fixed
- **Risk class:** High privilege/integrity boundary bypass.
- **What this vulnerability meant:**
  - Driver-level policy checks around `can_put_and_get()` were too weak in `transfer_object()`.
  - Containers signaling "deny put/get" could still be treated as transferable in some return paths.
- **How an attacker could exploit it (pre-fix):**
  - Move items out of containers that should block extraction.
  - Insert items into containers that should block insertion.
  - Abuse any mudlib command path that relies on driver `transfer()` behavior for inventory authorization.
  - If a malicious LPC object could call `transfer()`, it could bypass intended container policy hooks.
- **Practical attacker prerequisites:**
  - Any actor who can reach a `transfer()` code path (player command flow or hostile LPC object).
- **Security impact:**
  - Unauthorized item movement, theft-style behavior, and inventory state integrity violations.
- **Fix summary:** source/destination container checks in `transfer_object()` now require numeric truthy return from `can_put_and_get()`; zero return is denied.
- **Fix references:**
  - simulate.c:2554
  - simulate.c:2568
- **Validation references:**
  - test-mudlib ID `SEC-001`

### 14. wizard save_object() namespace boundary bypass (prefix-match weakness)
- **Status:** Fixed
- **Risk class:** High filesystem integrity boundary bypass.
- **What this vulnerability meant:**
  - Wizard save path ownership used a prefix match (`players/<name...>`) instead of an exact path-segment boundary.
  - A wizard named `alice` could pass checks for paths beginning with `players/alice2`.
- **How an attacker could exploit it (pre-fix):**
  - Write or overwrite persistent save files outside their own namespace.
  - Corrupt or replace another wizard/account state file.
  - Plant crafted persistent data that could later be consumed by `restore_object()` paths.
- **Practical attacker prerequisites:**
  - Wizard-level LPC execution context (or compromised wizard account/object) with access to `save_object()`.
- **Security impact:**
  - Cross-namespace persistent-data tampering, integrity loss, and potential follow-on privilege abuse via poisoned state.
- **Fix summary:** wizard save paths now require an exact `players/<wizard>` segment boundary, blocking prefix collisions like `players/alice2` for wizard `alice`.
- **Fix references:**
  - object.c:65
- **Validation references:**
  - test-mudlib ID `SEC-002`

## Security Regression Coverage (Current)

- `SEC-001`: covered in default `runtests` (`test_mudlib`).
- `SEC-002`: covered in default `runtests` (`test_mudlib`).
- `SEC-003`: covered in default `runtests` (`test_mudlib`) for privileged efun gates.
- `SEC-004`: intentionally quarantined from default `runtests`; exercised via isolated hazard command.
- `SEC-005`: covered in default `runtests` (`test_mudlib`) for bounded long-message handling.
- `SEC-006`: covered in default `runtests` (`test_mudlib`) for `list_files` short/long-name stability.
- `SEC-007`: covered in default `runtests` (`test_mudlib`) for `restore_object` boundary-shaped input.
- `SEC-008`: covered in default `runtests` (`test_mudlib`) for `create_wizard` metachar payload rejection.
- `SEC-009`: covered in default `runtests` (`test_mudlib`) for parser plural/name boundedness.
- `SECNET-001`: covered by `tests/run_driver_security_suite.sh` (listener bind-default integration check).
- `SECNET-002`: covered by `tests/run_driver_security_suite.sh` (malformed `ACCESS.DENY` admission robustness).
- `SECNET-003`: covered by `tests/run_driver_security_suite.sh` (newline-heavy socket payload resilience).
- `SECNET-004`: tracked in the integration suite as environment-dependent; currently `SKIP` in generic harness runs.

## Potential Security Issues Under Investigation

### 15. restore_object() variable-name parse may write past fixed stack buffer
- **Status:** Open (confirmed bug; not yet patched)
- **Risk class:** High memory safety (local file-content-triggered corruption).
- **What this vulnerability means:**
  - `restore_object()` parses a variable token into `char var[100]` using an
    unbounded copy length derived from input (`space - buff`).
  - If the token before the first space exceeds 99 bytes, `var[space - buff]`
    can write out of bounds on stack memory.
- **How an attacker could exploit it (current branch):**
  - Provide crafted object-save content with an overlong field name token.
  - Trigger restore on that file to corrupt stack state and crash the driver;
    exploitability beyond DoS depends on platform/compiler hardening.
- **Practical attacker prerequisites:**
  - Ability to influence restore-file contents and trigger `restore_object()`.
- **Security impact:**
  - Potential denial of service; possible memory-corruption class escalation in
    weaker hardening environments.
- **Code references:**
  - object.c:158
  - object.c:159
- **Tracking references:**
  - BUGS.md `[UNFIXED][HIGH]` entry.

### 16. signal-handler shutdown path may violate async-signal-safety constraints
- **Status:** Under investigation
- **Risk class:** Medium availability/reliability.
- **What this issue means:**
  - `backend.c` installs `startshutdowngame` as a `SIGHUP` handler directly.
  - If that path (or callees) performs non-async-signal-safe operations, behavior
    in signal context is undefined and may deadlock or crash.
- **How this could be abused:**
  - Repeated signal delivery during busy runtime windows can destabilize control
    flow and force shutdown/crash behavior.
- **Practical attacker prerequisites:**
  - Ability to send process signals (typically local operator/co-tenant scope).
- **Security impact:**
  - Availability degradation; primarily DoS-class risk.
- **Code references:**
  - backend.c:126
- **Tracking references:**
  - BUGS.md `[UNVERIFIED][MEDIUM]` entry.

### 17. unchecked numeric parsing in input/file paths (atoi/sscanf)
- **Status:** Under investigation
- **Risk class:** Medium input-validation weakness (context dependent).
- **What this issue means:**
  - Multiple paths parse numbers with `atoi()` or loose `sscanf("%d", ...)`
    checks and do not validate full-string parse success or range.
  - Malformed values may be accepted silently as `0`/truncated integers.
- **How this could be abused:**
  - Feed malformed numeric fields in command or file-driven paths to force
    unexpected control behavior, misconfiguration of limits, or logic bypass
    in edge cases.
- **Practical attacker prerequisites:**
  - Access to affected input surfaces (`main.c`, restore/parser flows,
    user-facing command parsing, or wizlist data files).
- **Security impact:**
  - Mostly integrity/reliability risk; exploitability depends on downstream use.
- **Code references:**
  - main.c:49
  - object.c:171
  - parse.c:513
  - parse.c:1311
  - simulate.c:606
  - wiz_list.c:139
  - wiz_list.c:142
- **Tracking references:**
  - BUGS.md `[UNVERIFIED][MEDIUM]` entry.

## Remaining Issues (Ordered By Risk To System Data Integrity)

### 1. Open high-confidence memory-safety issue in `restore_object()` parse path (see item 15).

### 2. Security-adjacent hardening work remains for signal-handler safety and checked numeric parsing (items 16-17).

- Note: `SEC-001` and `SEC-002` remain fixed in this branch; the open items above were identified in later static-analysis follow-up.

## Residual Risk Notes
- Mudlib LPC policy (`valid_read`, `valid_write`, command-level authorization) still determines real-world security posture.
- Filesystem permissions for runtime directories (players/, log/, etc.) remain operationally critical.
- Additional fuzzing of parser/interactive input and a dedicated LPC-level permission audit are recommended.
