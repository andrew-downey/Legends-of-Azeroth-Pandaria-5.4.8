# Pet Battle System — Documentation

## File Locations

| File | Purpose |
|---|---|
| `src/server/game/BattlePet/PetBattle.cpp` | Core battle logic |
| `src/server/game/BattlePet/PetBattle.h` | Types, enums, `PetBattleTeam` struct |
| `src/server/game/BattlePet/BattlePetSpawnMgr.cpp` | Wild pet spawning, GUID management |
| `src/server/game/BattlePet/BattlePetTrainerMgr.cpp` | Trainer roster loading |
| `src/server/game/Handlers/BattlePetHandler.cpp` | Client packet handlers |
| `src/server/game/BattlePet/BattlePetAbilityEffect.cpp` | Ability effect handlers (damage, heal, aura, weather, etc.) |
| `src/server/game/BattlePet/BattlePetAura.cpp` | Aura lifecycle: `OnApply()`, `Process()`, `Expire()` |

## Battle Flow

### 1. Battle Start

1. Player right-clicks trainer → `CMSG_PET_BATTLE_REQUEST_UPDATE` → `PetBattle` created (state: `Created`)
2. `SendInitialUpdate()` → client shows pet selection UI
3. State → `WaitingForFrontPets`
4. Player sends `CMSG_PET_BATTLE_SET_FRONT_PET` (pet 1) → player team `m_ready = true`
5. Trainer team already ready (set via `SetActivePet(GetPet(0))` in `AddPlayer()`)
6. Both ready → `StartBattle()` → `SendFirstRound()` → state → `InProgress`

### 2. Round Loop

1. Player sends `CMSG_PET_BATTLE_INPUT` (ability or swap) → `m_ready = true`
2. `Update()` tick: both ready → `HandleRound()`
3. Pending moves processed: trainer casts ability, player casts ability or swaps pet
4. `GetFirstAttackingTeam()` — faster pet attacks first (based on speed stat)
5. Round start procs → abilities execute → round end procs
6. Auto-swap trainer dead pets (if any) before sending round result
7. Auras processed, stat updates sent
8. Cooldowns decremented, then round result sent to player
9. `TurnFinished()` on both teams → trainer auto-queues ability, player `m_ready = false`
10. Death check → repeat

### 3. Pet Swapping

Players can swap pets at any time (alive or dead). Two separate packet paths:

**Swap alive pet** — `CMSG_PET_BATTLE_INPUT` with `PET_BATTLE_MOVE_TYPE_SWAP_OR_PASS`:
1. Player sends `CMSG_PET_BATTLE_INPUT` with move type 2 + pet index → `SetPendingMove(SWAP_OR_PASS)` → `m_ready = true`
2. `HandleRound()` processes pending move → `SwapActivePet()` → new pet active for this round
3. `CanSwap()` validates: no multi-turn ability running, no swap lock from abilities (e.g., Sticky Web, Banished)

**Swap dead pet** — `CMSG_PET_BATTLE_SET_FRONT_PET`:
1. Player sends `CMSG_PET_BATTLE_SET_FRONT_PET` with pet index
2. Server validates: active pet must be dead (line 852-855 in `BattlePetHandler.cpp`)
3. `SetPendingMove(SWAP_DEAD_PET)` → `m_ready = true`
4. Next `HandleRound()` processes swap → new pet active

**Trainer dead pet** — auto-swap in `HandleRound()`:
1. Pet dies in round → `HandleRound()` auto-swaps trainer dead pet to next available pet before `SendRoundResult()`
2. Client receives pet death + swap effect in same round result packet, allowing it to proceed correctly
3. `TurnFinished()` also queues `SWAP_DEAD_PET` pending move for trainer (used for AI ability selection on the new pet)

### 4. Pet Death

1. Pet dies in round → `Kill()` sets round result
2. For trainer battles: `HandleRound()` auto-swaps dead trainer pet before sending round result
3. For wild pet battles: `CATCH_OR_KILL` round result is sent, triggering catch screen
4. `TurnFinished()` handles trainer AI: sets `SWAP_DEAD_PET` pending move and picks new ability
5. Player must send `CMSG_PET_BATTLE_SET_FRONT_PET` → `SetPendingMove(SWAP_DEAD_PET)` for their own dead pet
6. Next round: both ready → swap processed → new pet active

### 5. Battle End

1. One team has no alive pets → `EndBattle(lostTeam)`
2. XP calculation, achievement credit, quest credit for defeated trainer
3. State → `Finished`

## Key Design Principles

- **Trainer and player death flows are symmetric.** Both use `SetPendingMove()` → `m_ready = true` → next `HandleRound()`. The only difference is the trainer auto-submits via `TurnFinished()` while the player sends explicit client input.
- **`TurnFinished()` is the single entry point for PvE team decision-making** — both trainer and wild opponents use the same `!m_owner` path.
- **`HandleRound()` MUST auto-swap trainer dead pets before `SendRoundResult()`.** The client needs the swap effect in the same round as the pet death to proceed correctly.
- **`CATCH_OR_KILL` round result is ONLY for wild pet battles (`PET_BATTLE_TYPE_PVE`).** Trainer pet deaths send `NORMAL` round result.
- **Players can swap pets at any time** (alive or dead). Two packet paths:
  - `CMSG_PET_BATTLE_INPUT` with `PET_BATTLE_MOVE_TYPE_SWAP_OR_PASS` — swap alive pet mid-round
  - `CMSG_PET_BATTLE_SET_FRONT_PET` — initial pet selection AND dead pet replacement only
- **`CMSG_PET_BATTLE_INPUT`** is used for both ability selection AND alive pet swaps.
- **`CanSwap()`** validates swap eligibility: no multi-turn ability running, no swap locks from abilities (e.g., Sticky Web, Banished), target pet must be alive.
- **The `m_ready` flag is the synchronization mechanism** — both teams must be ready before `HandleRound()` runs.
- **Weather effects (Effect 80) target all pets on both teams** via `PET_BATTLE_ABILITY_TARGET_ALL`. The aura is applied to each pet individually with per-pet state modifiers (e.g., reduced healing taken). All weather abilities have `MaxAllowed=0` in DB2 (passed via `Properties[3]`), meaning weather auras are not capped per-pet — multiple weather effects can coexist.
- **Quest credit for trainer battles uses `QUEST_OBJECTIVE_WINPETBATTLEAGAINSTNPC` (type 11),** not `QUEST_OBJECTIVE_MONSTER` (type 0). `KilledMonsterCredit` handles type 0; `PetBattleCompleteQuest` handles type 11.

## Critical Learnings (Database)

Hard-won lessons from provisioning 51 pet battle tamers:

- **AllowableRaces bitmask**: Uses `(1 << (race-1))` where race is ChrRaces ID (Human=1, Orc=2, etc.). Two known masks: Alliance = 18875469 (Human+Dwarf+NightElf+Gnome+Draenei+Worgen+AlliPanda), Horde = 33555378 (Orc+Undead+Tauren+Troll+Goblin+BloodElf+HordePanda), both = 0.
- **creature.id not creature_id**: The column in the `creature` table is `id`, not `creature_id`. Queries using the wrong name silently return empty.
- **PrevQuestID limit**: TrinityCore supports only a single `PrevQuestID`. Faction-gated quest chains require separate duplicate quest IDs for Horde and Alliance — a single quest with conditional PrevQuestID is not possible.
- **curhealth=1 = dead**: Creature spawns with `curhealth=1` appear dead/non-interactable. Fix: `curhealth = level × zone_multiplier` (Classic ×30, Outland ×130, Northrend ×100, Cataclysm ×135).
- **creature_queststarter required**: `npcflag=2` (QUEST_GIVER) alone does not make an NPC show quest dialog. At least one row in `creature_queststarter` is required.
- **gossip_menu_id=900001**: Minimal gossip menu (TextID=1) used for NPCs that only give quests but need a non-zero `gossip_menu_id`. Created via `DELETE+INSERT` and applied with `UPDATE WHERE gossip_menu_id=0`.
- **quest_objective type=11**: Pet battle trainer quests use `QUEST_OBJECTIVE_WINPETBATTLEAGAINSTNPC` (type 11), not type 0 (`MONSTER`). Always check both `quest_objective` and `battle_pet_trainer` when auditing tamers.
- **Quest flags**: Daily = 4096 (0x1000), First-time = 524288 (0x80000), Auto-accept = 262144 (0x40000), Zone completion = 327680 (0x50000).
- **Varzok:63626 pre-existing issue**: Has `npcflag=50` (QUEST_GIVER+TAXI+TRAINER) but `gossip_menu_id=0`, causing "wrong gossip menu" error. Pre-existing, not introduced by our changes.

## Quest Chain Architecture

### First-time chains (one per faction, sequential unlock)

**Horde (Kalimdor):**
```
Varzok:63626 → Zunta(31812) → Dagra(31813) → Analynn(31814) → Zonya(31815) → Merda(31817) → Cassandra(31870) → A Tamer's Homecoming:31918
```
**Alliance (Eastern Kingdoms):**
```
Audrey Burnhep:63596 → Julia Stevens(31316) → Old MacDonald(31724) → Lindsay(31725) → Eric Davidson(31726) → Steven Lisbane(31729) → Bill Buckler(31728) → A Tamer's Homecoming:31917
```

Completing "A Tamer's Homecoming" gates the zone-completion quests and (via PrevQuestID) the lower-tier daily quests for Classic trainers.

### Zone completions → Grand Masters

Each zone requires defeating all world tamers in that zone, then gates a Grand Master:

| Zone | Alliance | Horde | Gated Behind | Unlocks |
|---|---|---|---|---|
| Kalimdor | 31889 | 31891 | Homecoming(31917/31918) | GM Trixxy (31897) |
| Eastern Kingdoms | 33014 | 31903 | Homecoming(31917/31918) | GM Lydia Accoste (31915) |
| Outland | 31919 | 31921 | Returning Champion chain | GM Antari (31920) |
| Northrend | 31927 | 31929 | Exceeding Expectations | GM Payne (31928) |
| Cataclysm | 31966 | 31967 | A Brief Reprieve | GM Obalis (31970) |
| Pandaria | 31930 | 31952 | The Triumphant Return | GM Aki (31951) |

### Daily quests

Two categories:
- **Grand Master dailies** (Zunta 31818, Hyuna 31953, Outland/Northrend/Cata/Pandaria): `PrevQuestID=0` — always available after first-time completion.
- **Classic lower-tier dailies** (Lindsay, Dagra, Eric, Bill, Steven, Analynn, Zonya, Merda, Cassandra): `PrevQuestID` set to their faction's Homecoming quest (31917 Alliance / 31918 Horde). Requires completing the first-time chain first.

**Faction gating fix**: The 5 Horde classic dailies (Dagra 31819, Analynn 31854, Zonya 31862, Merda 31872, Cassandra 31904) were originally gated behind Alliance quest 31917. Horde copies created at 33009-33013 with `PrevQuestID=31918`.

## Trainer Provisioning

51 unique pet battle tamers were identified by cross-referencing three data sources:
1. `quest_objective WHERE type = 11` (WINPETBATTLEAGAINSTNPC)
2. `battle_pet_trainer` (pet team assignments)
3. `creature` spawns (world placement)

| Metric | Count |
|---|---|
| Total unique tamers (quest_objective type=11) | 51 |
| Pre-existing spawns (before this work) | 16 |
| New creature spawns added | 37 |
| Tamers with battle_pet_trainer data | 51 (Brok, Jeremy Feasel, Little Tommy added) |
| Zoneless tamers (wrathion:73138) | 1 (no battle quest, legendary NPC) |

**Spawn placement**: World tamers are placed +4 units east of the nearest innkeeper in their zone. Health = level × zone_multiplier (Classic ×30, Outland ×130, Northrend ×100, Cataclysm ×135).

All changes in `sql/updates/world/2026_05_22_00_battle_pet_trainer_spawns.sql`.

## Fixes Applied

| # | File | Fix |
|---|---|---|
| 1 | `BattlePetSpawnMgr.cpp` | `erase(guid)` instead of `erase(replacementGUID)` — duplicate GUID crash when trainer swaps pets |
| 2 | `BattlePetSpawnMgr.cpp` | `break;;` → `break;` — syntax bug |
| 3 | `PetBattle.cpp` | Removed `else if (m_owner && !m_ready)` auto-pass block — player never got a turn |
| 4 | `PetBattle.h` | Removed orphaned `SetReady()` declaration |
| 5 | `PetBattle.cpp` | `HandleRound()` auto-swap for trainer dead pets with `ignoreAlive=true` — client needs swap in same round as death |
| 6 | `PetBattle.cpp` | CATCH_OR_KILL round result only for wild battles; trainer pet deaths send NORMAL |
| 7 | `PetBattle.cpp` | Removed duplicate `PET_BATTLE_EFFECT_ACTIVE_PET` from `SetActivePet()` — `SwapActivePet()` already adds it |
| 8 | `PetBattle.cpp` | Moved cooldown decrement from `TurnFinished()` to before `SendRoundResult()` — cooldowns were one turn too high |
| 9 | `BattlePetAbilityEffect.cpp` | Weather effect target changed from `HEAD` to `ALL` — weather auras now apply to all 6 pets |
| 10 | `Player.cpp/h`, `PetBattle.cpp` | Added `PetBattleCompleteQuest()` for type 11 objectives — trainer victory correctly completes pet battle quests |
| 11 | `PetBattle.cpp` | Fixed `GetInputStatusFlags()` — dead-pet and swap-lock checks are now mutually exclusive, preventing conflicting `LOCK_PET_SWAP` + `SELECT_NEW_PET` flags from suppressing the swap prompt on player pet death |
| DB | creature_queststarter/ender | Added first-time chain starters/enders for 19 quests; daily quest links for 13 trainers; 6 new quests (33009-33014) |
| DB | creature | 37 new spawns for pet tamers; curhealth fix for 8 pre-existing spawns (1→correct value) |
| DB | gossip_menu | Created menu_id=900001 (TextID=1), applied to 37 trainers with gossip_menu_id=0 |
| DB | battle_pet_trainer | Added pet teams for Brok, Jeremy Feasel, Little Tommy Newcomer |
| 12 | `BattlePetAbilityEffect.cpp` | `HandlePeriodicPositiveTrigger` (Effect 63): added `maxAllowed=1` to `AddAura` — prevents periodic buffs/HoTs from stacking infinitely |
| 13 | `BattlePetAbilityEffect.cpp` | `HandleDamageToggleAura` (Effect 76): added `maxAllowed=1` to `AddAura` on first-apply; replaced `GetAura()` with a full loop to expire all matching auras on toggle — prevents toggle auras from stacking |
| 14 | `BattlePetAbilityEffect.cpp` | `HandlePowerlessAura` (Effect 178): added `maxAllowed=1` to `AddAura` — prevents powerless-type debuffs from stacking |
| 15 | `PetBattle.cpp` | `AddAura()`: moved `m_effects.push_back(effect)` after `if (flags) return;` — fixes client-server desync where client received phantom aura-apply for missed/blocked attacks |
| 16 | `BattlePetAbilityEffect.cpp` | `HandlePeriodicTrigger` (Effect 54): changed `maxAllowed` from DBC `Properties[3]` to hardcoded `1` — prevents periodic DoTs (like poison) from stacking |
| 17 | `BattlePetAbilityEffect.cpp` | `HandleNegativeAura` (Effect 50): changed `maxAllowed` from DBC `Properties[3]` to hardcoded `1` — prevents debuff auras from stacking |

## Wild Pet Spawning System

### Two Approaches

**Approach A — Type 8 critter replacement (Old World):**
Pool `entry` references a type 8 critter (e.g., `721` Rabbit) that already spawns in the zone. The replacement system picks it up in `OnAddToZone()`, adds it to `CreaturesReadyForReplace`, then `SpawnCreature()` creates a new creature with the battle pet's `NpcId` from `BattlePetSpecies.db2` and gives it `UNIT_NPC_FLAG_WILDPET_CAPTURABLE`.

**Approach B — Type 14 direct spawn (MoP, Pandaria):**
The battle pet's NpcId is placed directly in the `creature` table as a type 14 spawn (no special flags in DB — `npcflag=0`). Pool `entry` references the NpcId itself. The replacement system re-creates the same creature with the `WILDPET_CAPTURABLE` flag at runtime. Used for all MoP wild pets (Crested Owl, Bandicoon, etc.) — 52 pool entries with matching creature spawns.

### Hybrid Zones

A zone can mix both approaches. Example: Feralas uses type 8 replacement for Rabbit/Squirrel/Snake (common pets with existing critters) and type 14 direct spawns for Nether Faerie Dragon / Stunted Yeti (unique pets with no matching critter).

### Key Code Paths

| Path | File | Purpose |
|------|------|---------|
| `BattlePetSpawnMgr::Initialise()` | `BattlePetSpawnMgr.cpp:27` | Loads `battle_pet_wild_pool` into `m_battlePetMapPools` |
| `BattlePetSpawnMgr::OnAddToMap()` | `BattlePetSpawnMgr.cpp:115` | Called when creature added to map → delegates to zone mgr |
| `BattlePetSpawnZoneMgr::OnAddToZone()` | `BattlePetSpawnMgr.cpp:285` | Matches creature entry to pool template, adds to replace queue |
| `BattlePetSpawnZoneMgr::PopulateZone()` | `BattlePetSpawnMgr.cpp:256` | Every 2s, replaces queued critters with battle pets |
| `BattlePetSpawnZoneMgr::SpawnCreature()` | `BattlePetSpawnMgr.cpp:318` | Creates battle pet with `WILDPET_CAPTURABLE`, despawns original |
| `BattlePetSpawnMgr::GetWildBattlePet()` | `BattlePetSpawnMgr.cpp:203` | Looks up battle pet data for a creature GUID |

### Duplicate Check

At `BattlePetSpawnMgr.cpp:83-91`: Two species in the same zone cannot share the same `entry` (base critter/NpcId). This prevents one critter from being replaced by two different battle pets.

### Creature Table Pattern for Type 14 Spawns

```sql
INSERT INTO creature (id, map, zoneId, areaId, spawnMask, phaseMask, position_x, position_y, position_z, orientation, spawntimesecs, wander_distance, MovementType, VerifiedBuild) VALUES
(<npcId>, <map>, <zoneId>, <areaId>, 1, 1, <x>, <y>, <z>, <orient>, 300, 5, 1, 0);
```

- `spawntimesecs=300` (5 min respawn — standard across all wild pet spawns)
- `wander_distance=5` (they roam a bit from their anchor point)
- `MovementType=1` (random movement)
- Orientation, X/Y/Z copied from existing nearby creature spawns is sufficient (critters wander anyway)
- No special NPC flags needed — the replacement system sets them

## Current State

- ✅ Database provisioning: all 51 pet battle tamers have spawns, quest links, and pet teams
- ✅ Quest chain: first-time progression + daily quests for all zones/factions
- ✅ Faction gating: Horde/Alliance duplicates for Classic dailies (33009-33013) and EK completion (33014)
- ✅ Wild pet battles: working
- ✅ Trainer pet battles: multi-pet rosters, pet death swaps, round flow all working
- ✅ Pet death handling: symmetric between player and trainer; swap prompt correctly appears when player's pet dies
- ✅ Trainer creature cleanup: unroots, un-pacifies, teleports to original position
- ✅ XP/achievement credit on player victory
- ✅ Weather effects: applied to all pets on both teams, persist through swaps
- ✅ Quest credit: `QUEST_OBJECTIVE_WINPETBATTLEAGAINSTNPC` objectives complete on trainer victory
- ⏳ Multi-pet wild battles: not implemented (TODO at `PetBattle.cpp:434` — `AddWildBattlePet()` only adds one pet)
- ⏳ Trainer AI: random ability selection (acceptable for 3 abilities)
