# LPmud-NG

Modernized LPmud 2.4.5 playground and maintenance codebase focused on the `parse` driver, mudlib behavior, and reproducible validation tooling.

For driver build, deployment, and environment variables, start with [lpmud/README.md](lpmud/README.md).

---

## Start Here (By Role)

- Driver maintainer: [lpmud/README.md](lpmud/README.md) -> [lpmud/DEPENDENCIES.md](lpmud/DEPENDENCIES.md) -> [lpmud/QUICKSTART.md](lpmud/QUICKSTART.md)
- Mudlib wizard/admin: [lpmud/mudlib/README.md](lpmud/mudlib/README.md) -> [lpmud/mudlib/QUICKSTART.md](lpmud/mudlib/QUICKSTART.md)
- Test engineer/QA: [lpmud/tests/README.md](lpmud/tests/README.md) -> [lpmud/test-mudlib/README.md](lpmud/test-mudlib/README.md)
- TLS/operations: [lpmud/SSL.md](lpmud/SSL.md) -> [lpmud/README.md](lpmud/README.md)

Quick sanity run from `lpmud/`:

```bash
make parse && make mudlibtest
```

---

## Documentation Guide

### Repository-Level Docs

- [README.md](README.md): high-level repository overview and modernization status.
- [LICENSE.md](LICENSE.md): licensing terms.

### Driver and Build Docs

- [lpmud/README.md](lpmud/README.md): primary build, validation, and deployment reference.
- [lpmud/DEPENDENCIES.md](lpmud/DEPENDENCIES.md): external toolchain and runtime dependency matrix.
- [lpmud/QUICKSTART.md](lpmud/QUICKSTART.md): driver architecture and maintainer workflow.
- [lpmud/SSL.md](lpmud/SSL.md): canonical SSL/TLS build and runtime guide.

### Mudlib Docs

- [lpmud/mudlib/README.md](lpmud/mudlib/README.md): mudlib directory orientation and key paths.
- [lpmud/mudlib/QUICKSTART.md](lpmud/mudlib/QUICKSTART.md): wizard/admin gameplay-content workflow.

### Testing Docs

- [lpmud/tests/README.md](lpmud/tests/README.md): test runners, harness relationships, and Makefile integration.
- [lpmud/test-mudlib/README.md](lpmud/test-mudlib/README.md): dedicated deterministic driver-focused test mudlib.

---

## Project Modernization Summary

1. **Toolchain modernization (C23)**: migrated the driver build to C23 and aligned workflows with modern clang toolchains.
2. **Compiler warning and pedantic cleanup**: reduced `-Wall`/`-Wextra`/`-Wpedantic` findings through low-risk fixes.
3. **Static-analysis remediation**: added analyzer checks and fixed high-severity findings, including null-deref and UB paths.
4. **Runtime/parser hygiene**: tightened interpreter/runtime control flow and removed parser-conflict behavior.
5. **64-bit portability hardening**: corrected pointer/cast and memory-safety edge cases for modern systems.
6. **Precompiler/platform compatibility**: improved portability assumptions across macOS/Linux/FreeBSD.
7. **Security-focused pass**: reviewed and patched high-confidence issues; tracking remains in `lpmud/Security.txt` and `lpmud/ISSUES`.
8. **CI safety gates**: established repeatable `ci-warnings`, `ci-sanitize`, `ci-analyze`, and aggregate `ci`.
9. **Mudlib topology tooling**: added room/castle graph generation under `lpmud/tools/`.
10. **Current status**: latest verification showed `make ci` succeeding with tracked hard analyzer issues resolved.

---

## Driver-Focused Test Suites

The repository includes three complementary test paths focused on code running on top of `parse`:

### 1) Dedicated LPC Engine Test Mudlib (`lpmud/test-mudlib`)

- Purpose: deterministic, low-noise driver/efun validation via a single `runtests` command.
- Entry point: `cd lpmud && ./tests/run_test_mudlib_suite.sh`
- Docs: [lpmud/test-mudlib/README.md](lpmud/test-mudlib/README.md)

### 2) Classic Mudlib Functional Network Suite

- Purpose: end-to-end gameplay/session checks against `mudlib` using `nc` + `expect`.
- Entry point: `cd lpmud && ./tests/run_engine_functional_suite.sh`
- Harness: `lpmud/tests/nc_engine_suite.exp`

### 3) Static Mudlib Analysis Tooling

- Purpose: heuristic reports for monster difficulty/weapon drops and LPC feature usage.
- Entry point: `cd lpmud && python3 tests/analyze_mudlib.py --mudlib-root mudlib --output-dir tests/reports/static_only`
- Docs: [lpmud/tests/README.md](lpmud/tests/README.md)

All generated test artifacts are written under `lpmud/tests/reports/`.

---

## Optional TLS (SSL) Runtime

TLS support is optional:

- build with `USE_SSL=1`
- enable at runtime with `MUD_SSL=1`
- provide cert/key env vars (`MUD_SSL_CERT_FILE`, `MUD_SSL_KEY_FILE`)

For complete SSL build, certificate, client compatibility, and operational guidance, use the canonical guide:

- [lpmud/SSL.md](lpmud/SSL.md)
