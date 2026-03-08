# TLS/SSL Support in `parse`

This document explains the optional TLS support added to the LPmud driver (`parse`): what it does, how to build and run it, and how to test it safely without regressing non-TLS behavior.

## 1) What Was Added

TLS support is optional and controlled by:

- Build-time flag: `USE_SSL=1`
- Runtime flag: `MUD_SSL=1`

When enabled, the driver:

- initializes an OpenSSL server context at startup,
- accepts TLS handshakes per player connection,
- uses TLS-aware read/write paths for interactive sockets,
- keeps plain TCP behavior unchanged when TLS is disabled.

Key implementation files:

- `comm1.c` (TLS init, handshake integration, read/write wrappers)
- `comm.h` (per-connection TLS state in `struct interactive`)
- `Makefile` (`USE_SSL` compile/link wiring)

## 2) Security and Behavior Model

- Default runtime remains plain TCP (same as historical behavior).
- TLS is only active when both:
  - binary is built with `USE_SSL=1`, and
  - runtime sets `MUD_SSL=1`.
- If `MUD_SSL=1` is set on a non-SSL build, startup fails fast with a clear fatal error.
- Player `logon()` is deferred until the TLS handshake completes.
- No static keypairs are committed in this repo.

## 3) Environment Variables

### Core networking

- `MUD_BIND_ADDR`
  - Listener bind address.
  - Defaults to `127.0.0.1`.
  - Use `*` or `0.0.0.0` to bind all interfaces.

### TLS control

- `MUD_SSL`
  - Enables TLS when truthy.
  - Treated as false if set to: `0`, `false`, `no`, or `off` (case-insensitive).
  - Any other non-empty value enables TLS.

- `MUD_SSL_CERT_FILE` (required when `MUD_SSL=1`)
  - PEM certificate path.

- `MUD_SSL_KEY_FILE` (required when `MUD_SSL=1`)
  - PEM private key path.

- `MUD_SSL_CA_FILE` (optional)
  - PEM CA bundle for client certificate verification.

- `MUD_SSL_VERIFY_CLIENT` (optional)
  - When truthy, enables client certificate verification.
  - Requires `MUD_SSL_CA_FILE`.

## 4) Build Instructions

All commands below are run from `lpmud/`.

### Build without TLS (default)

```bash
make clean
make parse
```

### Build with TLS support

```bash
make clean
make parse USE_SSL=1
```

Requirements for `USE_SSL=1`:

- OpenSSL development headers and libraries available to the compiler/linker.
- If OpenSSL is installed in a non-default location, set `OPENSSL_ROOT`, for example:

```bash
make parse USE_SSL=1 OPENSSL_ROOT=/opt/homebrew/opt/openssl@3
```

## 5) Run Instructions

### Run plain TCP mode

```bash
MUD_BIND_ADDR=127.0.0.1 MUD_LIB=./mudlib ./parse
```

### Run TLS mode

```bash
MUD_SSL=1 \
MUD_SSL_CERT_FILE=/absolute/path/server.crt \
MUD_SSL_KEY_FILE=/absolute/path/server.key \
MUD_BIND_ADDR=127.0.0.1 \
MUD_LIB=./mudlib \
./parse
```

### Quick local self-signed cert generation

```bash
mkdir -p tests/tls
openssl req -x509 -newkey rsa:2048 -nodes \
  -keyout tests/tls/server.key \
  -out tests/tls/server.crt \
  -days 1 \
  -subj "/CN=localhost"
```

## 6) Testing Strategy

Run non-TLS regression tests first, then TLS tests.

### A) Non-TLS regression (must remain green)

```bash
./tests/run_test_mudlib_suite.sh
./tests/run_engine_functional_suite.sh
```

Expected:

- `test-mudlib` summary reports `fail=0`.
- engine functional suite completes with no `FAIL:` lines in expect logs.

### B) TLS functional test (`test-mudlib`)

```bash
./tests/run_test_mudlib_ssl_suite.sh
```

What this script does:

- cleans and rebuilds `parse` with `USE_SSL=1`,
- creates an ephemeral self-signed cert for that test run,
- starts `parse` with `MUD_SSL=1`,
- drives the `runtests` flow over TLS using `openssl s_client` + `expect`.

Artifacts:

- `tests/reports/test_mudlib_ssl_suite_<timestamp>/`
- includes `make_clean_ssl.log`, `make_parse_ssl.log`, `openssl_req.log`, `parse.log`, `expect.log`, `session.log`, and `SUMMARY.txt`.

## 7) CI/Static Quality Gates

For strict compile/analyze validation:

```bash
make ci-warnings
make ci-analyze
```

These gates are used to verify:

- `-Wall -Wextra -Wpedantic` clean builds (`-Werror` path),
- static analyzer checks over maintained sources.

## 8) Troubleshooting

### `MUD_SSL=1 requested, but parse was built without USE_SSL=1.`

- Rebuild with `make parse USE_SSL=1`.

### `fatal error: 'openssl/err.h' file not found`

- Install OpenSSL development headers and libraries.
- Rebuild with `USE_SSL=1`.
- If needed, pass `OPENSSL_ROOT` to point at the install root.

### `library 'ssl' not found` at link time

- OpenSSL libraries are not on the linker path.
- Install OpenSSL dev libs and/or set `OPENSSL_ROOT`.

### TLS test script fails before runtime

- Check `make_parse_ssl.log` in the generated report directory.
- Most early failures are dependency/toolchain related, not runtime TLS logic.

## 9) Operational Notes

- Keep default deployments in plain TCP unless TLS is required.
- For staged rollout:
  1. build with `USE_SSL=1`,
  2. validate non-TLS regressions,
  3. run TLS test suite,
  4. enable `MUD_SSL=1` in staging first.

- Runtime rollback is immediate by setting `MUD_SSL=0` (or unsetting) and restarting the process (assuming binary still supports both modes).
