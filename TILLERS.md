# Tillers Farm System — Documentation

## File Locations

| File | Purpose |
|---|---|
| `src/server/scripts/Pandaria/TillersFarm/tillers_farm.h` | Types, enums, constants, helper declarations |
| `src/server/scripts/Pandaria/TillersFarm/tillers_farm.cpp` | Full implementation: player script, creature script, spell scripts |
| `sql/updates/characters/2026_05_24_00_characters_tillers_farm.sql` | Character DB schema: plot state + farm data tables |
| `src/server/scripts/ScriptLoader/ScriptLoader.cpp:1024,2087` | Script registration |
| `src/server/scripts/Pandaria/CMakeLists.txt:38-39` | Build configuration |

## System Overview

Sunsong Ranch is a personal farming system in Valley of the Four Winds. Each player gets their own phased farm at Sunsong Ranch (coordinates ~52, 48.5 in Halfhill) where they can plant seeds, tend crops, and harvest produce. The farm starts with 4 plots and expands to 16 through reputation-gated upgrades.

### Key Design Principles

1. **Phased per-player.** Each player enters their own version of Sunsong Ranch. Farm plots are summoned as `TEMPSUMMON_MANUAL_DESPAWN` creatures owned by the player.
2. **24-hour growth cycle.** Crops planted at time T become ripe at T + 86400s. Growth is checked on spawn and on gossip interaction.
3. **Bonus crop rotation.** A 10-day cycle determines which crop gives +2 bonus yield on harvest day.
4. **Reputation-gated progression.** Plots unlock at 4/8/12/16 based on Tillers faction reputation (Neutral→Honored→Revered→Exalted).
5. **Tool upgrades.** Three farm-wide tools (irrigation, pest control, plow) become available after clearing obstacles.

## Current State

### Implemented
- [x] 16 plot positions at Sunsong Ranch
- [x] 12 crop types with seed/harvest item mappings
- [x] 6 plot states: UNTILLED → TILLED → GROWING → RIPE, STUBBORN, OCCUPIED
- [x] 10 crop conditions: HEALTHY, PARCHED, PESTS, WEEDS, ALLURING, WIGGLING, SMOTHERED, WILD, RUNTY, TANGLED
- [x] 2 special crop types: PLUMP (+3 yield), BURSTING (instant harvest)
- [x] Gossip-based plot interaction (till, plant, harvest, water, spray, pull weeds, shoo birds, dig virmen, clear smothered, wrestle wild, pull runty, untangle vines, pull stubborn soil, expose occupied virmen)
- [x] Post-harvest soil states: STUBBORN (15% chance), OCCUPIED (10% chance)
- [x] Tiller gift rare drop from harvest (5% chance, item 79246)
- [x] Tillers faction reputation gain on harvest (50 rep per crop)
- [x] Seed drop chance on harvest (50%, 1-3 seeds)
- [x] Character DB persistence for plot state and farm upgrades
- [x] In-memory cache with thread-safe `FarmDataManager` (`std::map<ObjectGuid, ...>`)
- [x] Player login/logout load/save lifecycle
- [x] Zone-based spawn/despawn (Valley of the Four Winds zone 5805)
- [x] Bonus crop 10-day rotation schedule
- [x] Farm tool upgrade flags (irrigation, antipest, plow)
- [x] Votes mask for Tillers union progression
- [x] Water spell script (removes parched aura 115824)

### Code Quality
- [x] Deadlock fix: `SavePlot_NoLock` (internal, no lock) / `SavePlot` (public, acquires lock)
- [x] `unordered_map<ObjectGuid>` → `std::map<ObjectGrid>` (no hash available)
- [x] `snprintf` → `std::string` concatenation (no buffer overflow risk)
- [x] `CAST_AI` → `static_cast<npc_farm_plotAI*>(creature->AI())`
- [x] All enums → `enum class` (type-safe: `FarmPlotState`, `CropType`, `FarmCondition`, `SpecialCrop`)
- [x] `#define` → `constexpr` constants
- [x] `[[nodiscard]]` on helper functions
- [x] `UpdateGrowthTimers()` centralized (eliminates duplicate growth logic)
- [x] `FACTION_TILLERS` (1074), `HARVEST_REP_GAIN` (50) constants
- [x] SCALLION seed ID fix (79104 → 79103; 79104 is Rusty Watering Can)

### Missing (vs Retail)

**Crop Interactions (Mini-Games):**
- [x] Alluring crops — gossip option to shoo birds
- [x] Wiggling crops — gossip option to dig out virmen
- [x] Smothered crops — gossip option to clear weeds
- [x] Wild crops — gossip option to wrestle
- [x] Runty crops — gossip option to pull up
- [x] Tangled crops — gossip option to untangle vines
- [x] Stubborn soil post-harvest — gossip option to pull loose
- [x] Occupied soil post-harvest — gossip option to expose virmen
- [ ] Alluring crops — actual bird summon + combat mini-game
- [ ] Wiggling crops — actual virmen summon + combat mini-game
- [ ] Wild crops — actual wrestle mini-game (animation/ability)
- [ ] Runty crops — actual jump mini-game
- [ ] Tangled crops — actual run mini-game

**Reputation & Progression:**
- [x] Tillers faction (ID 1074) reputation gain on harvest (50 rep at level 90)
- [x] Tiller gift rare drops from harvest
- [ ] Friendship system with 10 individual Tiller NPCs
- [ ] Gift delivery via Andi daily quests

**Farm Upgrades:**
- [ ] 15-minute in-game timer for each farm expansion (Gai Lan, Fish Fellreed, Haohan/Thunder)
- [ ] Farm expansion phases: 4→8 plots (Honored), 8→12 (Revered), 12→16 (Exalted + all 5 votes)
- [ ] Jinyu Princess Sprinkler — waters all parched crops in a row of 8
- [ ] Thunder King Pest Repellers — auto-clears pest conditions
- [ ] Earth-Slasher Plow — tills rows of 4, stuns virmen at 30% HP

**Quests & Dailies:**
- [ ] Farmer Yoon daily crop planting quests (10-quest pool, +350 rep)
- [ ] Farmer Yoon daily kill quests (5-quest pool, +275 rep)
- [ ] Andi gift delivery daily quests (+50 rep + friendship)
- [ ] Two visiting farmer daily quests (+300 rep + friendship)
- [ ] Vote questlines: Gina, Mung-Mung, Farmer Fung, Nana, Haohan
- [ ] Inherit the Earth questline (Exalted + all plots)
- [ ] Work orders bulletin board (post-Inherit the Earth)

**Content:**
- [ ] Profession seeds (Revered+): Enigma, Magebulb, Raptorleaf, Snakeroot, Windshear, Songbell
- [ ] Decorative seeds: Autumn Blossom, Winter Blossom saplings
- [ ] Tillers Shrine gossip for reputation tracking
- [ ] Dark Soil interaction for friendship gifts
- [ ] Sunsong Ranch ownership changes (post-Inherit the Earth)

## Planned State

### Phase 1 — Critical Bugs & Code Quality (COMPLETE)
- [x] Fix deadlock: `SavePlot_NoLock` split
- [x] Fix hash: `unordered_map` → `std::map`
- [x] Replace `snprintf` with `std::string`
- [x] Replace `CAST_AI` with direct cast
- [x] Centralize growth timer: `UpdateGrowthTimers()`
- [x] Convert enums to `enum class`
- [x] Add `[[nodiscard]]` to helpers
- [x] Use `constexpr` for static data
- [x] Reputation on harvest
- [x] SCALLION seed ID fix

### Phase 2 — Crop Interactions & Reputation (IN PROGRESS)
- [x] Add 6 new crop conditions to enum and roll table
- [x] Add STUBBORN, OCCUPIED post-harvest soil states
- [x] Add gossip options for all new conditions and soil states
- [x] Add gossip select handlers for all new interactions
- [x] Add post-harvest soil state chance logic
- [x] Add Tiller gift rare drop
- [x] Fix seed drop `CropHarvestMap.end()` → `CropSeedMap.end()` bug
- [ ] Replace gossip interactions with actual mini-game mechanics (combat, animations, abilities)

### Phase 3 — Farm Upgrades & Tools
- [ ] Implement 15-minute in-game timer system for farm expansions
- [ ] Add Gai Lan (weeds→8 plots), Fish Fellreed (wagon→12), Haohan/Thunder (boulder→16)
- [ ] Implement tool upgrades: sprinkler, pest repellers, earth-slasher plow

### Phase 4 — Quests, Dailies, Friendship
- [ ] Daily quest system with Yoon, Andi, visiting farmers
- [ ] Friendship reputation system with 10 Tiller NPCs
- [ ] Vote questlines tracking
- [ ] Profession seeds and decorative seeds

### Phase 5 — Documentation & Verification
- [ ] Final TILLERS.md update reflecting completed state
- [ ] Build verification

## Progress Log

| Date | Phase | Changes |
|---|---|---|
| 2026-05-24 | Initial | Base farm system created: plot states, gossip, DB persistence |
| 2026-05-25 | Planning | Analysis complete, TILLERS.md created with full roadmap |
| 2026-05-25 | Phase 1 | Fixed deadlock (SavePlot_NoLock split), unordered_map→std::map, snprintf→std::string, CAST_AI→direct cast, enum class conversion, constexpr constants, [[nodiscard]] helpers, UpdateGrowthTimers centralization, reputation on harvest, SCALLION seed ID fix |
| 2026-05-25 | Phase 2 | Added 6 new crop conditions (ALLURING, WIGGLING, SMOTHERED, WILD, RUNTY, TANGLED), 2 post-harvest soil states (STUBBORN, OCCUPIED), gossip options + handlers for all, post-harvest soil state chance logic, Tiller gift drop, fixed seed drop bug |
