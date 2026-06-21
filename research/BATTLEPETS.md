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
| 18 | `PetBattle.cpp` | `StartBattle()`: added `PET_BATTLE_EFFECT_ACTIVE_PET` for initial active pets — fixes bench pets appearing in 3D cages on the battlefield |

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

## Ability Effect System

### Handler Table Architecture

The handler table in `BattlePetAbilityEffect.cpp:29-236` is a fixed-size array indexed by `EffectProperty` (from `BattlePetAbilityEffect.db2`). Size: `PET_BATTLE_TOTAL_ABILITY_EFFECTS` (now 256, was 198).

Each entry: `{ HandlerFunction, TargetType }`.

**Effect flow per `Execute()` call:**
1. Resolve target list from `TargetType` (CASTER, TARGET, ALL, HEAD, etc.)
2. For each target: save flags, apply miss if untargetable, call handler, restore flags
3. Effects are independent — flags do NOT carry across effects in the same ability

**ChainFailure** (`m_chainFailure`): Controlled by `Properties[0]` on some effect types. If set, a miss/immune on a chainFailure effect skips subsequent effects. Set per `BattlePetAbilityEffect` object — not by the handler directly.

### DBC Field Mapping

`BattlePetAbilityEffectEntry` (12 uint32 fields from `BattlePetAbilityEffect.db2`):

| Index | Field | Meaning |
|-------|-------|---------|
| 0 | Id | Primary key |
| 1 | TurnId | FK → `BattlePetAbilityTurn.Id` |
| 2 | TriggerAbility | FK → ability to trigger (often 0) |
| 3 | Unk | **Critical**: should be ability for `BattlePetAbilityState` lookups, but code uses `TriggerAbility` instead |
| 4 | EffectProperty | Effect type (handler table index) |
| 5 | EffectPropertyBit | Bit position within the turn (execution order) |
| 6-11 | Properties[6] | Effect-specific parameters (see below) |

### Property Encoding Convention

For almost all effect types, `Properties` follow positional semantics:

| Index | Common Meaning | Notes |
|-------|---------------|-------|
| 0 | Base value (damage, heal, state ID, pct) | Primary parameter |
| 1 | Accuracy/chance | Miss rolls: `CalculateHit(acc)` |
| 2 | Duration / state ID / IsPeriodic flag | Dependent on effect type |
| 3 | State ID / multiplier / MaxAllowed | Second state or count |
| 4 | Effect-specific | |
| 5 | `reportFailAsImmune` / chainFailure | In `HandlePowerlessAura` |

### Handler Status (2026-06-11 Audit)

Out of 78 unique effect types used in the DBC:

| Status | Count | Details |
|--------|-------|---------|
| **Implemented** | 17 | Original handlers (Heal, Damage, PositiveAura, etc.) |
| **Re-mapped** | 14 | Effects that reused existing handlers (22, 28, 52, 79, 85, 139, 145, 150, 165, 168, 169, 178, 179, 204) |
| **New** | 15 | Handlers added in 2026-06-11 session |
| **Unhandled** | ~32 | Remain as `HandleNull` — see deferred list below |
| Total used | 78 | From `BattlePetAbilityEffect.db2` |

**Implemented effects** (46 total, up from 17):

| Effect | Handler | Abilities | Purpose |
|--------|---------|-----------|---------|
| 22 | NegativeAura | 2 | Target debuff |
| 23 | Heal | many | Flat heal + power scaling |
| 24 | Damage | many | Flat damage + family bonus |
| 25 | Catch | 1 | Wild pet capture |
| 26 | PositiveAura | many | Self-buff aura |
| 27 | RampingDamage | 3 | Scaling damage per use |
| 28 | PositiveAura | 1 | Self-buff aura |
| 29 | StateBonusDamage | 17 | Extra damage if caster/target has state |
| 31 | SetState | 3 | Set caster state to value |
| 32 | HealPctDealt | 8 | Heal % of last damage dealt |
| 33 | HealPct | 0 (unused) | Heal % of max health |
| 44 | HealWithLastHit | 3 | Heal % of last damage dealt |
| 50 | NegativeAura | many | Target debuff aura |
| 52 | NegativeAura | 28 | Debuff aura (stun/sleep) |
| 53 | HealPct | 7 | Heal % of max health |
| 54 | PeriodicTrigger | many | Periodic DoT/HoT |
| 59 | LowHpDamage | 3 | Bonus when caster low HP |
| 62 | PctHealthDamage | 10 | % of target max HP |
| 63 | PeriodicPositiveTrigger | many | Periodic HoT/buff |
| 66 | ExecuteDamage | 3 | Bonus when target low HP |
| 68 | Sacrifice | 6 | Kill caster, damage target |
| 76 | DamageToggleAura | 2 | Toggle on/off damage |
| 79 | Damage | 1 | Damage with accuracy |
| 80 | WeatherAura | 7 | Weather effect (all pets) |
| 85 | PositiveAura | 1 | Self-buff aura |
| 96 | DamageHitState | 2 | Damage conditional on states |
| 100 | HealSplit | 4 | Heal with power scaling |
| 103 | ExtraAttackFirst | 4 | Extra hit if faster |
| 104 | HealState | 7 | Heal conditional on states |
| 131 | Interrupt | 7 | Turn lock + optional stun |
| 135 | KillActive | 12 | Damage both active pets |
| 136 | Cleanse | 18 | Remove harmful auras |
| 139 | NegativeAura | 2 | Target debuff aura |
| 145 | NegativeAura | 1 | Target debuff aura |
| 149 | DamageNonLethal | 2 | Damage (min 1 HP remaining) |
| 150 | PositiveAura | 1 | Self HoT |
| 160 | ExtraAttackIfSlower | 1 | Extra hit if slower → **fixes Tail Sweep** |
| 164 | MultiStrike | 19 | 1-3 hits with chance → **fixes Rend** |
| 165 | NegativeAura | 2 | Target debuff aura |
| 168 | NegativeAura | 1 | Target debuff aura |
| 169 | WeatherAura | 1 | Team-wide weather |
| 177 | Stun | 22 | Immunity gate only (see Stun Architecture) |
| 178 | PowerlessAura | 17 | Stun/sleep with immunity check |
| 179 | PositiveAura | 1 | Self-buff |
| 197 | Vengeance | 1 | Reflect damage taken |
| 204 | PositiveAura | 1 | Reflective Shield self-buff |

### New Handler Implementations (2026-06-11)

**`HandleHealPct`** (Effects 33, 53): `Heal(target, CalculatePct(target->GetMaxHealth(), Properties[0]))` — flat % of max health, no power scaling. Recovery heals 4% per round.

**`HandleHealWithLastHit`** (Effect 44): `Heal(target, CalculatePct(caster->LastHitDealt, Properties[0]))` — used by Healing Flame (50% of last hit).

**`HandleStateBonusDamage`** (Effect 29): `Damage = base[0]`. If `Properties[2]` > 0 and `caster->States[Properties[2]]` > 0, add bonus base. Else if `Properties[3]` > 0 and `target->States[Properties[3]]` > 0, add bonus base. Used by Counterstrike (state 28 = WAS_DAMAGED_THIS_ROUND), Ice Lance (state 52 = CHILLED), etc.

**`HandleLowHpDamage`** (Effect 59): If `Properties[2]` > 0 and caster HP% < threshold, deals 2× damage. Comeback: no threshold (always 1×). Early Advantage: threshold 10%.

**`HandlePctHealthDamage`** (Effect 62): `Damage = target MaxHP × Properties[0] / 100`. Not scaled by power. Used by Corpse Explosion (5%), Trample (10%), etc.

**`HandleExecuteDamage`** (Effect 66): Like LowHpDamage but checks target HP%. Properties[2] = 100 for all current abilities (always active).

**`HandleSacrifice`** (Effect 68): `SetHealth(caster, 0)` then `Damage(target, Properties[0])`. Kills caster, deals damage to target.

**`HandleMultiStrike`** (Effect 164): Always deals 1 hit of `Properties[0]` damage, then rolls `Properties[1]`% chance for a 2nd hit, then rolls again for a 3rd. No accuracy check (the primary ability effect handles hit/miss). Used by Rend (50% chance), Triple Snap (66%), Slicing Wind (33%).

**`HandleInterrupt`** (Effect 131): Always sets `BATTLE_PET_STATE_TURN_LOCK` (35). If `Properties[0]` > 0, also sets `BATTLE_PET_STATE_MECHANIC_STUNNED` (22). Kick uses 0 (interrupt only), Horn Attack uses 1 (interrupt + stun).

**`HandleVengeance`** (Effect 197): `Damage = caster->LastHitTaken × Properties[0] / 100`. Vengeance (Darkmoon Zeppelin): 100% of damage taken.

### Stun/Sleep Architecture

Two separate effects work together for CC:

**Effect 177 (`HandleStun`)**: Immunity gate only. Checks `target->States[Properties[0]]` (always 149 = RESILITANT, the Humanoid racial passive). If > 0, sets `IMMUNE` flag and returns. Does NOT apply the stun itself.

**Effect 178 (`HandlePowerlessAura`)**: Applies the actual stun/sleep via an aura. Checks if target already has the state (Properties[3] = 149), reports immune if so. Otherwise applies aura for `Properties[2]` turns. The aura should set `BATTLE_PET_STATE_MECHANIC_STUNNED` (22) via `BattlePetAbilityState.db2` entries — but see the BattlePetAbilityState bug below.

**Effect 52 (`HandleNegativeAura`)**: Secondary stun mechanism. Properties[1] is the chance (e.g., 25 for Headbutt's 25% stun). Applies a debuff aura for Properties[2] turns. The 25% chance is implemented as `CalculateHit(25)` within the handler.

**Execution order**: Determined by `EffectPropertyBit` — typically 177 (bit 2) fires before 52 (bit 3).

### Critical Bug: BattlePetAbilityState Unk Field

`BattlePetAbilityEffect.db2` field[3] (called `Unk`) contains the ability ID for `BattlePetAbilityState.db2` lookups. However, `BattlePetAura::OnApply()` uses `m_ability` (which is set from `TriggerAbility` = field[2] = often 0) instead of the Unk field.

This means:
- **Effect 49** (47 abilities): Intended to be a "state aura" that applies `BattlePetAbilityState` modifiers. Since `TriggerAbility` is 0 and Unk is correct (e.g., 927 for Headbutt's stun, 340 for Burrow), **no states are applied**. Burrow, Lift-Off, Meteor Strike, and other delayed-untargetable abilities don't work.
- **Effect 52/178 stun auras**: The aura is created but its `OnApply()` never matches `BattlePetAbilityState` entries (which use the Unk ability ID, not TriggerAbility). **Stun/sleep from auras does not actually apply `BATTLE_PET_STATE_MECHANIC_STUNNED` (state 22).**

**Fix needed**: In `PetBattle::AddAura()`, pass the Unk field (or a third ability parameter) to `BattlePetAura` so `OnApply()` can query the correct `BattlePetAbilityState` entries. Alternatively, pass `m_effectEntry->Unk` as the `ability` parameter to `AddAura` for effect types 49, 52, 178.

### State Reference (Key IDs for Handler Logic)

| ID | Constant | Type | Used By |
|----|----------|------|---------|
| 1 | IS_DEAD | Bool | Death tracking |
| 21 | MECHANIC_POISONED | Bool | Poison DoT |
| 22 | MECHANIC_STUNNED | Bool | Stun/sleep — set by BattlePetAbilityState |
| 28 | CONDITION_WAS_DAMAGED_THIS_ROUND | Bool | Counterstrike conditional |
| 29 | UNTARGETABLE | Bool | Burrow, Lift-Off |
| 31 | LAST_HIT_TAKEN | Int | Vengeance damage reflection |
| 32 | LAST_HIT_DEALT | Int | HealWithLastHit, HealPctDealt |
| 34 | MECHANIC_BURNING | Bool | Conflagrate conditional |
| 35 | TURN_LOCK | Bool | Interrupt effect |
| 52 | MECHANIC_CHILLED | Bool | Ice Lance conditional |
| 64 | MECHANIC_WEBBED | Bool | CanAttack check |
| 77 | MECHANIC_BLEEDING | Bool | Maul conditional |
| 82 | MECHANIC_BLIND | Bool | Light conditional |
| 149 | RESILITANT | Bool | Humanoid racial — immunity to stun/sleep |

`BATTLE_PET_MAX_STATES = 163` — state IDs beyond named constants exist in `BattlePetAbilityState.db2` (e.g., states 91, 100, 200) but are outside the `enum`.

### Effect 49 (StateAura) — Deferred

47 abilities use Effect 49 including Burrow, Lift-Off, Asleep, Meteor Strike, Supercharged. Properties[0] is 100 or 200 (turn delay?), Properties[1] is always 0. Not safe to remap to `HandleSetState` because:
- Properties[0] values (100, 200) are not valid state IDs (BATTLE_PET_MAX_STATES = 163)
- The actual state application requires `BattlePetAbilityState` lookups which the aura system doesn't do correctly (see Critical Bug above)

Needs a dedicated `HandleStateAura` that creates an aura whose `OnApply()` queries `BattlePetAbilityState` using the Unk field.

### Other Deferred Effects (~32 remain as HandleNull)

| Effects | Count | Description |
|---------|-------|-------------|
| 30, 43, 45 | 3 | Chomp-style sequential triggers (Trufflesnuffle ability) |
| 55, 56, 57, 78, 111, 121, 122, 123, 124, 125 | 10 | Pet swap/swap-to-back/swap-target mechanics |
| 58 | 1 | Repeat last ability |
| 61 | 2 | Heal % of max health (distinct from HealPct) |
| 65, 67 | 2 | Channeled attacks (Dreadful Breath, Incendiary Breath) |
| 72, 73, 74, 75, 99 | 5 | Multi-damage per state (Mana Surge, Water Jet, Geyser) |
| 77, 137, 138 | 3 | Shield/absorb/bubble mechanics |
| 86, 142, 143, 144 | 4 | Sonic barrier, reflective shield, immortal fortitude |
| 97, 107, 108, 112, 116, 117, 128, 129, 132, 133, 134, 140, 141, 147, 156, 157, 158, 159, 170, 171, 172, 194 | ~22 | Niche/one-off mechanics (wonderpets, watchers, swarm, decoy, overcharge, weather variants) |

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
- ✅ Bench pet 3D cages: fixed — `StartBattle()` now sends `PET_BATTLE_EFFECT_ACTIVE_PET` so client doesn't render non-active pets with cage models
- ⏳ Multi-pet wild battles: not implemented (TODO at `PetBattle.cpp:434` — `AddWildBattlePet()` only adds one pet)
- ⏳ Trainer AI: random ability selection (acceptable for 3 abilities)
