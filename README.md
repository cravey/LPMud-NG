# lpmud
lpmud 2.4.5 playground to play with Claude Code and GPT codex on a legacy codebase

## Project Modernization Summary

1. Toolchain modernization to C23
- Migrated the driver build to C23 and aligned build/test flows for modern clang toolchains.

2. Compiler warning cleanup and pedantic fixes
- Removed a broad set of `-Wall`/`-Wextra`/`-Wpedantic` issues, starting with low-risk warnings.

3. Static-analysis remediation
- Added and iterated analyzer checks, then fixed high-severity findings (H-01..H-04), including null-deref and undefined-behavior paths.

4. Runtime correctness and parser/interpreter hygiene
- Tightened interpreter/runtime error handling and control flow.
- Removed parser conflict behavior and made grammar handling explicit.

5. 64-bit portability and safety work
- Reviewed and corrected pointer/cast and memory-safety edge cases for modern 64-bit systems.

6. Precompiler and platform compatibility
- Addressed precompiler path/environment issues and modernized `cpp` assumptions for macOS/Linux/FreeBSD environments.

7. Security-focused pass
- Performed a security review, documented findings, and applied low-risk/high-confidence fixes.
- Tracking maintained in `Security.txt` and `ISSUES`.

8. CI safety gates and project hygiene
- Added/used repeatable CI gates (`ci-warnings`, `ci-sanitize`, `ci-analyze`, `ci`) and iteratively reduced findings.

9. Mudlib topology tooling
- Added a generator and artifacts for room/castle connectivity:
  - `tools/gen_mudlib_graph.pl`
  - `mudlib_rooms_castles.dot`
  - `mudlib_rooms_castles.png`

10. Current status
- Latest verification pass showed `make ci` succeeding with tracked hard analyzer issues resolved.

## Driver-Focused Test Suites

The repository includes three complementary test paths focused on code that runs on top of `parse`:

1. Dedicated LPC engine test mudlib (`lpmud/test-mudlib`)
- Purpose: deterministic, low-noise driver/efun validation through one `runtests` command.
- Entry point: `cd lpmud && ./tests/run_test_mudlib_suite.sh`
- Docs: `lpmud/test-mudlib/README.md`

2. Classic mudlib functional network suite
- Purpose: end-to-end gameplay/session checks against the regular `mudlib` using `nc` + `expect`.
- Entry point: `cd lpmud && ./tests/run_engine_functional_suite.sh`
- Harness: `lpmud/tests/nc_engine_suite.exp`

3. Static mudlib analysis tooling
- Purpose: heuristic reports for monster difficulty/weapon drops and LPC feature usage.
- Entry point: `cd lpmud && python3 tests/analyze_mudlib.py --mudlib-root mudlib --output-dir tests/reports/static_only`
- Docs: `lpmud/tests/README.md`

All generated test artifacts are written under `lpmud/tests/reports/`.

## Optional TLS (SSL) Runtime

TLS support is optional and gated by both build-time and runtime flags:

- Build-time: `USE_SSL=1`
- Runtime: `MUD_SSL=1`

Required runtime env vars when TLS is enabled:

- `MUD_SSL_CERT_FILE`
- `MUD_SSL_KEY_FILE`

Optional runtime env vars:

- `MUD_SSL_CA_FILE`
- `MUD_SSL_VERIFY_CLIENT`

### About sample keypairs

No static sample keypairs are committed in this repository.
The SSL test harness creates an ephemeral self-signed cert/key per run under:

- `lpmud/tests/reports/test_mudlib_ssl_suite_<timestamp>/tls/`

### Manual local TLS run

```bash
cd lpmud
make parse USE_SSL=1

mkdir -p tests/tls
openssl req -x509 -newkey rsa:2048 -nodes \
  -keyout tests/tls/server.key \
  -out tests/tls/server.crt \
  -days 1 \
  -subj "/CN=localhost"

MUD_SSL=1 \
MUD_SSL_CERT_FILE=tests/tls/server.crt \
MUD_SSL_KEY_FILE=tests/tls/server.key \
MUD_BIND_ADDR=127.0.0.1 \
MUD_LIB=./test-mudlib \
./parse
```

### TLS test-mudlib suite

```bash
cd lpmud
./tests/run_test_mudlib_ssl_suite.sh
```

Note: building with `USE_SSL=1` requires OpenSSL development headers/libraries.
If OpenSSL is in a non-default prefix, set `OPENSSL_ROOT`, for example:

```bash
cd lpmud
make parse USE_SSL=1 OPENSSL_ROOT=/tmp/openssl-3.6.1-local
```
