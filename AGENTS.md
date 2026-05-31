# Legends of Azeroth -- Pandaria 5.4.8

WoW Mists of Pandaria 5.4.8 server emulator (TrinityCore/SkyFire fork). C++20, CMake.

## Build & develop

```
./build.sh configure          # cmake - Release, -DTOOLS=0, uses ccache + gold linker
./build.sh configure --debug  # Debug build
./build.sh build              # cmake --build -> worldserver + authserver
./build.sh install            # installs to $HOME/warcraft-server (backups old as *_old)
./build.sh restore            # restore previous binaries
```

Manual CMake equivalent:
```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DTOOLS=0
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

- **`build.sh` uses `ccache` + `-fuse-ld=gold`** for faster local rebuilds (not used in CI).
- **CMake configure modifies source files.** `ModulesLoader.cpp` is auto-generated from `modules/ModulesLoader.cpp.in.cmake` and written back to `modules/ModulesLoader.cpp` in the source tree.
- **`revision.h` is auto-generated** at configure time by `cmake/genrev.cmake` (from `revision.h.in.cmake`). Uses `hg` (Mercurial) commands; falls back to defaults if `hg` unavailable.
- **Config files use `.dist` extension** at `src/server/worldserver/worldserver.conf.dist`, `src/server/authserver/authserver.conf.dist`, `modules/mod_playerbots/config/playerbots.conf.dist`, `src/updater/updater.conf.dist`. Copy to same name without `.dist` to customize.
- **CI workflows ignore `sql/**` path changes** — SQL-only PRs/commits skip all build checks.
- **Playerbots** is early-stage, may crash. Requires `enUS` DBC, a `playerbots.conf`, and specific `worldserver.conf` entries (see README).
- **Install prefix** defaults to `$HOME/warcraft-server` via `build.sh`, or `/server/wow/horizon` when `BUILD_DEPLOY=ON` and not `WITH_COREDEBUG`.

## Database

Three databases: `auth`, `characters`, `world` + optional `playerbots`. Base schemas in `sql/base/`, incremental updates in `sql/updates/`.

**⚠️ Always confirm things in the database with `mysql -uread -pread` before making changes or assumptions.**

## Blizzlike Design Philosophy

The goal is to be as close to retail WoW 5.4.8 (Blizzlike) as possible. When implementing features, fixing mechanics, or making design decisions:

- **Match retail behavior** — client version, spell data, NPC behavior, loot tables, and combat formulas should align with Pandaria 5.4.8 retail as the source of truth.
- **Prioritize DBC/DB data over hardcoded logic** — prefer correcting database entries (DBC, creature_template, spell_dbc, etc.) rather than patching around bad data in C++ code.
- **Reference public sources** — use tools like WoWWiki, Wowpedia, and retail packet/spell dumps to verify mechanics when documentation is ambiguous.
- **Avoid custom content or balance changes** — do not introduce homebrew mechanics, adjusted DPS numbers, or altered difficulty unless explicitly requested. This is a replication project, not a private server with custom features.
- **When in doubt, match the client** — if behavior differs from retail but matches what the 5.4.8 client expects (including intentional bugs or quirks), preserve it and document the deviation.

## Pet Battle System

- Full documentation in `PETBATTLES.md` — battle flow, key design principles, fixes applied, current state
- Core files: `src/server/game/BattlePet/PetBattle.cpp`, `PetBattle.h`, `BattlePetSpawnMgr.cpp`, `BattlePetTrainerMgr.cpp`, `src/server/game/Handlers/BattlePetHandler.cpp`
- Key rule: `HandleRound()` should NEVER auto-swap pets. Pet death swaps belong in `TurnFinished()` or via client input.

## ⚠️ Build Safety Rules (CRITICAL - AI Agents)

- **NEVER** reconfigure (cmake), rebuild the build directory, or clean the build folder without explicit user approval. The server is slow and reconfiguration wastes ~30 minutes.
- **NEVER** start a build unless the user explicitly asks for it.
- **ALWAYS** use `./build.sh` in the local project directory for any build operations (it handles ccache + gold linker + correct flags).
- **ALWAYS** save database writes as SQL migration files in `sql/updates/world/` or `sql/updates/characters/` — NEVER run UPDATE/INSERT/DELETE directly against the database without having a migration file first.
