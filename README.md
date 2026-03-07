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
