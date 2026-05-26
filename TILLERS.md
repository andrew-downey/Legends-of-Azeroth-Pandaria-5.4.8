# Tillers Farm System — Documentation

## File Locations

| File | Purpose |
|---|---|
| `src/server/scripts/Pandaria/TillersFarm/tillers_farm.h` | Types, enums, constants, helper declarations |
| `src/server/scripts/Pandaria/TillersFarm/tillers_farm.cpp` | Full implementation: player script, creature script, spell scripts |
| `sql/updates/characters/2026_05_24_00_characters_tillers_farm.sql` | Character DB schema: plot state + farm data tables |
| `src/server/scripts/ScriptLoader/ScriptLoader.cpp:1024,2087` | Script registration |
| `src/server/scripts/Pandaria/CMakeLists.txt:38-43` | Build configuration |
| `src/server/scripts/Pandaria/TillersFarm/npc_tillers_friendship.cpp` | Friendship gossip for all 10 Tiller NPCs |
| `src/server/scripts/Pandaria/TillersFarm/go_tillers_shrine.cpp` | Shrine GO showing all friendship standings |
| `src/server/scripts/Pandaria/TillersFarm/go_dark_soil.cpp` | Dark Soil treasure drops |
| `src/server/scripts/Pandaria/TillersFarm/npc_farmer_yoon.cpp` | Farmer Yoon — expansion gossip + daily quest assignment |
| `src/server/scripts/Pandaria/TillersFarm/npc_tillers_vote.cpp` | Vote questlines for Mung-Mung + Nana Mudclaw |
| `sql/updates/characters/2026_05_26_02_characters_tillers_refactor.sql` | Character DB: npc_food_timers table |
| `sql/updates/world/2026_05_26_04_tillers_refactor.sql` | World DB: quest starter/ender bindings for dailies |

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
- [x] AI interaction state machine in npc_farm_plotAI (UpdateAI, 4 interaction types)
- [x] All 8 crop condition mini-games (channel, timer, distance, summon NPCs)
- [x] Dented Shovel to remove growing crops (item 89880)
- [x] Active interaction guard (prevents gossip during active plot interaction)
- [x] Correct tool item IDs: Dented Shovel (89880), Bug Sprayer (80513), Earth-Slasher (89814), Jinyu Princess (89812), Thunder King Pest Repellers (89813), Master Plow (89815)
- [x] Summoned NPC constants: Plainshawk (58910), Virmen (55483)
- [x] Profession seeds (Revered+): Snakeroot (85215), Enigma (85216), Magebulb (85217), Windshear Cactus (89197), Raptorleaf (89202), Songbell (89233)
- [x] Blossom tree saplings: Autumn (85267→85264), Spring (85268→85265), Winter (85269→85266)
- [x] Enigma seed harvest → random Pandaren herb (Green Tea Leaf, Silkweed, Rain Poppy, Golden Lotus, Snow Lily)
- [x] Profession seed item constants (HERB_*)
- [x] Farm expansion timer system: 15-min real-time timer via `expansion_timer_end` in DB
- [x] CheckFarmExpiration() on login, zone enter, and Yoon gossip
- [x] Farmer Yoon NPC script (`npc_farmer_yoon.cpp`) with reputation-gated expansion gossip
- [x] Three expansion tiers: Honored→8 plots, Revered→12 plots, Exalted+votes→16 plots
- [x] Tool installation via Yoon: consumes item, sets upgrade flag, gives Master Plow tool
- [x] Auto-clear PARCHED (hasIrrigation) and PESTS (hasAntipest) on zone enter
- [x] Reputation-gated profession seeds (Revered+ required to see in plant menu)
- [x] Deadlock fix: `SavePlot_NoLock` (internal, no lock) / `SavePlot` (public, acquires lock)
- [x] `unordered_map<ObjectGuid>` → `std::map<ObjectGuid, FarmData>` (no hash available)
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
- [x] Alluring crops — summon Plainshawk (58910) + combat mini-game
- [x] Wiggling crops — summon Virmen (55483) + combat mini-game
- [x] Smothered crops — channel 3s interaction
- [x] Wild crops — channel 4s interaction
- [x] Runty crops — 1s timer interaction
- [x] Tangled crops — move 15y away from plot interaction
- [x] Stubborn soil post-harvest — channel 3s interaction
- [x] Occupied soil post-harvest — summon Virmen (55483) + combat mini-game
- [x] Dented Shovel — remove growing crop (reverts plot to UNTILLED)
- [x] Active interaction guard — prevents starting new interaction while one is in progress

**Reputation & Progression:**
- [x] Tillers faction (ID 1272) reputation gain on harvest (50 rep at level 90)
- [x] 10 individual Tiller NPC factions (IDs 1273-1284, per DBC, `CanHaveReputation()=true`)
- [x] Standard `ReputationMgr` API for all NPC standing — no custom friendship manager
- [x] Friendly rank system: Stranger → Acquaintance → Buddy → Friend → Good Friend → Best Friend (same thresholds as retail, backed by actual faction rep values)
- [x] Daily food turn-in: 1800 standing per NPC per day (tracked via `character_tillers_npc_timers`)
- [x] Gift turn-in: unlimited per day, 540 standing per gift, 900 for preferred item
- [x] Gift delivery via Andi standard daily quest (entry 58708)
- [x] Visiting farmer daily quests with 8 NPC pool (2 per day via standard creature_queststarter/ender)

**Farm Upgrades:**
- [x] 15-minute real-time timer for each farm expansion (Yoon gossip-based)
- [x] Farm expansion phases: 4→8 plots (Honored), 8→12 (Revered), 12→16 (Exalted + all 5 votes)
- [x] Jinyu Princess Sprinkler item consumed → auto-clears parched condition on zone entry
- [x] Thunder King Pest Repellers item consumed → auto-clears pest condition on zone entry
- [x] Earth-Slasher Plow assembly kit consumed → gives Master Plow tool to player

**Quests & Dailies:**
- [x] Farmer Yoon daily crop planting quests (10-quest pool, +350 rep via standard `creature_queststarter/ender`, daily `QuestInfo` ID 1)
- [x] Farmer Yoon daily kill quests (5-quest pool, +275 rep)
- [x] Andi gift delivery daily quest (+150 rep + 1400 standing via standard quest)
- [x] Two visiting farmer daily quests (+150 rep + 2000 standing per NPC faction)
- [x] Server-wide deterministic daily rotation: crop = seed%2 (5 of 10), kill = seed%5 (1 of 5), visiting = seed/7%4*2 (2 consecutive of 8)
- [x] Vote gossip questlines: Gina, Mung-Mung, Farmer Fung, Nana, Haohan (custom gossip-based, not quest-based)
- [x] Tutorial gate (quest 30257 "Learn and Grow V") — auto-creates farm on completion
- [x] Standard core `character_queststatus` daily tracking — no custom DailyQuestManager
- [ ] Inherit the Earth questline (Exalted + all plots)
- [ ] Work orders bulletin board (post-Inherit the Earth)

**Content:**
- [x] Profession seeds (Revered+): Snakeroot, Enigma, Magebulb, Windshear Cactus, Raptorleaf, Songbell
- [x] Blossom tree saplings: Autumn (Honored+), Spring (Revered+), Winter (Exalted+)
- [x] Tillers Shrine gossip for reputation tracking (entry 215705)
- [x] Dark Soil interaction for friendship gifts (entry 210565, 5 treasure types)
- [x] Tutorial gate: farm auto-created on quest 30257 completion via OnLogin + SpawnFarm
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

### Phase 2 — Crop Interactions & Mini-Games (COMPLETE)
- [x] Add 6 new crop conditions to enum and roll table
- [x] Add STUBBORN, OCCUPIED post-harvest soil states
- [x] Add gossip options for all new conditions and soil states
- [x] Add gossip select handlers for all new interactions
- [x] Add post-harvest soil state chance logic
- [x] Add Tiller gift rare drop
- [x] Fix seed drop `CropHarvestMap.end()` → `CropSeedMap.end()` bug
- [x] AI state machine in npc_farm_plotAI (UpdateAI, 4 interaction types, FinishInteraction)
- [x] Alluring → summon Plainshawk (58910) + combat
- [x] Wiggling → summon Virmen (55483) + combat
- [x] Smothered → 3s channel interaction
- [x] Wild → 4s channel interaction
- [x] Runty → 1s timer interaction
- [x] Tangled → move 15y distance check
- [x] Stubborn → 3s channel interaction
- [x] Occupied → summon Virmen (55483) + combat
- [x] Dented Shovel — remove growing crop (plot → UNTILLED)
- [x] Interaction guard — prevents gossip during active interaction
- [x] Fix tool item IDs in `tillers_farm.h` (4 incorrect constants)
- [x] Fix seed action offset bug (GREEN_CABBAGE/SCALLION could not be planted)
- [x] Added ITEM_MASTER_PLOW, ITEM_EARTH_SLASHER, NPC_PLAINSHAWK, NPC_VIRMEN constants

### Phase 3 — Farm Upgrades & Tools (COMPLETE)
- [x] Implement 15-minute real-time timer system for farm expansions (via DB + Yoon gossip)
- [x] Expansion tiers: Honored→8 plots, Revered→12 plots, Exalted+votes→16 plots
- [x] Tool installation: Jinyu Princess (auto-clear parched), Thunder King (auto-clear pests), Earth-Slasher (gives Master Plow)
- [x] Auto-clear upgrade flags on zone enter
- [x] Profession seeds (6 types + 3 blossom saplings) with rep gating
- [x] Enigma seed random herb harvest

### Phase 4A — Friendship System (REPLACED by Phase 4B refactor)
- [x] 10 Tiller friendship NPCs with gossip (food/gift turn-in) — **kept, refactored to use reputation API**
- [x] Tillers Shrine gossip for reputation tracking (entry 215705) — **kept, refactored**
- [x] Dark Soil interaction for friendship gifts (entry 210565, 5 treasure types) — **kept, unchanged**
- [ ] ~~FriendshipManager with DB persistence (character_tillers_friendship table)~~ — **removed, replaced with standard ReputationMgr**
- [ ] ~~Daily food timer (custom)~~ — **removed, replaced with character_tillers_npc_timers + reputation API**
- [ ] ~~Daily gift timer (custom)~~ — **removed, gifts now unlimited per day**
- [ ] ~~Npc factions based on guessed IDs~~ — **replaced with retail DBC IDs 1273-1284**

### Phase 4B — Quests, Dailies (COMPLETE)

**Design change from Phase 4A**: Replaced custom `FriendshipManager` (own DB table) with standard WoW `ReputationMgr` using individual NPC factions 1273-1284 from Faction.dbc (retail-accurate). Replaced custom `DailyQuestManager` (own DB table + bitmask rotation) with standard core `character_queststatus` + `creature_queststarter/ender` DB entries. This eliminates two custom DB tables and two singleton managers, relying entirely on built-in game systems.

- [x] Per-NPC factions (1273-1284) via `ReputationMgr` instead of custom `FriendshipManager`
- [x] Standard `creature_queststarter/ender` entries + `QuestInfo` ID 1 for dailies instead of custom `DailyQuestManager`
- [x] Server-wide deterministic daily rotation (`GetTodaySeed()` = `time(nullptr)/86400`)
- [x] 10 crop dailies from Farmer Yoon (+350 rep), 5 active per day
- [x] 5 kill dailies from Farmer Yoon (+275 rep), 1 active per day
- [x] Andi gift delivery daily (58708, quest 32010) via DB quest binding
- [x] 8 visiting farmer dailies from their NPCs (32011-32018), 2 active per day
- [x] Vote gossip scripts: Gina, Mung-Mung, Farmer Fung, Nana, Haohan (unchanged)
- [x] Tutorial gate on quest 30257 (Learn and Grow V)
- [x] `character_tillers_npc_timers` table: food-once-per-day tracking per NPC (only remaining custom table)
- [x] DROP old tables (`character_tillers_friendship`, `character_tillers_daily_quests`) — SQL files deleted, schema clean

### Phase 4C — Inherit the Earth & Work Orders
- [ ] Inherit the Earth questline (Exalted + all 5 votes + all 16 plots)
- [ ] Work orders bulletin board (repeatable turn-ins for rep/friendship)
- [ ] Sunsong Ranch ownership changes (post-Inherit the Earth)

### Phase 5 — Final Polish & Verification
- [ ] In-game testing of dailies + vote questlines
- [ ] Edge case handling (disconnect during daily, race conditions)
- [ ] Build verification with full CI config (-DTOOLS=1 -DELUNA=0)
- [ ] Final review pass of TILLERS.md before closing Phase 5

## Progress Log

| Date | Phase | Changes |
|---|---|---|
| 2026-05-24 | Initial | Base farm system created: plot states, gossip, DB persistence |
| 2026-05-25 | Planning | Analysis complete, TILLERS.md created with full roadmap |
| 2026-05-25 | Phase 1 | Fixed deadlock (SavePlot_NoLock split), unordered_map→std::map, snprintf→std::string, CAST_AI→direct cast, enum class conversion, constexpr constants, [[nodiscard]] helpers, UpdateGrowthTimers centralization, reputation on harvest, SCALLION seed ID fix |
| 2026-05-25 | Phase 2 | Added 6 new crop conditions (ALLURING, WIGGLING, SMOTHERED, WILD, RUNTY, TANGLED), 2 post-harvest soil states (STUBBORN, OCCUPIED), gossip options + handlers for all, post-harvest soil state chance logic, Tiller gift drop, fixed seed drop bug |
| 2026-05-25 | Phase 2 | AI state machine (UpdateAI, FinishInteraction, 4 interaction types: CHANNEL/SUMMON/TIMER/MOVE_AWAY), all 8 mini-games implemented (Plainshawk summon, Virmen summon, channels, timer, distance), Dented Shovel (89880), interaction guard, tool item ID fixes, seed action offset bugfix |
| 2026-05-25 | Phase 3 | Farm expansion timer system (15-min real-time via DB + Yoon gossip), tool installation gossip (3 upgrades), auto-clear upgrade flags, profession seeds (6 types + 3 blossoms), Enigma random herb harvest, rep-gated seed menu, Farmer Yoon NPC script (npc_farmer_yoon.cpp), CheckFarmExpiration on login/zone, expansion_timer_end column in character DB |
| 2026-05-25 | Phase 4A | FriendshipManager with DB persistence (character_tillers_friendship table), FriendshipRank enum class (Stranger→Best Friend, 6 tiers), 10 Tiller friendship NPCs (npc_tillers_friendship.cpp), daily food/gift timers (1800/540/900 standing), Tillers Shrine gossip (go_tillers_shrine.cpp, entry 215705), Dark Soil treasure drops (go_dark_soil in tillers_farm.cpp, entry 210565, 5 treasure types), ScriptName bindings for all friendship NPCs + shrine + dark soil |
| 2026-05-26 | Phase 4B | DailyQuestManager singleton, crop dailies (10 crop + 5 kill), Andi gift daily (32010), 8 visiting farmer dailies (32011-32018), vote questlines (32025-32029) for Gina/Mung-Mung/Fung/Nana/Haohan, CropDailyAssignment struct, npc_farmer_yoon expanded with daily quest gossip, npc_andi_gift delivery, npc_mung_mung/npc_nana_mudclaw scripts, tutorial gate on quest 30257 (Learn and Grow V) with auto-farm-creation on login |
| 2026-05-26 | Phase 4B refactor | Replaced custom FriendshipManager with standard ReputationMgr (DBC faction IDs 1273-1284). Replaced custom DailyQuestManager with standard creature_queststarter/ender + QuestInfo ID 1. Removed character_tillers_friendship + character_tillers_daily_quests tables. Deleted tillers_dailies.cpp. Refactored npc_tillers_friendship.cpp, npc_farmer_yoon.cpp, go_tillers_shrine.cpp, npc_tillers_vote.cpp to use core APIs. Added character_tillers_npc_timers for food-once-per-day. Moved visiting farmer quests to their own NPCs. |
