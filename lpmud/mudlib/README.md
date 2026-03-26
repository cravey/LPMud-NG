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

## Start Here

- For wizard/admin workflows: [Quickstart.md](Quickstart.md)
- For legacy brief inventory: [`README`](README)

---

## Relationship to Driver and Tests

- Driver runtime root is this mudlib when `MUD_LIB=./mudlib`.
- Driver/build/deploy reference: [../README.md](../README.md)
- Test tooling reference: [../tests/README.md](../tests/README.md)
- Dedicated driver regression mudlib (separate from gameplay mudlib): [../test-mudlib/README.md](../test-mudlib/README.md)
