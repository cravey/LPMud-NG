# Mudlib Quickstart (Wizards + Mudlib Admin)

This guide is for new wizards building content and for admins maintaining mudlib operations.

## 1) What mudlib Contains

Directory overview:

- `doc/` documentation
- `obj/` reusable LPC objects (player, monster, weapon, armour, boards, mail, etc.)
- `room/` world rooms and core locations
- `players/` wizard areas and player save data files
- `log/` game logs

Reference:

- [`README`](README)
- [`doc/build/README`](doc/build/README)

## 2) Wizard Workflow Basics

Common wizard commands are documented in:

- [`doc/w/README`](doc/w/README)
- [`doc/w/ed`](doc/w/ed)
- [`doc/w/load`](doc/w/load)
- [`doc/w/update`](doc/w/update)
- [`doc/w/clone`](doc/w/clone)
- [`doc/w/mkdir`](doc/w/mkdir)

Typical loop:

1. Edit file (`ed`).
2. Reload definition (`update <file>`).
3. Load/test room or object (`load <file>`, `clone <file>` where appropriate).
4. Iterate.

## 3) Adding and Modifying Castles (New Wizard)

### How a new wizard gets started

- The driver supports creating a wizard castle via `create_wizard` logic in `room/port_castle` flow; that creates `players/<wizard>/castle.c` from the default template.
- After that, the wizard expands from this facade into real rooms.

Reference:

- [`doc/build/castle`](doc/build/castle)
- [`room/def_castle.c`](room/def_castle.c)
- [`obj/player.c`](obj/player.c)

### Practical castle expansion

1. Create a hall room (copy/adapt a standard room pattern).
2. Change your `castle.c` enter behavior to move into your hall.
3. Add exits and additional rooms under `players/<yourname>/`.
4. Use `update` to reload changed rooms.

Helpful room docs:

- [`doc/build/general`](doc/build/general)
- [`doc/build/std`](doc/build/std)
- [`doc/build/std2`](doc/build/std2)
- [`doc/build/door`](doc/build/door)

## 4) Adding Monsters and Items

### Monsters

- Start from generic monster object by cloning/configuring `obj/monster`.
- Configure name, level, hp, wc/ac, aggression, spells/chats as needed.

Reference:

- [`doc/build/monster`](doc/build/monster)
- [`obj/monster.c`](obj/monster.c)

### Weapons / Armour / Other items

- Reuse templates and docs:
  - [`doc/build/weapon`](doc/build/weapon)
  - [`doc/build/armour`](doc/build/armour)
  - [`doc/build/container`](doc/build/container)
  - [`doc/build/food`](doc/build/food)
  - [`doc/build/drinks`](doc/build/drinks)

## 5) Persistent Data (Admin-Critical)

These are key persistent mudlib data files:

- Player save files: `players/*.o`
- Mail/post data: `room/post_dir/*.o`
- Banish data: `banish/*.o`
- Bulletin/wizard boards: `bulletin.o`, `wiz_bull*.o`
- Wizard ranking/state: `WIZLIST`

Notes:

- `.o` files in mudlib are LPC save-state files from `save_object`, not C object files.
- Keep backups for `players/`, `room/post_dir/`, `banish/`, `WIZLIST`, and board files.

## 6) Admin Operating Guidance

- Treat `log/` as operational data; rotate/archive externally if needed.
- Distinguish source content (`.c`, `.h`, docs) from runtime mutable state (`*.o`, logs, WIZLIST).
- Prefer staging and testing room/object changes with wizard commands before promoting broad changes.

## 7) Recommended Learning Path for New Wizards

1. Read [`doc/build/RULES`](doc/build/RULES).
2. Read [`doc/build/general`](doc/build/general).
3. Build one simple room using [`doc/build/std`](doc/build/std).
4. Expand castle using [`doc/build/castle`](doc/build/castle).
5. Add one monster using [`doc/build/monster`](doc/build/monster).
6. Practice safe update/load cycle via docs in `doc/w/`.
