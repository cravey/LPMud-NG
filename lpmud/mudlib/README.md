# Mudlib Overview

This directory contains the gameplay/content side of LPmud: rooms, objects, player areas, and runtime data files used by the `parse` driver.

---

## Key Directories

- `doc/`: builder and wizard documentation.
- `obj/`: reusable LPC object definitions (`player`, `monster`, `weapon`, etc.).
- `room/`: world rooms and core environment objects.
- `players/`: wizard/player content and persistent save-state files.
- `log/`: runtime log outputs.

---

## Source-Controlled vs Runtime-Mutable Data

| Category | Typical paths | Guidance |
|---|---|---|
| Source-controlled LPC/docs | `room/*.c`, `obj/*.c`, `doc/**`, wizard content sources | normal code review and commit flow |
| Runtime mutable save-state | `players/*.o`, `room/post_dir/*.o`, `banish/*.o`, `bulletin.o`, `wiz_bull*.o`, `WIZLIST` | treat as operational state; back up before risky operations |
| Runtime logs | `log/**` | do not rely on git for retention; rotate/archive externally |

Before large mudlib edits or migrations, snapshot mutable state separately from source control.

---

## Start Here

- For wizard/admin workflows: [QUICKSTART.md](QUICKSTART.md)
- For legacy brief inventory: [`README`](README)

---

## Relationship to Driver and Tests

- Driver runtime root is this mudlib when `MUD_LIB=./mudlib`.
- Driver/build/deploy reference: [../README.md](../README.md)
- Test tooling reference: [../tests/README.md](../tests/README.md)
- Dedicated driver regression mudlib (separate from gameplay mudlib): [../test-mudlib/README.md](../test-mudlib/README.md)
