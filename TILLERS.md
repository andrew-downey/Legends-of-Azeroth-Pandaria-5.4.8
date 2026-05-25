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
- [x] 4 plot states: UNTILLED → TILLED → GROWING → RIPE
- [x] 3 crop conditions: HEALTHY, PARCHED, PESTS, WEEDS
- [x] 2 special crop types: PLUMP (+3 yield), BURSTING (instant harvest)
- [x] Gossip-based plot interaction (till, plant, harvest, water, spray, pull weeds)
- [x] Character DB persistence for plot state and farm upgrades
- [x] In-memory cache with thread-safe `FarmDataManager`
- [x] Player login/logout load/save lifecycle
- [x] Zone-based spawn/despawn (Valley of the Four Winds zone 5805)
- [x] Bonus crop 10-day rotation schedule
- [x] Farm tool upgrade flags (irrigation, antipest, plow)
- [x] Votes mask for Tillers union progression
- [x] Water spell script (removes parched aura 115824)

### Known Bugs
- [CRITICAL] **Deadlock in `SaveToDB`** — `SaveToDB` locks `_mutex` then calls `SavePlot` which locks `_mutex` again. Will deadlock on every logout/save.
- [CRITICAL] **`unordered_map<ObjectGuid>` missing hash** — `ObjectGuid` has no `std::hash` specialization. Compile failure or undefined behavior.
- [BUG] `snprintf` with 64-byte buffers for dynamic crop names — potential truncation.
- [BUG] Deprecated `CAST_AI` macro usage instead of direct cast.
- [BUG] Redundant growth timer check in both `SpawnFarm` and `OnGossipHello`.

### Missing (vs Retail)

**Crop Interactions (Mini-Games):**
- [ ] Alluring crops — bird attacks, click soil to summon and kill
- [ ] Infested crops — bug spray required (beyond gossip)
- [ ] Wiggling crops — virmen under crop, click to summon and kill
- [ ] Smothered crops — pull weed mini-game
- [ ] Wild crops — wrestle mini-game
- [ ] Runty crops — jump to pull up
- [ ] Tangled crops — run to untangle vine
- [ ] Stubborn soil post-harvest — pull ability
- [ ] Occupied soil post-harvest — virmen in soil, click to expose and kill

**Reputation & Progression:**
- [ ] Tillers faction (ID 1074) reputation gain on harvest (50 rep at level 90)
- [ ] Friendship system with 10 individual Tiller NPCs
- [ ] Gift delivery via Andi daily quests
- [ ] Tiller gift rare drops from harvest

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

### Phase 1 — Critical Bugs & Code Quality
- Fix deadlock: `SavePlot` should not re-acquire mutex when called from `SaveToDB`
- Fix hash: switch `unordered_map` to `std::map` or provide `ObjectGuid` hash
- Replace `snprintf` with `std::string` / `std::format`
- Replace `CAST_AI` with direct `creature->AI()` cast
- Centralize growth timer check into single `UpdateGrowth` function
- Convert enums to `enum class` for type safety
- Add `[[nodiscard]]` to helpers, improve const-correctness
- Use `constexpr` for static data instead of `#define`

### Phase 2 — Crop Interactions & Reputation
- Implement all 9 crop interaction types as mini-game conditions
- Add post-harvest soil states (Stubborn, Occupied)
- Add Tillers faction reputation gain on harvest
- Add seed seed-drop chance on harvest (50%, 1-3 seeds)
- Add Tiller gift rare drop chance

### Phase 3 — Farm Upgrades & Tools
- Implement 15-minute in-game timer system for farm expansions
- Add Gai Lan (weeds→8 plots), Fish Fellreed (wagon→12), Haohan/Thunder (boulder→16)
- Implement tool upgrades: sprinkler, pest repellers, earth-slasher plow

### Phase 4 — Quests, Dailies, Friendship
- Daily quest system with Yoon, Andi, visiting farmers
- Friendship reputation system with 10 Tiller NPCs
- Vote questlines tracking
- Profession seeds and decorative seeds

### Phase 5 — Documentation & Verification
- Final TILLERS.md update reflecting completed state
- Build verification

## Progress Log

| Date | Phase | Changes |
|---|---|---|
| 2026-05-24 | Initial | Base farm system created: plot states, gossip, DB persistence |
| 2026-05-25 | Planning | Analysis complete, TILLERS.md created with full roadmap |
| 2026-05-25 | Phase 1 | Fixed deadlock (SavePlot_NoLock split), unordered_map→std::map, snprintf→std::string, CAST_AI→direct cast, enum class conversion, constexpr constants, [[nodiscard]] helpers, UpdateGrowthTimers centralization, reputation on harvest, SCALLION seed ID fix |
