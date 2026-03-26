# LPMud-NG Driver and Mudlib

## 1) Repository Summary

This directory is the main LPmud runtime workspace and contains:

- the C mud driver (`parse`) and build system,
- the gameplay mudlib (`mudlib/`),
- a dedicated deterministic test mudlib (`test-mudlib/`),
- test/analysis/fuzz/mutation tooling (`tests/`).

Major components:

- Build system: `Makefile`
- Driver source: `main.c`, `backend.c`, `interpret.c`, `simulate.c`, `comm1.c`, and related C files
- Runtime content: `mudlib/`
- Driver-focused test content: `test-mudlib/`
- Test harness and utilities: `tests/`
- Additional docs: `Quickstart.md`, `Security.txt`, `SSL.md`

## 2) External Build Dependencies

Required for standard builds:

- `make`
- C compiler with C23 support (`cc`, `clang`, or `gcc`)
- `lex` and `yacc` (or compatible replacements, such as `flex` and `bison`)
- standard C/POSIX headers and libraries
- `libm` (`-lm`)
- Linux: `libcrypt` development support (`-lcrypt` is linked on Linux)

Optional build dependencies:

- OpenSSL development headers/libraries for SSL builds (`USE_SSL=1`)
- `clang` for `make ci-analyze`

Common non-build test/runtime tools:

- `python3`, `expect`, `nc`, `openssl`, `lsof`, `uuidgen`

## 3) Basic Build Instructions and Major Make Targets

Run from this directory (`lpmud/`).

Core targets:

- `make parse`: build the main mud driver
- `make all`: build `parse` and `count_active`
- `make count_active`: build helper utility
- `make debug`: build debug binary
- `make clean`: remove build artifacts and `tests/reports/*`

Validation and CI-style targets:

- `make ci-warnings`: strict warnings gate (`-Wall -Wextra -Wpedantic -Werror`)
- `make ci-sanitize`: AddressSanitizer/UBSan build gate
- `make ci-analyze`: static analysis (`clang --analyze`)
- `make ci`: run all three gates above

Driver suite convenience target:

- `make mudlibtest`: runs `./tests/run_test_mudlib_suite.sh` and prints suite summary

SSL build (optional):

```bash
make parse USE_SSL=1
```

If OpenSSL is in a non-default prefix:

```bash
make parse USE_SSL=1 OPENSSL_ROOT=/path/to/openssl
```

For full SSL details, see [SSL.md](SSL.md).

## 4) Basic Deployment and Critical Environment Variables

Basic local deployment:

```bash
make parse
MUD_LIB=./mudlib MUD_BIND_ADDR=127.0.0.1 ./parse
```

Notes:

- If `MUD_LIB` is unset, startup defaults to `./mudlib`.
- If `MUD_BIND_ADDR` is unset, listener defaults to `127.0.0.1`.
- You can pass a port as arg1, for example `./parse 2000`.

Critical runtime environment variables:

- `MUD_LIB`: mudlib directory to `chdir` into at startup
- `MUD_BIND_ADDR`: bind address (`127.0.0.1` default, `0.0.0.0` or `*` for all interfaces)
- `MUD_SSL`: enable TLS mode when truthy
- `MUD_SSL_CERT_FILE`: PEM server certificate path (required when `MUD_SSL=1`)
- `MUD_SSL_KEY_FILE`: PEM private key path (required when `MUD_SSL=1`)
- `MUD_SSL_CA_FILE`: PEM CA bundle path (optional; used with client cert verification)
- `MUD_SSL_VERIFY_CLIENT`: enable client cert verification (requires `MUD_SSL_CA_FILE`)

TLS deployment example:

```bash
make parse USE_SSL=1
MUD_SSL=1 \
MUD_SSL_CERT_FILE=/abs/path/server.crt \
MUD_SSL_KEY_FILE=/abs/path/server.key \
MUD_LIB=./mudlib \
MUD_BIND_ADDR=127.0.0.1 \
./parse
```

See [SSL.md](SSL.md) for certificate creation, compatible clients, and SSL maintenance guidance.
