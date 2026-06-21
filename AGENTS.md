# Legends of Azeroth -- Pandaria 5.4.8

WoW Mists of Pandaria 5.4.8 server emulator (TrinityCore/SkyFire fork). C++20, CMake, Ninja.

## Build & develop

```
./build.sh configure          # cmake - Release, Ninja, -DTOOLS=0, -DELUNA=0, ccache + gold linker
./build.sh configure --debug  # Debug build
./build.sh build              # cmake --build -> worldserver + authserver
./build.sh install            # versioned symlinks in $HOME/warcraft-server (prunes >7 days)
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

## Key CMake options (defined in `cmake/options.cmake` unless noted)

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
| `BUILD_DEPLOY` | ON (UNIX) | Deployment-optimized build (sets install prefix to `/server/wow/horizon`) |
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

Full documentation in `BATTLEPETS.md`. Key rule: `HandleRound()` should NEVER auto-swap pets. Pet death swaps belong in `TurnFinished()` or via client input.

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
- **NEVER** use sed, Python, or any other method to modify files during plan/read-only mode. Respect tool restrictions and use the designated edit tools only when explicitly in build/editing mode.

## Tillers Farm

Full implementation of the Tillers farm system for Valley of Four Winds (zone 1023).

### Architecture

- **Scene Builder pattern**: Farm reconstructed from state at login/relog/teleport — no world mutation per player
- **Two-phase isolation model**:
  - Phase 1 (PUBLIC_FARM_MASK = 128): Static farm objects visible to pre-quest players
  - Phase 2 (phaseMask = `(guid << 8) | 1`): Private dynamic farm, fully isolated per player
- **Isolation via `_privateObjectOwner`**: All spawned objects (Yoon, soil, companions) have owner set — prevents cross-player visibility
- **Content stored in `PlayerFarmState` struct**: `farmState` (obstacle bitmask), `plotsUnlocked`, `bestFriendUnlocks` (bitmask), `lastGrowthTick`

### Key Files

- `src/server/scripts/Pandaria/TillersFarmMgr.h` — Header with companion constants, creature entries, `PlayerFarmState` struct, class interface
- `src/server/scripts/Pandaria/TillersFarmMgr.cpp` — Scene builder implementation, `SpawnPlayerFarm()`, companion spawning logic
- `src/server/scripts/Pandaria/TillersZoneHooks.cpp` — Zone enter hook, Best Friend detection, Lost Dog quest hook
- `src/server/scripts/Pandaria/TillersSoilScript.cpp` — Soil GameObject interaction script
- `src/server/scripts/Pandaria/TillersWorkstation.cpp` — Workstation interaction handlers
- `src/server/scripts/Commands/cs_tillers.cpp` — GM commands (`.tillers grow`, `.tillers reset`)

### Companion System

- **10 companions total**: 9 Best Friends (Exalted reputation unlock) + 1 Lost Dog (quest 30526)
- **Bitmask storage**: `PlayerFarmState.bestFriendUnlocks` (uint16) — 16 bits available
- **Best Friend unlock constants**:
  - `BEST_FRIEND_SHAGGY` (1<<0) — Farmer Fung Best Friend — Yak (85814)
  - `BEST_FRIEND_FIFI` (1<<1) — Haohan Mudclaw Best Friend — Mushan (85791)
  - `BEST_FRIEND_CHICKENS` (1<<2) — Old Hillpaw Best Friend — Chickens (85820)
  - `BEST_FRIEND_SHEEP` (1<<3) — Chee Chee Best Friend — Sheep (85808)
  - `BEST_FRIEND_LUNA` (1<<4) — Ella Best Friend — Cat (85818)
  - `BEST_FRIEND_PIGGY` (1<<5) — Fish Fellreed Best Friend — Pigs (85802)
  - `BEST_FRIEND_ORANGE_TREE` (1<<6) — Sho Best Friend — Orange tree (237243)
  - `BEST_FRIEND_FURNITURE` (1<<7) — Tina Mudclaw Best Friend — Furniture (237244)
  - `BEST_FRIEND_MAILBOX` (1<<8) — Gina Mudclaw Best Friend — Mailbox (237242)
  - `BEST_FRIEND_LOST_DOG` (1<<9) — Lost Dog quest (30526) — Dog (85826)
- **Best Friend detection**: `IsBestFriend()` checks per-NPC friendship faction (1275–1283) reputation >= Exalted (42000). Each Tillers NPC has its own faction in Faction.dbc.
- **Companion metadata**: `BestFriendData` struct maps bitmask → creature/GO entry → faction ID. `BestFriendCompanions[10]` array provides indexed lookup for all 10 companions (9 reputation + 1 quest).
- **Lost Dog**: Quest 30526 completion via `IsQuestRewarded(30526)`
- **Auto-unlock on zone enter**: `UpdateBestFriendUnlockState()` called from `SpawnPlayerFarm()` when player enters VFW
- **Scene builder**: `SpawnPlayerFarmCompanions()` iterates `bestFriendUnlocks` bitmask, looks up positions from creature/GO table, and spawns creatures via `SpawnBestFriendUnlock()` or GOs via `SpawnBestFriendUnlockGO()`. GOs tracked in `_playerBestFriendUnlocks` alongside creatures for unified cleanup.

### Database Schema

- **`player_farm_state` table** (characters database):
  - `guid` — player GUID
  - `farm_phase` — FarmState bitmask (14=full, 12=weeds cleared, 8=wagon cleared, 0=all cleared)
  - `plots_unlocked` — number of unlocked plots (4, 8, 12, or 16)
  - `best_friend_unlocks` — SMALLINT UNSIGNED bitmask of unlocked best friends
  - `last_growth_tick` — timestamp for growth scheduling
- **`player_farm_plots` table** (characters database):
  - `guid`, `plot_id`, `state`, `seed_entry`, `needs_watering`, `has_pests`, `maturity_timestamp`
- **SQL migrations**: Located in `sql/updates/world/` and `sql/updates/characters/`
- **Latest migration**: `2026_06_03_08_tillers_best_friend_unlocks_rename.sql` — renames `companions` → `best_friend_unlocks`, ensures default rows
- **Companion migration**: `2026_06_20_01_tillers_companions.sql` — creature_template/GO entries + spawn positions for all 10 companions

### Farm State Progression

- **FARM_STATE_FULL (14/0b1110)**: All obstacles — 4 plots unlocked
- **FARM_STATE_WEEDS_CLEARED (12/0b1100)**: Wagon + boulder — 8 plots unlocked
- **FARM_STATE_WAGON_CLEARED (8/0b1000)**: Boulder only — 12 plots unlocked
- **FARM_STATE_ALL_CLEARED (0/0b0000)**: No obstacles — 16 plots unlocked

### Quest Gating

- **Quest 30252** (entry quest): Required to spawn personal farm
- **Quest 30256** (Learn and Grow IV): Unlocks soil GameObjects and crop planting
- **Quest 30257** (dark soil): Additional quest chain progression
- **Quest 30526** (Lost Dog): Unlocks companion

### Implementation Status

**Completed**:
- Phase mask formula: `(guid << 8) | 1` (bit 0 keeps normal world visible)
- `_privateObjectOwner` mechanism on all spawned objects
- `PlayerFarmState` struct with `bestFriendUnlocks` bitmask
- Best Friend unlock constants and creature/GO entries
- Best Friend unlock spawning/removal methods
- Best Friend reputation check logic
- Lost Dog quest hook
- `UpdateBestFriendUnlockState()` called in `SpawnPlayerFarm()` after state load
- SQL migration for `best_friend_unlocks` column rename
- Full persistence: `LoadPlayerState()` loads `best_friend_unlocks`, `SavePlayerFarm()` saves via REPLACE INTO
- New player state row creation via `REPLACE INTO player_farm_state`
- `SpawnPlayerFarmCompanions()` integrated into `SpawnPlayerFarm()` scene builder — iterates `bestFriendUnlocks` bits, spawns creatures/GOs
- Per-NPC friendship faction checks (1275–1283) in `IsBestFriend()` — replaces single-faction 1934 check
- GO companion spawning via `SpawnBestFriendUnlockGO()` with unified tracking in `_playerBestFriendUnlocks`
- `BestFriendData` struct + `BestFriendCompanions[10]` static array for indexed metadata lookup
- `IsGameObjectEntry()` helper for branching creature vs GO spawning
- `RemoveBestFriendUnlocks()` handles both creature and GO cleanup via `IsCreature()`/`IsGameObject()` type check
- SQL migration `2026_06_20_01_tillers_companions.sql` — creature_template/GO entries + spawn positions for all 10 companions
- Seed-to-vegetable item mapping (`GetVegetableForSeed()`) supporting 9 seed types with retail vegetable entries
- `HarvestCrop` reward fix — 5 vegetables base yield (retail), Tillers reputation scaling per harvest (125 at Neutral → 15 at Exalted)

**Remaining work**:
- Test best friend unlock flow on zone enter
