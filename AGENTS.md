# Legends of Azeroth -- Pandaria 5.4.8

WoW Mists of Pandaria 5.4.8 server emulator (TrinityCore/SkyFire fork). C++20, CMake, Ninja.

## Build & develop

```
./build.sh configure          # cmake - Release, Ninja, -DTOOLS=0, -DELUNA=0, ccache + gold linker
./build.sh configure --debug  # Debug build
./build.sh build              # cmake --build -> worldserver + authserver
./build.sh install            # versioned symlinks in $HOME/warcraft-server (prunes >7 days)
./build.sh restore            # restore previous binaries
```

Manual CMake equivalent:
```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DTOOLS=0 -G Ninja
cmake --build build --target worldserver authserver -j$(nproc)
```

CI reference builds:
- **Linux GCC** `-DTOOLS=1 -DELUNA=0`, `make -j4 -k`
- **Linux Clang** `-DTOOLS=1 -DELUNA=0 -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++`, `make -j4 -k`
- **macOS arm64** `cmake -GNinja -B build -DWITH_WARNINGS=1 -DELUNA=0`, `ninja`
- **Windows** `cmake .. -DTOOLS=ON -DELUNA=1 -A x64`, `cmake --build . --config RelWithDebInfo`

## Key CMake options (all in `cmake/options.cmake`)

| Option | Default | What |
|---|---|---|
| `TOOLS` | OFF | map/vmap/mmap extraction tools |
| `ELUNA` | ON | Lua scripting engine |
| `PLAYERBOTS` | ON | AI player bots module |
| `SCRIPTS` | ON | Scripted content |
| `USE_COREPCH` | ON | Precompiled headers for servers |
| `USE_MODULES` | ON | Module system (auto-discovers `modules/` subdirectories) |
| `WITH_SANITIZER` | OFF | AddressSanitizer (GCC only) |
| `WITH_COREDEBUG` | OFF | Additional debug assertions/logging |
| `UPDATER` | OFF | Build updater tool |
| `NOJEM` | (implicit OFF) | Disable jemalloc (use for valgrind) |

**Disable ELUNA when PLAYERBOTS is enabled** (they conflict at runtime). README warns this explicitly.

**Hidden: set `NOPCH=1`** (any value) at configure time to disable all precompiled headers globally — useful for build troubleshooting.

## No tests, no lint, no typecheck

Compilation passing CI is the only quality gate. No unit test framework is configured (CTest exists but unused). No `.clang-format` or `.clang-tidy`.

Code standards (`doc/code_standards.md`): Oracle/Sun style, 4-space indent, no tabs, no trailing whitespace, LF line endings, squash PR commits.

## Quirks

- **`build.sh` hardcodes `-DELUNA=0` and `-DTOOLS=0`** — overrides CMake defaults; use manual CMake to change.
- **`build.sh` uses Ninja generator** — `cmake -G Ninja` is implicit.
- **`build.sh` installs to `$HOME/warcraft-server`** — creates versioned symlinks (`worldservers/worldserver-YYYYMMDD-HHMM`), auto-prunes >7 days.
- **CMake configure modifies source files.** `ModulesLoader.cpp` is auto-generated from `modules/ModulesLoader.cpp.in.cmake` and written back to `modules/ModulesLoader.cpp` in the source tree.
- **`revision.h` is auto-generated** at configure time by `cmake/genrev.cmake` (from `revision.h.in.cmake`). Uses `hg` (Mercurial) commands; falls back to defaults if `hg` unavailable.
- **Config files use `.dist` extension** — copy to same name without `.dist` for worldserver to read them: `worldserver.conf.dist` → `worldserver.conf`, `authserver.conf.dist` → `authserver.conf`, `modules/mod_playerbots/config/playerbots.conf.dist` → `playerbots.conf`, `src/updater/updater.conf.dist` → `updater.conf`.
- **CI workflows ignore `sql/**` path changes** — SQL-only PRs/commits skip all build checks.
- **Playerbots** is early-stage, may crash. Requires `enUS` DBC, a `playerbots.conf`, and specific `worldserver.conf` entries (see README).
- **Install prefix** defaults to `$HOME/warcraft-server` via `build.sh`, or `/server/wow/horizon` when `BUILD_DEPLOY=ON` and not `WITH_COREDEBUG`.

## Database

Three databases: `auth`, `characters`, `world` + optional `playerbots`. Base schemas in `sql/base/`, incremental updates in `sql/updates/`.

**SQL migration naming**: `YYYY_MM_DD_XX_description.sql` — `XX` is a numeric suffix for same-day files. Placed in `sql/updates/world/` or `sql/updates/characters/`.

**Runtime reload**: `reload smart_scripts` hot-reloads SAI without restart.

**DBC data path**: `/home/andrew/warcraft-server/bin/Data/dbc/` — Spell.dbc, AreaTable.dbc, etc. for investigation.

**⚠️ Always confirm things in the database with `mysql -uread -pread` before making changes or assumptions.**

**👤 The user may run commands in-game (e.g., `.lookup item`, `.go`, `.quest`) to inspect live behavior and provide information that may not match the database. Treat user-supplied in-game observations as ground truth.**

## Blizzlike Design Philosophy

The goal is to be as close to retail WoW 5.4.8 (Blizzlike) as possible. When implementing features, fixing mechanics, or making design decisions:

- **Match retail behavior** — client version, spell data, NPC behavior, loot tables, and combat formulas should align with Pandaria 5.4.8 retail as the source of truth.
- **Prioritize DBC/DB data over hardcoded logic** — prefer correcting database entries (DBC, creature_template, spell_dbc, etc.) rather than patching around bad data in C++ code.
- **Reference public sources** — use tools like WoWWiki, Wowpedia, and retail packet/spell dumps to verify mechanics when documentation is ambiguous.
- **Avoid custom content or balance changes** — do not introduce homebrew mechanics, adjusted DPS numbers, or altered difficulty unless explicitly requested. This is a replication project, not a private server with custom features.
- **When in doubt, match the client** — if behavior differs from retail but matches what the 5.4.8 client expects (including intentional bugs or quirks), preserve it and document the deviation.

## Pet Battle System

Full documentation in `PETBATTLES.md`. Key rule: `HandleRound()` should NEVER auto-swap pets. Pet death swaps belong in `TurnFinished()` or via client input.

## SmartAI (SAI)

Full reference in `SAI.md`. Three patterns that come up most often:

- **Spellclick chain**: `npc_spellclick_spells` (spell→NPC) + `conditions` (SourceTypeOrReferenceId=18) + `smart_scripts` (event_type=73) + `creature_template.npcflag=16777216`
- **CONDITION_QUESTTAKEN** (type 9) gates spellclicks to active quests only
- **spell_scripts** only fires for SPELL_EFFECT_DUMMY (3) or SPELL_EFFECT_SCRIPT_EFFECT (77)

## ⚠️ Build Safety Rules (CRITICAL - AI Agents)

- **NEVER** reconfigure (cmake), rebuild the build directory, or clean the build folder without explicit user approval. The server is slow and reconfiguration wastes ~30 minutes.
- **NEVER** start a build unless the user explicitly asks for it.
- **ALWAYS** use `./build.sh` in the local project directory for any build operations (it handles ccache + gold linker + correct flags).
- **ALWAYS** save database writes as SQL migration files in `sql/updates/world/` or `sql/updates/characters/` — NEVER run UPDATE/INSERT/DELETE directly against the database without having a migration file first.
