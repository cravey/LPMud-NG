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
- Supporting docs: [QUICKSTART.md](QUICKSTART.md), [DEPENDENCIES.md](DEPENDENCIES.md), [SECURITY.md](SECURITY.md), [BUGS.md](BUGS.md), [SSL.md](SSL.md)

---

## 2) External Build Dependencies

Dependency details are maintained in [DEPENDENCIES.md](DEPENDENCIES.md), including:

- build-required toolchain,
- Make target specific tooling (`clang`, `lint`, `etags`),
- test/runtime script dependencies,
- platform package-install examples.

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
- `make ci-tidy`: calibrated clang-tidy checks for high-signal bug/security patterns
- `make ci`: run all three validation gates
  - note: `ci-tidy` is intentionally standalone and not included in `make ci`

Linux note for `ci-sanitize`:

- `ci-sanitize` now preserves platform link libraries (including Linux `-lcrypt`) while adding sanitizer link flags.

### Driver Suite Convenience Target

- `make mudlibtest`: runs `./tests/run_test_mudlib_suite.sh` and prints suite summary
- `make securitytest`: runs `./tests/run_driver_security_suite.sh` and prints integration-security summary

### CI vs Test-Suite Scope

| Command | What it validates | Launches driver runtime? | Uses expect/network harness? |
|---|---|---|---|
| `make ci-warnings` | compile warnings (`-Wall -Wextra -Wpedantic -Werror`) | no | no |
| `make ci-sanitize` | sanitizer compile/link gate | no | no |
| `make ci-analyze` | static analysis (`clang --analyze`) | no | no |
| `make ci-tidy` | calibrated static lint (`clang-tidy`) | no | no |
| `make ci` | all three CI compile/analyze gates above | no | no |
| `make mudlibtest` | dedicated `test-mudlib` behavioral suite | yes | yes |
| `make securitytest` | driver/network security integration suite | yes | yes |

If you need stronger static coverage, run `make ci-tidy` in addition to `make ci`.
If you need behavioral runtime validation, run `make mudlibtest` and `make securitytest` as well.

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
