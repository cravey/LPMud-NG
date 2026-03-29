# External Dependencies

This document lists external tools and libraries used by the `lpmud/` build and test workflow.

---

## 1) Build-Critical Dependencies

These are required for standard `make parse` / `make all` builds:

- `make` (GNU Make syntax is used; on FreeBSD use `gmake`)
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
| `make ci-sanitize` | compiler with sanitizer support; Linux also needs `-lcrypt` during sanitizer link | yes | `Makefile` |
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
- `head`
- `od`
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
  lsof openssl libssl-dev libcrypt-dev emacs-nox
```

### Fedora/RHEL/CentOS

```bash
sudo dnf install -y gcc make bison flex clang expect nc lsof util-linux \
  openssl openssl-devel libxcrypt-devel emacs
```

### FreeBSD

```bash
sudo pkg install -y gmake bison flex llvm expect netcat lsof openssl emacs
```

Notes:

- Use `gmake` (or alias `make=gmake`) because this project `Makefile` uses GNU make features.
- `head` and `od` are provided by FreeBSD base system (`/usr/bin`).

---

## 5) Verify Tool Availability

From `lpmud/`, quick checks:

```bash
command -v make cc lex yacc
command -v clang expect nc lsof head od mktemp
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

## 6) `ci-sanitize` Link Behavior

`ci-sanitize` in `Makefile` runs:

```bash
$(MAKE) parse count_active CFLAGS='$(BASE_CFLAGS) $(CI_SANITIZE_FLAGS)' LIBS='-lm $(CRYPT_LIB) $(SSL_LIBS) -fsanitize=address,undefined'
```

This preserves platform-required libs (for example Linux `-lcrypt`) while still enforcing sanitizer runtime linkage.

If you override `LIBS` manually for sanitizer builds, include any required platform libs as needed.

---

## 7) Relationship to Other Docs

- Build/deploy overview: [README.md](README.md)
- Driver internals and architecture: [QUICKSTART.md](QUICKSTART.md)
- TLS build and operations: [SSL.md](SSL.md)
- Test runner behavior and script relationships: [tests/README.md](tests/README.md)
