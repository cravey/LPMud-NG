# LPMud-NG Driver and Mudlib

This directory is the primary runtime workspace for the LPmud driver (`parse`) and mudlib content.

---

## 1) Repository Summary

This directory contains:

- the C mud driver and build system,
- gameplay mudlib content under `mudlib/`,
- dedicated deterministic driver test content under `test-mudlib/`,
- automated validation and analysis tooling under `tests/`.

### Major Components

- Build system: `Makefile`
- Driver source: `main.c`, `backend.c`, `interpret.c`, `simulate.c`, `comm1.c`, and related C files
- Runtime content: `mudlib/`
- Driver-focused test content: `test-mudlib/`
- Test harness and utilities: `tests/`
- Supporting docs: `Quickstart.md`, `Security.txt`, `SSL.md`

---

## 2) External Build Dependencies

### Required for Standard Builds

- `make`
- C compiler with C23 support (`cc`, `clang`, or `gcc`)
- `lex` and `yacc` (or compatible replacements such as `flex` and `bison`)
- standard C/POSIX headers and libraries
- `libm` (`-lm`)
- Linux: `libcrypt` development support (`-lcrypt` is linked on Linux)

### Optional Build Dependencies

- OpenSSL development headers/libraries for SSL builds (`USE_SSL=1`)
- `clang` for `make ci-analyze`

### Common Non-Build Test/Runtime Tools

- `python3`, `expect`, `nc`, `openssl`, `lsof`, `uuidgen`

---

## 3) Basic Build Instructions and Major Make Targets

Run commands from this directory (`lpmud/`).

### Core Build Targets

- `make parse`: build the main mud driver
- `make all`: build `parse` and `count_active`
- `make count_active`: build helper utility
- `make debug`: build debug binary
- `make clean`: remove build artifacts and `tests/reports/*`

### Validation Targets

- `make ci-warnings`: strict warning gate (`-Wall -Wextra -Wpedantic -Werror`)
- `make ci-sanitize`: AddressSanitizer/UBSan build gate
- `make ci-analyze`: static analysis (`clang --analyze`)
- `make ci`: run all three validation gates

### Driver Suite Convenience Target

- `make mudlibtest`: runs `./tests/run_test_mudlib_suite.sh` and prints suite summary

### SSL Build (Optional)

```bash
make parse USE_SSL=1
```

If OpenSSL is in a non-default prefix:

```bash
make parse USE_SSL=1 OPENSSL_ROOT=/path/to/openssl
```

For complete SSL details, see [SSL.md](SSL.md).

---

## 4) Basic Deployment and Critical Environment Variables

### Basic Local Deployment

```bash
make parse
MUD_LIB=./mudlib MUD_BIND_ADDR=127.0.0.1 ./parse
```

Defaults and notes:

- if `MUD_LIB` is unset, startup defaults to `./mudlib`
- if `MUD_BIND_ADDR` is unset, listener defaults to `127.0.0.1`
- you can pass a port as arg1, for example: `./parse 2000`

### Critical Runtime Environment Variables

- `MUD_LIB`: mudlib directory to `chdir` into at startup
- `MUD_BIND_ADDR`: bind address (`127.0.0.1` default, `0.0.0.0` or `*` for all interfaces)
- `MUD_SSL`: enable TLS mode when truthy
- `MUD_SSL_CERT_FILE`: PEM server certificate path (required when `MUD_SSL=1`)
- `MUD_SSL_KEY_FILE`: PEM private key path (required when `MUD_SSL=1`)
- `MUD_SSL_CA_FILE`: PEM CA bundle path (optional, used with client cert verification)
- `MUD_SSL_VERIFY_CLIENT`: enable client cert verification (requires `MUD_SSL_CA_FILE`)

### TLS Deployment Example

```bash
make parse USE_SSL=1
MUD_SSL=1 \
MUD_SSL_CERT_FILE=/abs/path/server.crt \
MUD_SSL_KEY_FILE=/abs/path/server.key \
MUD_LIB=./mudlib \
MUD_BIND_ADDR=127.0.0.1 \
./parse
```

For certificate creation, compatible clients, and SSL maintenance guidance, see [SSL.md](SSL.md).
