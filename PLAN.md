# Tillers Phase 4B Refactor Plan

## Goal
Replace the custom gossip-based daily quest system and custom FriendshipManager with the proper WoW faction reputation system + standard creature_queststarter/finisher.

## Key Discoveries (Faction.dbc: ~/warcraft-server/bin/Data/dbc/Faction.dbc)

The DBC already defines per-NPC factions for all 10 Tillers NPCs:

| DBC ID | repListID | Name | Parent |
|---|---|---|---|
| 1272 | 171 | The Tillers (main) | 1245 |
| 1273 | 156 | Jogu the Drunk | 1272 |
| 1275 | 158 | Ella | 1272 |
| 1276 | 159 | Old Hillpaw | 1272 |
| 1277 | 160 | Chee Chee | 1272 |
| 1278 | 161 | Sho | 1272 |
| 1279 | 162 | Haohan Mudclaw | 1272 |
| 1280 | 163 | Tina Mudclaw | 1272 |
| 1281 | 164 | Gina Mudclaw | 1272 |
| 1282 | 165 | Fish Fellreed | 1272 |
| 1283 | 166 | Farmer Fung | 1272 |

All have CanHaveReputation() = true. The old constants (2091-2100 and FACTION_TILLERS=1074) are wrong.

## Rep system replaces FriendshipManager

- `player->GetReputationMgr().ModifyReputation(sFactionStore.LookupEntry(factionId), gain)` replaces every `sFriendship->ModifyStanding(...)`
- `player->GetReputationMgr().GetReputation(sFactionStore.LookupEntry(factionId))` replaces `sFriendship->GetFriendship()->standing`
- The built-in `character_reputation` table handles persistence (auto-loaded/saved by core)
- Friendship daily food caps use a NEW lightweight table `character_tillers_npc_timers(guid, npc_entry, last_food_day)`
- Dark Soil gifts are unlimited (no timer needed per guide)
- Friendship ranks (Stranger→Best Friend) still tracked via reputation standing value

## Daily quests use standard quest system

- Server-wide daily rotation (deterministic from calendar day, no per-player DB storage)
- Quests offered through `creature_queststarter` + `creature_questfinisher` in DB
- `QuestMenu::AddMenuItem()` shows proper quest dialog instead of gossip strings
- No more `DailyQuestManager`, `sDailyQuest`, or `character_tillers_daily_quests`

## Server-wide Daily Rotation

```cpp
uint32 todaySeed = uint32(time(nullptr) / 86400);
// Crop dailies: 10 pool, 5 active per day (first half or second half)
bool cropSecondHalf = (todaySeed % 2 == 1);
// Kill dailies: 5 pool, 1 active per day
uint32 killIndex = todaySeed % 5;
// Visiting farmers: 8 pool, 2 active per day (consecutive pairs)
uint32 visitBase = (todaySeed / 7) % 4;  // 0,1,2,3 → picks pairs (0,1), (2,3), (4,5), (6,7)
```

---

## Files to Change

### 1. tillers_farm.h — Constants cleanup

**Fix faction IDs:**
- FACTION_TILLERS: 1074 → 1272
- Individual NPC factions: 2091-2100 → 1273-1283 (as mapped above)

**Remove:**
- FriendshipRank enum, FriendshipEntry struct, FriendshipManager class, sFriendship macro
- DailyQuestData struct, DailyQuestManager class, sDailyQuest macro
- CropDailyAssignment struct + CropDailies extern
- All QUEST_CROP_*, QUEST_KILL_*, QUEST_VISITING_*, QUEST_ANDI_GIFT constants
- All DAILY_* reward constants
- ITEM_ANDI_GIFT
- VISITING_FARMER_EXCLUDED array
- FRIENDSHIP_DAILY_FOOD_GAIN, FRIENDSHIP_DAILY_GIFT_GAIN, FRIENDSHIP_IDEAL_GIFT_GAIN
- FRIENDSHIP_DAILY_RESET_INTERVAL

**Rename:**
- GetFriendshipRank() → GetFriendlyRank()
- GetFriendshipRankName() → GetFriendlyRankName()
- FRIENDSHIP_DAILY_FOOD_GAIN (1800) → NPC_FOOD_STANDING_GAIN
- FRIENDSHIP_DAILY_GIFT_GAIN (540) → NPC_GIFT_STANDING_GAIN
- FRIENDSHIP_IDEAL_GIFT_GAIN (900) → NPC_IDEAL_GIFT_STANDING_GAIN

**Keep (unchanged):**
- NPC constants (all 58703-58746), ITEM_*, SPELL_*, FARM_* constants
- FarmPlotState, CropType, FarmCondition, SpecialCrop enums
- FarmPlotData, FarmToolUpgrades, PlayerFarmCache structs
- Vote constants (VoteData, VOTE_*, VoteRequirements)
- Gift/food item constants
- NPC_CHEE_CHEE etc. entries
- DarkSoilTreasures
- Helper function declarations
- CreatePlayerFarm

**Add:**
- NPC_FOOD_STANDING_GAIN (1800), NPC_GIFT_STANDING_GAIN (540), NPC_IDEAL_GIFT_STANDING_GAIN (900)
- GetFactionIdForNpc(uint32 npcEntry) helper (maps NPC entry → faction ID)
- AddSC_tillers_vote() declaration (keep for minimal vote script)
- Remove AddSC_tillers_dailies() declaration
- Remove AddSC_npc_farmer_yoon() (dead code)

### 2. tillers_farm.cpp — Remove FriendshipManager

**Remove:**
- FriendshipManager::LoadFromDB (full method)
- FriendshipManager::SaveToDB (full method)
- FriendshipManager::Unload (full method)
- FriendshipManager::GetFriendship (full method)
- FriendshipManager::ModifyStanding (full method)
- FriendshipManager::UpdateDailyFoodTimer (full method)
- FriendshipManager::UpdateDailyGiftTimer (full method)
- sFriendship->LoadFromDB(player) in OnLogin
- sFriendship->SaveToDB/Save/Unload in OnLogout

**Add:**
- NpcFactionMap (static array mapping NPC entry → faction ID)
- GetFactionIdForNpc() implementation
- GetFriendlyRank() / GetFriendlyRankName() renames

### 3. npc_farmer_yoon.cpp — Daily quest rotation + proper QuestMenu

**Remove:**
- All sDailyQuest-> calls (9 call sites)
- The entire daily quest gossip section (crop/kill/Andi/visiting)
- All references to tillers_dailies.h/CropDailyAssignment
- npc_farmer_yoon_dailies class (if merged)
- Any MOD quest code + sDailyQuest->CompleteDaily patterns

**Add:**
- OnGossipHello: PrepareQuestMenu + ClearMenu + AddMenuItem for today's 5 crop + 1 kill
- Server-wide rotation helper functions (GetTodaySeed, IsCropDailyActive, GetActiveKillDaily)
- Keep: expansion gossip, tool installation, tutorial gate, good-bye

**Flow:**
```
OnGossipHello:
  1. Clear quest menu
  2. For each of 10 crop dailies: if in today's half, AddMenuItem(questId, 2)
  3. Add 1 kill: AddMenuItem(killQuestId, 2)
  4. If no farm + quest not done: show tutorial gossip
  5. If has farm: show expansion gossip + tool gossip
  6. Goodbye
  7. SendGossipMenuFor
```

### 4. npc_tillers_friendship.cpp — Reputation API + food timer

**Replace everywhere:**
```
sFriendship->ModifyStanding(guid, npc, amt)
→ player->GetReputationMgr().ModifyReputation(sFactionStore.LookupEntry(fid), amt)

sFriendship->GetFriendship(guid, npc)->standing
→ player->GetReputationMgr().GetReputation(sFactionStore.LookupEntry(fid))

sFriendship->UpdateDailyFoodTimer(guid, npc)
→ CheckFoodTimer(player, npc)  // new helper
sFriendship->UpdateDailyGiftTimer(guid, npc)
→ remove (gifts unlimited)
```

**Food timer implementation (inline or free function):**
```
CanGiveFoodToday(player, npcEntry):
   today = time/86400
   SELECT last_food_day FROM character_tillers_npc_timers WHERE guid=X AND npc_entry=Y
   return (no row) OR (last_food_day < today)

SetFoodGivenToday(player, npcEntry):
   today = time/86400
   INSERT ... ON DUPLICATE KEY UPDATE last_food_day = today
```

**Visiting farmer quest filtering in OnGossipHello:**
Each Tiller NPC's gossip should check if their visiting quest is active today. If not active, don't show quest in menu. Use `IsVisitingFarmerActive(npcEntry)`.

**Vote quest OnQuestReward:**
For Gina, Fung, Haohan (scripts already here): add OnQuestReward that sets vote bit in farm data.
For Mung-Mung, Nana: handled in npc_tillers_vote.cpp (minimal script).

### 5. go_tillers_shrine.cpp — Use reputation API

**Replace:**
```
FriendshipEntry* f = sFriendship->GetFriendship(guid, npc);
int32 standing = f->standing;
```
With:
```
uint32 fid = GetFactionIdForNpc(npc);
int32 standing = player->GetReputationMgr().GetReputation(sFactionStore.LookupEntry(fid));
```

And use GetFriendlyRank() / GetFriendlyRankName().

### 6. tillers_dailies.cpp — DELETE

Full removal. Contains DailyQuestManager, npc_farmer_yoon_dailies, npc_andi_gift, npc_visiting_farmer, AddSC_tillers_dailies.

Andi gift quest handled by standard creature_queststarter (58708 → 32010).

### 7. npc_tillers_vote.cpp — Strip to minimal

**Remove:** npc_mung_mung gossip, npc_nana_mudclaw gossip, shared HandleVoteGossip/HandleVoteSelect helpers.

**Keep:** Minimal script with OnQuestReward for vote bit setting:
- npc_mung_mung_vote: OnQuestReward → set VOTE_MUNG_MUNG bit
- npc_nana_mudclaw_vote: OnQuestReward → set VOTE_NANA bit

### 8. ScriptLoader.cpp

- Remove: `void AddSC_tillers_dailies();` declaration (line ~1027)
- Remove: `AddSC_tillers_dailies();` call (line ~2096)
- Keep: `AddSC_tillers_vote();` (the minimal version still exists)
- Remove: `AddSC_npc_farmer_yoon();` (dead code, was never called)

### 9. CMakeLists.txt

- Remove: `Pandaria/TillersFarm/tillers_dailies.cpp` line 43
- Keep: all others unchanged

---

## SQL Changes

### Characters SQL (new file: characters/2026_05_26_02_characters_tillers_refactor.sql)
```sql
-- Drop old custom tables
DROP TABLE IF EXISTS `character_tillers_daily_quests`;
DROP TABLE IF EXISTS `character_tillers_friendship`;

-- New lightweight food timer (once-per-day per-NPC food tracking)
CREATE TABLE `character_tillers_npc_timers` (
    `guid` INT UNSIGNED NOT NULL,
    `npc_entry` INT UNSIGNED NOT NULL,
    `last_food_day` INT UNSIGNED NOT NULL DEFAULT 0,
    PRIMARY KEY (`guid`, `npc_entry`)
);
```

### World SQL (new file: world/2026_05_26_04_tillers_quests.sql)
```sql
-- Creature quest starters
-- Farmer Yoon: all 15 dailies (10 crop + 5 kill)
INSERT IGNORE INTO `creature_queststarter` (`id`, `quest`) VALUES
(58646, 32000), (58646, 32001), (58646, 32002), (58646, 32003),
(58646, 32004), (58646, 32005), (58646, 32006), (58646, 32007),
(58646, 32008), (58646, 32009),
(58646, 32019), (58646, 32020), (58646, 32021), (58646, 32022),
(58646, 32023);

-- Andi: gift daily
INSERT IGNORE INTO `creature_queststarter` (`id`, `quest`) VALUES (58708, 32010);

-- Visiting farmers: their quests at their Halfhill spots
INSERT IGNORE INTO `creature_queststarter` (`id`, `quest`) VALUES
(58734, 32011), (58732, 32012), (58730, 32013), (58735, 32014),
(58736, 32015), (58737, 32016), (58738, 32017), (58739, 32018);

-- Creature quest finishers (blue question mark for turn-in)
INSERT IGNORE INTO `creature_questfinisher` (`id`, `quest`) VALUES
(58708, 32010),
(58734, 32011), (58732, 32012), (58730, 32013), (58735, 32014),
(58736, 32015), (58737, 32016), (58738, 32017), (58739, 32018);
```

### World SQL (new file: world/2026_05_26_05_tillers_cleanup_scriptnames.sql)
```sql
-- Remove ScriptName for Andi (was npc_andi_gift, now handled by DB quest system)
UPDATE `creature_template` SET `ScriptName` = '' WHERE `entry` = 58708;
```

---

## Tables to Keep/Drop

| Table | Action | Reason |
|---|---|---|
| character_tillers_daily_quests | DROP | Replaced by standard character_queststatus |
| character_tillers_friendship | DROP | Replaced by character_reputation |
| character_tillers_npc_timers | CREATE | Only for once-per-day food cap |
| character_tillers_farm | KEEP | Farm plot data still needed |

---

## Implementation Order

1. tillers_farm.h — constants + header cleanup
2. tillers_farm.cpp — remove FriendshipManager, add helpers
3. npc_tillers_friendship.cpp — rep API + food timer + visiting quest filtering
4. npc_farmer_yoon.cpp — quest rotation + QuestMenu
5. go_tillers_shrine.cpp — rep API
6. npc_tillers_vote.cpp — strip to minimal
7. tillers_dailies.cpp — DELETE
8. ScriptLoader.cpp — cleanup
9. CMakeLists.txt — cleanup
10. SQL files — create migrations
11. Build & verify
