# External Dependencies

This document lists external tools and libraries used by the `lpmud/` build and test workflow.

---

## 1) Build-Critical Dependencies

These are required for standard `make parse` / `make all` builds:

- `make`
- C compiler with C23 support (`cc`, `clang`, or `gcc`)
- `lex` and `yacc` compatible tooling
  - defaults in `Makefile`: `LEX=lex`, `YFLAGS=-d`
  - common replacements: `flex` and `bison`
- standard C/POSIX headers and libc development support
- math library (`-lm`)
- Linux only: `libcrypt` development support (`-lcrypt` linked on Linux)

Optional build extension:

- OpenSSL development headers/libraries for `USE_SSL=1`

---

## 2) Make Target Specific Tools

| Make target | External tool(s) | Required? | Source |
|---|---|---|---|
| `make parse`, `make all`, `make debug`, `make count_active` | compiler toolchain + lex/yacc family | yes | `Makefile` build rules |
| `make ci-warnings` | same as build toolchain | yes | `Makefile` |
| `make ci-sanitize` | compiler with sanitizer support | yes | `Makefile` |
| `make ci-analyze` | `clang` | yes for this target | `Makefile` checks `command -v clang` |
| `make ci-tidy` | `clang-tidy` | yes for this target | `Makefile` prefers `/opt/homebrew/opt/llvm/bin/clang-tidy`, falls back to `command -v clang-tidy` |
| `make ci` | inherits `ci-*` dependencies | yes | `Makefile` |
| `make mudlibtest` | test-suite script dependencies | yes for this target | `tests/run_test_mudlib_suite.sh` |
| `make securitytest` | test-suite script dependencies | yes for this target | `tests/run_driver_security_suite.sh` |
| `make lint` | `lint` | yes for this target | `Makefile` `lint: *.c` |
| `make TAGS` | `etags` | yes for this target | `Makefile` `TAGS: etags $(SRC)` |
| `make diff` | `make_diffs` helper command | yes for this target | `Makefile` `diff:` rule |

Notes:

- `rg`/ripgrep is not required by current make targets or test scripts.
- `ctags` is not currently used by `Makefile`; tag generation target uses `etags`.

---

## 3) Test and Runtime Script Dependencies

### Core test harness tools

- `bash`
- `python3`
- `expect`
- `nc` (netcat; used for readiness checks and client sessions)
- `lsof`
- `uuidgen`
- `mktemp`

### Common shell utilities used in runners

- `grep`, `sed`, `awk`, `tail`, `sort`, `wc`, `find`, `cp`, `date`, `tr`, `cut`

### SSL test path

- `openssl`
  - required for `./tests/run_test_mudlib_ssl_suite.sh`
  - required for TLS client mode in `test_mudlib_suite.exp` (`openssl s_client`)

---

## 4) Platform Package Guidance (Typical)

Package names vary by distro/version; these are common starting points.

### macOS (Homebrew)

```bash
brew install bison flex expect openssl@3
```

Often already present via macOS/Xcode CLT:

- `make`, `clang`, BSD userland (`grep`, `sed`, `awk`, `nc`, `lsof`)

Potential extras depending on environment:

- `etags` (commonly from Emacs install)
- `lint` (provided by Xcode toolchain on many systems)

### Debian/Ubuntu

```bash
sudo apt-get update
sudo apt-get install -y build-essential bison flex clang expect netcat-openbsd \
  lsof uuid-runtime openssl libssl-dev libcrypt-dev emacs-nox
```

### Fedora/RHEL/CentOS

```bash
sudo dnf install -y gcc make bison flex clang expect nc lsof util-linux \
  openssl openssl-devel libxcrypt-devel emacs
```

---

## 5) Verify Tool Availability

From `lpmud/`, quick checks:

```bash
command -v make cc lex yacc
command -v clang expect nc lsof uuidgen mktemp
command -v clang-tidy
command -v openssl grep sed awk tail
command -v etags lint
```

For SSL header/library discovery:

```bash
make clean
make parse USE_SSL=1
```

If OpenSSL is installed in a non-default prefix:

```bash
make parse USE_SSL=1 OPENSSL_ROOT=/path/to/openssl
```

---

## 6) Relationship to Other Docs

- Build/deploy overview: [README.md](README.md)
- Driver internals and architecture: [QUICKSTART.md](QUICKSTART.md)
- TLS build and operations: [SSL.md](SSL.md)
- Test runner behavior and script relationships: [tests/README.md](tests/README.md)
