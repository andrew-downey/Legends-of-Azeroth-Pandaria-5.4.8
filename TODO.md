## Pet Battles
### Round Flow / Timing
- [x] **Pet death swap turn order**: After a pet is defeated and the player selects a replacement via `CMSG_PET_BATTLE_SET_FRONT_PET` → `SWAP_DEAD_PET`, the enemy pet immediately attacks in the same round as the swap. The swap should happen "before" the next round begins — the new pet should enter and the next round should proceed with normal speed-based turn order. The death-forced swap should NOT consume the player's turn.
- [x] **Voluntary swap should consume a turn**: When the player voluntarily swaps pets (alive → alive via `PET_BATTLE_MOVE_TYPE_SWAP_OR_PASS`), the swap should consume the player's turn for the round. The new pet comes in but does NOT attack that round. Currently, `HandleRound()` processes the swap via `SwapActivePet()` and then both teams execute `DoCasts()` — the new pet attacks in the same round, making swaps free action instead of turn-costing.
- [x] **XP/achievements granted before final round finishes**: In `EndBattle()`, XP calculation (lines 502-526), achievement criteria updates (lines 552-565), and quest credit (lines 568-576) are processed BEFORE `SendFinalRound()` sends the final round display to the client. This causes achievement popups and XP gains to appear before the last round's animation has finished playing, spoiling the outcome of captures and battle victories. Fix: defer reward processing until client acknowledges the final round.
- [x] **Delayed activation abilities not executing second hit** (e.g., Elementium Bolt): The ability is cast on turn 1 with correct animation and aura applied — aura (debuff with cooldown timer) shows on the enemy. When the timer reaches zero (expected delayed activation), the debuff is removed but NO damage or second animation plays. The expected behavior is: when the timer expires, the delayed hit should execute (damage + animation), then the debuff is removed. Currently, `BattlePetAura::Process()` handles the timed expiry but the `PET_BATTLE_ABILITY_PROC_ON_AURA_REMOVED` proc path in `BattlePetAura::Expire()` may not correctly trigger the ability's damage effects. Additionally, the `ActiveAbility` multi-turn mechanism (`DoCasts()` calling the ability with `TurnsPassed`) may also need investigation — the second call might not be reaching the client as a visible effect.
### Phase 0 — Weather slot revert (immediate)
- [x] Revert SendInitialUpdate StateCount = 0 for all slots
- [x] Clean up enviro data section entry (revert crash)
### Phase 1 — Family passive fixes (corrected for MoP)
- [x] **Elemental passive**: Corrected — only "ignore all negative weather effects" (no +50% damage in MoP)
- [x] **Humanoid passive**: 5% heal (was 4%)
- [x] **Mechanical passive**: Revive at 25% HP (was 20%)
- [x] **All other 7 passives**: Values confirmed correct vs warcraft.wiki.gg
### Phase 2 — New effect handlers (port from LegionCore dispatch table ✅)
- [x] **Effect 61** — HandleHealCasterPercentNotState: heal caster %maxHP with state condition checks
- [x] **Effects 75/76/77** — HandleDamageToggleAura: damage + toggle aura (76 existed; 75/77 were HandleNull)
- [ ] Effect 84: HandleWeatherDamage — deals damage scaling with weather state value
- [ ] Effect 87: HandleAuraCondAccuracyState — conditional aura accuracy
- [ ] Effect 170: HandleWeatherDamage (LegionCore effect 170 = our 84? need to verify DBC)
- [ ] Effect 172: HandleAuraCondAccuracyState (ditto)
- [ ] Effect 171: HandleHealOnSpecificWeather — heal only when specific weather active
- [ ] Effect 156/157/177: HandleCheckState — conditional chain continuance
- [ ] Effect 111: HandleResurect — revive a dead pet
- [ ] Effect 139: HandleSwapLow — swap to lowest HP pet
- [ ] Effect 145: HandleSwapHigh — swap to highest HP pet
- [ ] Effect 165: HandleTryRevive — attempt to revive
- [ ] Effect 128: HandleSetHealthPercent — set health to % of max
- [ ] Effect 129: HandleLockActiveAbility — lock ability slot
- [ ] Fill remaining gaps in 235-slot dispatch table (~40+ missing handlers)
### Phase 3 — Damage formula upgrade
- [ ] Power scaling: +5% per point of caster power
- [ ] DB2 type effectiveness table (BattlePetTypeDamageMod)
- [ ] Flat damage modifiers (Add_FlatDamageDealt, Add_FlatDamageTaken)
- [ ] +/-5% damage variance
### Phase 4 — 3-phase turn structure (Thrash proc chain fix)
- [ ] Restructure HandleRound: StartOfRound → SpeedOrderedAbility → EndOfRound
- [ ] Add PETBATTLE_ABILITY_TURN0_PROC_ON_ROUND_START/END proc triggers

### Abilities / Combat Bugs
- [x] **Thrash attacking too many times**: Fixed. HandleMultiStrike() had an unconditional base Damage() call + two independent 50% rolls.
- [x] **Mechanical racial passive**: Fixed in SetHealth() — revives with 33% HP once per battle.
- [x] **Undead racial passive**: Fixed in SetHealth() + HandleRound() — survives lethal damage for 1 round via UNKILLABLE state, dies at round end.
- [x] **Unimplemented family passives**: All 10 family passives now implemented (Magic 35% cap, Aquatic DoT -50%, Elemental weather immunity, Flying +50% speed, Humanoid 5% heal, Dragonkin +50%, Beast +25%, Critter immunity, Mechanical 25% revive, Undead unkillable).
- [x] **Elemental family passive**: Elementals ignore weather effects. ✅ Weather applied to all 6 pets at cast time (no swap-in animation). Elemental pets skipped. +50% damage when weather active added to CalculateDamage. (0,0) environment slot visualization still pending retail sniff.
- [ ] **On-damage-dealt/taken procs**: `PETBATTLE_ABILITY_TURN0_PROC_ON_DAMAGE_DEALT` (procType 5) and `ON_DAMAGE_TAKEN` (procType 6) never fire after `Damage()`. Missing ability triggers.
- [ ] **Nearby wild pets don't join**: `PetBattle.cpp:424` — placeholder exists, retail allows nearby spawns to join wild encounters.
- [ ] **Aura expire turn hardcoded**: `BattlePetAura.cpp:137` — `effect.SetTurn(1, 1)` should reflect the actual expire turn.
- [ ] **HandleNegativeAura stubs**: `BattlePetAbilityEffect.cpp:605` — ChainFailure, CasterState, TargetState params unread.
- [ ] **PvP pet battles broken**: Opponent never receives finalize packet (`PetBattle.cpp:1129`). Turn timer also unimplemented.
- [ ] **PvP pet battles broken**: Opponent never receives finalize packet (`PetBattle.cpp:1129`). Turn timer also unimplemented.
- [ ] **GetInputStatusFlags incomplete**: `PetBattle.cpp:251` — missing CC checks (stun, web, etc.) for client UI lock state.
- [ ] **Ability queue persists on pet swap/death**: When a pet dies or is swapped, the queued ability from the old pet can be used by the swapped-in pet. The active attack must be cleared before swapping.
- [ ] **Resilient state not implemented**: `BATTLE_PET_STATE_RESILITANT` (149) initialized to 0 in `InitialiseStates()` but never checked or set at runtime. CC effects like Frog Kiss can be applied back-to-back without immunity.

### Wild Battle Pet Spawns (SQL)
- [x] **Cataclysm zones**: Deepholm (2026_06_12_00), Twilight Highlands (2026_06_12_01), Mount Hyjal (2026_06_12_02), Uldum (2026_06_12_03)
- [x] **Northrend zones** (Part 1): Borean Tundra + Howling Fjord + Dragonblight (2026_06_12_04)
- [x] **Northrend zones** (Part 2): Grizzly Hills + Sholazar + Zul'Drak + Crystalsong + Icecrown + Storm Peaks (2026_06_12_05)
- [ ] **Remaining Kalimdor zones**: Feralas, Desolace, Stonetalon, Ashenvale, Darkshore, Silithus, Winterspring
- [ ] **Remaining Eastern Kingdoms zones**: Arathi, Badlands, Burning Steppes, EPL, Hinterlands, Searing Gorge, Swamp of Sorrows, Western Plaguelands
- [ ] **Outland zones**: Hellfire Peninsula, Zangarmarsh, Terokkar, Nagrand, Blade's Edge, Netherstorm, Shadowmoon

## Playerbots

## Pandaria
### Tillers
[ ] Player is given the "Parched" debuff when clicking on untilled soil during quest "Learn and grow II", the soil never changes to tilled soil.
[ ] Noted that the tilling animation from the item Dented Shovel, is the correct animation to use when smashing rocks for "quest "A Helping Hand" and for use on untilled soil.
See TILLERS-TODO.md

## Smart Script Fixes

### Non-trivial (needs research before fixing)
- [ ] **Missing creature_text**: 76+ `smart_scripts` entries reference text group IDs that don't exist in `creature_text` (entries: 1268, 8400, 8719, 6119, 25730, 60572, 17253, 16514, 46425, 35231, 45152, 46134, 46276, 46402, 48012, 54615, 54924, 54944, 55488, 57760, 59296, 59392, 66693, 69267, 69305, 30284, 30474, 25307, 24786, 24198, 23669, 19354, 18938, 15526, 15324, 14860, 13601, 9457, 2719, 4345, 7955, 7840, 7810, 7710, 7079, etc). Need to determine correct text or remove entries.
- [ ] **Non-existent creature entry 27754 + GO 96036**: 5 `smart_scripts` rows reference templates that don't exist. Investigate whether these were deleted intentionally or need recreating.
- [ ] **Missing spells in DBC**: 14,658 `action_type=11` entries have spells not in `spell_dbc` (479 rows vs ~203K in Spell.dbc). Need to verify if the runtime DBC file (`/home/andrew/warcraft-server/bin/Data/dbc/Spell.dbc`) actually has these spells. If yes, the DBC→SQL import tool is incomplete; if no, these are extinct Vanilla/TBC/Cata spells and the SAI entries should be cleaned up.
- [ ] **Invalid action types** (175 rows): Types 0, 134, 201–208 are invalid in this core version. These likely came from a TrinityCore/MaNGOS import and need manual review to determine correct action mappings.
- [ ] **Redundant kill credit / summon warnings**: ~180 "has already spell kill credit" + ~120 "There is a summon spell" messages. The SAI actions still execute but are redundant since a spell effect already handles this. Could either clean up the SAI entries or suppress the log level in `SmartScriptMgr.cpp`.
- [ ] **Unhandled action type 124**: Entry 170600 (SourceType 9, timed action list). Needs code implementation or DB clean-up.
  - Also: WaypointPath 1706 doesn't exist for entry 170600.

## Core
- [x] **Account-wide achievement criteria progress reset on login**: `LoadGlobalAccountData()` (which loads `account_achievement_progress` into `AccountAchievementMgr`) was defined but never called during login. Added call in `Player::LoadFromDB()` before `PlayerAchievementMgr::LoadFromDB()`. Also fixed `CHAR_SEL_ACCOUNT_DATA` registration from `CONNECTION_ASYNC` to `CONNECTION_BOTH` so the synchronous call succeeds.
