# Tillers of the Valley — Implementation Status

> Permanent tracking document for the Tillers Union, Sunsong Ranch, Halfhill Market, and all associated systems.
> Last updated: 2026-05-31 (audit against disk)

---

## Current State

### C++ Source (Phase 2-3 Complete)
- **`src/server/scripts/Pandaria/TillersFarmMgr.h/.cpp`** — Core farm singleton (Meyers' pattern). Manages 16 soil plots per player, per-player phasing (PhaseMgr::SetCustomPhase), transactional DB persistence (CharacterDatabaseTransaction), zone enter/leave hooks, 64-bucket striped mutex for thread safety.
- **`src/server/scripts/Pandaria/TillersZoneHooks.cpp`** — PlayerScript OnUpdateZone/OnMapChanged for Valley of the Four Winds (zone 1023). Spawns/despawns farm on zone entry/exit.
- **`src/server/scripts/Pandaria/TillersSoilScript.cpp`** — GameObjectScript for Loosely Turned Soil (GO 186314). State-based gossip menus (seed planting, watering, pest removal, harvesting, repair).
- **`src/server/scripts/Pandaria/TillersWorkstation.cpp`** — Skeleton scripts for workbench and composter (TBD).
- **`src/server/scripts/Commands/cs_tillers.cpp`** — GM commands: `.tillers status`, `.tillers reset`, `.tillers setphase`, `.tillers grow`.
- **Friendship bar (Phase 3):** Generic faction derivation from `Creature->GetFactionTemplateEntry()->faction` — added to `PlayerMenu` in GossipDef.h/.cpp and set in `Player::SendPreparedGossip()` (Player.cpp:15636). Works for all gossip targets with a reputation track. No static NPC→faction map.
- **Registered in:** `Pandaria/CMakeLists.txt` (5 source files), `Commands/CMakeLists.txt` (auto-GLOB), `ScriptLoader/ScriptLoader.cpp` (4 AddTillers* exports).

### Database
- **2 Phase 1 migrations** (characters DB):
  - `characters/2026_05_28_00_tillers_farm_plots.sql` — player_farm_plots table (8 columns)
  - `characters/2026_05_28_01_tillers_farm_state.sql` — player_farm_state table (4 columns)
- **3 Phase 2-3 migrations** (world DB, untracked):
  - `world/2026_05_30_01_tillers_gift_quests.sql` — 50 gift quests (100100-100149), 5 gifts × 10 NPCs. REPLACE INTO SELECT FROM retail 30381. Per-NPC factions (1273-1283). RewardFactionValue1=6 (350 rep). VerifiedBuild=18414.
  - `world/2026_05_29_00_tillers_crop_creatures.sql` — creature_template entries 89001-89007 for crop visual stages
  - `world/2026_05_30_02_tillers_food_quests.sql` — 10 daily food gift quests (100150-100159), 1 food × 1 NPC each. REPLACE INTO SELECT FROM retail 30402. Uses retail food items 74642-74655 (deletes custom 110040-110044 from item_template).
- Faction IDs 1273-1283 exist in `Faction.dbc` with PEACE_FORCED flag (0x10). No migration needed.
- Gift items 79264-79268 exist. Food items 74642-74655 exist (retail). Farm tools 79104/80513/89880 exist. GO 186314 exists.

---

## Target State

The complete Tillers gameplay loop as a hybrid C++/SmartAI system:

### Architecture (Phase 2)
```
┌──────────────────────────────────────────────────┐
│                 TillersFarmMgr                    │
│  (C++ singleton — personal farm state manager)    │
│                                                   │
│  - player_farm_plots DB CRUD                     │
│  - Daily-reset-aligned maturity timestamp         │
│  - Per-player GO spawning/despawning              │
│  - Zone enter/leave hooks (spawn on entry)        │
└──────────────┬───────────────────────────────────┘
               │ calls / gossip
┌──────────────▼───────────────────────────────────┐
│              Soil Patch GOs (Script)               │
│  GAMEOBJECT 186314 (Loosely Turned Soil)          │
│  - Plot state gossip (empty/seeded/grown/ready)   │
│  - Consume seed + call FarmMgr::PlantSeed          │
│  - Water / pest-remove / harvest / repair menus    │
│  - Maturity checked on zone entry (no timer)      │
└──────────────┬───────────────────────────────────┘
               │ future
┌──────────────▼───────────────────────────────────┐
│              Crop CREATURES (Spellclick)           │
│  Creature entries 89001-89007 (visual stages)     │
│  - Growth stages via auras (future)               │
│  - Defect states handled by C++ (future)          │
│  - Harvest via spellclick (future)                │
│  - Loot + reset plot via FarmMgr (future)         │
└───────────────────────────────────────────────────┘
```

### Phase Mapping
| Phase | Area | Mechanism |
|-------|------|-----------|
| 1 | Halfhill (public) | No phasing — shared world |
| 2 | Sunsong Ranch | Per-player PhaseMask — `PhaseMgr::SetCustomPhase()` |
| 3 | Friendship system | ReputationMgr factions 1273-1283 + generic gossip bar |
| 4 | Tillers Union questline | QuestScripts + plot expansion |
| 5 | Daily quests + Work Orders | SQL + existing quest framework |
| 6 | Crop defects + minigames | C++ handlers per defect type |
| 7 | Farm machinery + Jogu prediction | GameObjectScript + SmartAI |

### Feature Inventory
- **Sunsong Ranch**: 16 soil patches (4→8→12→16), 10 vegetables + 6 special crops + 3 blossom trees
- **Growth**: Daily reset maturation (crops ready ~03:00 after planting)
- **GM commands**: `.tillers status/reset/setphase/grow`
- **Crop defects**: Alluring, Infested, Parched, Runty, Smothered, Tangled, Wiggling, Wild — 8 states (Phase 6)
- **Farm tools**: Rusty Watering Can, Vintage Bug Sprayer, Dented Shovel (existing items, no scripts needed)
- **Farm machinery**: Jinyu Princess (irrigation), Thunder King (pest repeller), Earth-Slasher (multi-plow) (Phase 7)
- **Jogu prediction**: NPC gossip with friendship-based discount (Phase 7)
- **Friendship**: 10 NPCs, 6 ranks each (Stranger→Best Friend) (Phase 3)
- **Gift turn-ins**: 50 gift quests (100100-100149, all 350 rep) + 10 food gift daily quests (100150-100159, all 350 rep)
- **Gossip friendship bar**: Generic — derived from creature faction template. Shows for any NPC with a reputation track.
- **Questline**: 27+ quests from Neutral to Exalted (Phase 4)
- **Dailies**: 5/day — Yoon planting + Andi gift + 2 visiting farmers + 1 killing (Phase 5)
- **Work orders**: 8 factions × 2/day limit (Phase 5)
- **Rewards**: 3 goats, tabard, recipes, pet, Grand Commendation

---

## Progress Log

### 2026-05-28 — Research & Planning Complete
| Action | Details | Ref |
|--------|---------|-----|
| Reputation system analysis | Faction IDs 1273-1283 exist, ReputationMgr can handle friendship | `ref:energetic-bronze-bear` |
| Farming architecture research | SmartAI insufficient; needs C++ FarmMgr + custom DB + per-player phasing | `ref:unknown-silver-rattlesnake` |
| DB transaction research | CharacterDatabaseTransaction + BeginTransaction/CommitTransaction. Auto-rollback on failure. | `ref:ses_18fba4798ffel7kWiH1mBSFG3v` |
| Audit SQL script created | 11-section diagnostics at `sql/audit/tillers_audit.sql` | `ref:m0059` |
| Gift items confirmed | Quest turn-ins (NOT ItemScript). 50 quests: 5 gifts × 10 NPCs. Repeatable, no daily limit. | user directive |
| Phase 1 SQL migrations created | 2 files: farm_plots, farm_state | — |

### 2026-05-29 — Phase 2: C++ FarmMgr Implementation
| Action | Details | Ref |
|--------|---------|-----|
| TillersFarmMgr.h/.cpp | Singleton with 64-bucket mutex, Spawn/Despawn/Plant/Harvest/Water/Pests/Repair/ForceGrow | `ref:b2` |
| TillersZoneHooks.cpp | PlayerScript OnUpdateZone/OnMapChanged for zone 1023 | `ref:b2` |
| TillersSoilScript.cpp | GO 186314 gossip per plot state | `ref:b2` |
| TillersWorkstation.cpp | Skeleton scripts (TBD) | `ref:b2` |
| cs_tillers.cpp | GM commands: status/reset/setphase/grow | `ref:b2` |
| Crop creatures SQL | creature_template 89001-89007 | `ref:b2` |
| Jogu SmartAI SQL | SMART_EVENT_UPDATE=60 | `ref:b2` |
| CMakeLists + ScriptLoader | All Tillers files registered | `ref:b2` |
| Growth timer simplified | Daily reset match (no background timer, .tillers grow command) | `ref:b3` |
| Code review fixes | C1-C4 (compile errors), M1-M4 (behavioral) — all fixed | `ref:b2` |
| Filename cleanup | Tillis→Tillers across all files | `ref:ses_18ba6a865ffeNXY2ItcrqeB37H` |

### 2026-05-29 — Phase 3: Friendship System Implementation
| Action | Details | Ref |
|--------|---------|-----|
| Gift quest SQL rewritten | REPLACE INTO for idempotency, per-NPC factions (1273-1283), RewardFactionValue1=6, VerifiedBuild=18414 | — |
| Gift quest SQL corrected (post-review) | Restored missing QuestCompletionLog for 49 rows. Fixed PortraitGiverName for 40 rows (copy-paste errors). | — |
| Food gift daily quests SQL | 10 quests (100150-100159), 1 food × 1 NPC each, repeatable, faction value 6 | — |
| Friendship bar (C++) GossipDef.h/.cpp | Added _friendFactionId field + setter. Used in SendGossipMenu packet serialization (line 279). Cleared in ClearMenus(). | — |
| Friendship bar (C++) Player.cpp | 3 lines before SendGossipMenu (line 15636): derive faction generically from creature->GetFactionTemplateEntry()->faction | — |

---

### 2026-05-30 — Gift/Food Quest SQL Rewrite
| Action | Details | Ref |
|--------|---------|-----|
| Broken gift quests rewritten | Replaced `2026_05_28_00_tillers_gift_quests.sql` with `2026_05_30_01_tillers_gift_quests.sql`. Uses SELECT FROM retail 30381 template + CROSS JOIN for idempotent bulk insert. Fixed column count (96). 50 quests moved from 32700-32749 to 100100-100149. | user directive |
| Broken food quests rewritten | Replaced `2026_05_29_02_tillers_food_gift_quests.sql` with `2026_05_30_02_tillers_food_quests.sql`. Uses SELECT FROM retail 30402 template. Fixed food item mappings to retail 74642-74655. 10 quests moved from 32750-32759 to 100150-100159. Custom food items 110040-110044 deleted. | user directive |
| All quests mapped to Andi | `creature_queststarter` and `creature_questender` set to Andi (64464) for all 60 quests (100100-100159). | user directive |
| Proper column counts | Fixed all tables: quest_template (96), quest_offer_reward (11), quest_request_items (5), quest_template_addon (18). | user directive |
| Phase 3 marked as pending | Gift/food quests not yet tested with client. Friendship bar C++ code needs separate fix. | — |
| Migrations applied to DB | Both files executed successfully against `world` database | `ref:m0045` |
| DB verification pass | All 60 quests, 60 creature_queststarter, 60 creature_questender verified; 0 broken stubs; 0 custom food items remaining; all SpecialFlags=1 (repeatable) | `ref:m0045` |
| Guide for Future AI Agents appended | Added to end of TILLERS.md — documents key facts (Andi=64464, SELECT FROM pattern, retail items) for future agents | `ref:m0041` |

---

## Next Steps

- [x] Phase 1: Audit + SQL migrations (farm tables) ✅
- [x] Phase 2: C++ FarmMgr implementation + zone hooks + soil GO + GM commands ✅
- [x] Phase 3: Friendship system — 10 NPC factions, 60 gift/food quests, generic gossip bar ✅
- [ ] Phase 4: Tillers Union questline — Learn and Grow + vote quests + plot expansion
- [ ] Phase 5: Daily quests + Work Orders
- [ ] Phase 6: Crop defects + tending minigames
- [ ] Phase 7: Farm machinery + Jogu prediction

### Current Focus
- **Phase 4**: Tillers Union questline — Learn and Grow + vote quests + plot expansion

## Corrections & Design Notes

- **Gift/food quest SQL (2026-05-30)**: Replaced two broken files (`2026_05_28_00_tillers_gift_quests.sql`, `2026_05_29_02_tillers_food_gift_quests.sql`) with clean versions using SELECT FROM retail template + CROSS JOIN bulk inserts. Old files had wrong column counts (96 header but ~91 values per row), wrong food item mappings, and no creature_queststarter/ender entries. New files fix all issues and map all 60 quests to Andi (64464).
- **Removed non-retail SQL migrations**: `2026_05_29_01_tillers_jogu_smartai.sql` (Jogu SmartAI) and `2026_05_30_00_tillers_core_fixes.sql` (custom GO/item templates, spellclick) were removed — they added custom versions of existing retail content. Jogu's SmartAI logic and core fixes were not keeping the retail experience.
- **Generic friendship bar**: The bar shows for ALL gossip NPCs with a reputation track (not just Tillers). This is intentional — `FactionTemplateEntry.faction` universally identifies the reputation faction, and clients only render the bar for tracked reputations. No static NPC→faction map needed.
- **Favorite/other distinction dropped**: All gift and food quests give 350 rep (DBC index 6). The retail favorite-vs-other bonus is not implemented.
- **Build**: Not yet compiled (user builds separately).

## Guide for Future AI Agents

### Key Facts Often Wrong
- **Andi NPC ID = 64464** (NOT 58712 — that's Kol Ironpaw, "Master of the Grill"). The file `sql/audit/tillers_audit.sql` incorrectly labels 58712 as Andi — do not trust it.
- **All 60 quests (100100-100159) use `REPLACE INTO ... SELECT FROM` a single retail template quest** (30381 "A Jade Cat for Ella" for gifts, 30402 "A Dish for Chee Chee" for food) with `CROSS JOIN` for bulk generation, then per-quest UPDATEs for NPC-specific overrides. This avoids hand-writing 96-column VALUES rows (which was the root cause of the broken files).
- **Do NOT use `INSERT INTO ... VALUES` for `quest_template`** — 96 columns, error-prone. Always SELECT FROM a matching retail quest with explicit column names.
- **Food items are retail 74642-74655**, NOT custom 110040-110044. Custom food items were deleted from `item_template` by `2026_05_30_02_tillers_food_quests.sql`.
- **Gift items are retail 79264-79268** (Ruby Shard, Blue Feather, Jade Cat, Lovely Apple, Marsh Lily) — all verified in item_template.

### Migration Order
1. `2026_05_29_00_tillers_crop_creatures.sql` — creature_template 89001-89007 (crop visual stages)
2. `2026_05_30_01_tillers_gift_quests.sql` — 50 gift quests (100100-100149). Also sets creature_queststarter/ender for all 60 quests.
3. `2026_05_30_02_tillers_food_quests.sql` — 10 food quests (100150-100159). Also deletes custom food items 110040-110044.

All are wrapped in START TRANSACTION/COMMIT and use REPLACE INTO (idempotent). Run in order.

### What NOT to Do
- Don't reinvent the SELECT FROM pattern — it's already proven by `battle_pet_trainer_spawns.sql`.
- Don't add custom items when retail items exist — keep it Blizzlike per AGENTS.md.
- Don't use VALUES for quest_template — always use SELECT FROM with explicit columns.
- Don't hardcode all 96 columns in a single INSERT — use CROSS JOIN + UPDATE for overrides.

### Key Files
- `sql/updates/world/2026_05_30_01_tillers_gift_quests.sql` — 50 gift quests ✅
- `sql/updates/world/2026_05_30_02_tillers_food_quests.sql` — 10 food quests ✅
- `sql/updates/world/2026_05_29_00_tillers_crop_creatures.sql` — creature_template 89001-89007 ✅
- `sql/updates/characters/2026_05_28_00_tillers_farm_plots.sql` — player_farm_plots table ✅
- `sql/updates/characters/2026_05_28_01_tillers_farm_state.sql` — player_farm_state table ✅
- `sql/updates/world/2026_05_22_00_battle_pet_trainer_spawns.sql` — Reference for SELECT FROM pattern
- `sql/audit/tillers_audit.sql` — Diagnostics (note: mislabels 58712 as Andi — do not trust)
- `research/wowhead-tillers-guide.md` — Retail data source for NPC→food→gift mappings
- `research/wowhead-tillers-quests.md` — Quest research data
- `TILLERS.md` (this file) — Permanent tracking document


# KNOWN ISSUES CONFIRMED IN-GAME
- There are 3 "Farmer Yoon" NPCs, one inside a house, one beside the farm, and one on sunsong ranch. 
    - The one on the farm is facing the wrong direction, provides no gossip or quests. He is close to the tool locations.
    - The one in the house appears to be identical to the one closest to the tillers shrine, provides quests and gossip.
    - The last one appears to be the "correct" one, at a glance.
- Farmer Fung has Old Hillpaw quests
- Jogu the Drunk's quest line is entirely available, instead of being chained.
- The Sunsong Wranch farm is unusable
    - There are 8 plots showing watermelons only
    - After Growing the Farm I, there are 2 cabbages that spawn in the wrong locations, but CAN be interacted with. This is expected behaviour, but wrong location. 
    - Additionally, the cabbages were auto planted instead of requiring the player to target an empty plot.
- .npc near 20 output while standing on the farm: |
516082 (Entry: 66129) - [Ripe Striped Melon X:-171.542999 Y:637.843994 Z:165.492004 MapId:870]
516076 (Entry: 55626) - [General Purpose Bunny (DLA) X:-171.542999 Y:637.843994 Z:165.492996 MapId:870]
516084 (Entry: 66129) - [Ripe Striped Melon X:-171.727005 Y:642.276001 Z:165.492004 MapId:870]
516111 (Entry: 55626) - [General Purpose Bunny (DLA) X:-171.727005 Y:642.276001 Z:165.492996 MapId:870]
516060 (Entry: 58719) - [Unbudging Rock X:-170.748001 Y:642.601013 Z:165.492996 MapId:870]
516083 (Entry: 66129) - [Ripe Striped Melon X:-166.279999 Y:636.965027 Z:165.492004 MapId:870]
516089 (Entry: 55626) - [General Purpose Bunny (DLA) X:-166.279007 Y:636.965027 Z:165.492996 MapId:870]
516085 (Entry: 66129) - [Ripe Striped Melon X:-166.520996 Y:641.210022 Z:165.492004 MapId:870]
516075 (Entry: 55626) - [General Purpose Bunny (DLA) X:-166.520996 Y:641.210022 Z:165.492996 MapId:870]
516110 (Entry: 66129) - [Ripe Striped Melon X:-172.070999 Y:646.583008 Z:165.492004 MapId:870]
516091 (Entry: 55626) - [General Purpose Bunny (DLA) X:-172.070999 Y:646.583008 Z:165.492996 MapId:870]
516079 (Entry: 66129) - [Ripe Striped Melon X:-166.535995 Y:645.502014 Z:165.492004 MapId:870]
516093 (Entry: 55626) - [General Purpose Bunny (DLA) X:-166.535995 Y:645.502014 Z:165.492996 MapId:870]
516057 (Entry: 58719) - [Unbudging Rock X:-160.729004 Y:634.591980 Z:165.492996 MapId:870]
516081 (Entry: 66129) - [Ripe Striped Melon X:-166.623001 Y:649.690979 Z:165.492004 MapId:870]
516094 (Entry: 55626) - [General Purpose Bunny (DLA) X:-166.623001 Y:649.690979 Z:165.492996 MapId:870]
516080 (Entry: 66129) - [Ripe Striped Melon X:-172.089005 Y:650.838989 Z:165.492004 MapId:870]
516092 (Entry: 55626) - [General Purpose Bunny (DLA) X:-172.089005 Y:650.838989 Z:165.492996 MapId:870]
516061 (Entry: 58721) - [Farmer Yoon X:-159.462997 Y:633.603027 Z:165.408997 MapId:870]
516118 (Entry: 69098) - [Hillpaw's Chicken X:-159.020996 Y:633.130005 Z:165.408997 MapId:870]
516067 (Entry: 58646) - [Farmer Yoon X:-180.843994 Y:628.357971 Z:165.492996 MapId:870]
516058 (Entry: 58719) - [Unbudging Rock X:-179.028000 Y:650.283020 Z:165.507996 MapId:870]
516070 (Entry: 44775) - [Generic Bunny - PRK (Large AOI) X:-163.729004 Y:644.500000 Z:176.302002 MapId:870]
516095 (Entry: 55626) - [General Purpose Bunny (DLA) X:-156.000000 Y:638.984009 Z:165.492996 MapId:870]
516078 (Entry: 59669) - [Offering Bowl X:-187.434006 Y:637.083008 Z:166.190002 MapId:870]
516096 (Entry: 55626) - [General Purpose Bunny (DLA) X:-156.382004 Y:643.403015 Z:165.492996 MapId:870]
516097 (Entry: 55626) - [General Purpose Bunny (DLA) X:-156.766998 Y:647.411011 Z:165.492996 MapId:870]
- Hillpaw's Chicken, Shaggy, Dog, and possibly others, appear on the farm when they have not yet been unlocked (possible phasing issue)
- Farmer Yoon is offering ALL quests from the very beginning, with no gating or requirements.
