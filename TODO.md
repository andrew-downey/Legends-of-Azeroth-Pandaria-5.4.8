## Pet Battles
### Round Flow / Timing
- [x] **Pet death swap turn order**: After a pet is defeated and the player selects a replacement via `CMSG_PET_BATTLE_SET_FRONT_PET` → `SWAP_DEAD_PET`, the enemy pet immediately attacks in the same round as the swap. The swap should happen "before" the next round begins — the new pet should enter and the next round should proceed with normal speed-based turn order. The death-forced swap should NOT consume the player's turn.
- [x] **Voluntary swap should consume a turn**: When the player voluntarily swaps pets (alive → alive via `PET_BATTLE_MOVE_TYPE_SWAP_OR_PASS`), the swap should consume the player's turn for the round. The new pet comes in but does NOT attack that round. Currently, `HandleRound()` processes the swap via `SwapActivePet()` and then both teams execute `DoCasts()` — the new pet attacks in the same round, making swaps free action instead of turn-costing.
- [x] **XP/achievements granted before final round finishes**: In `EndBattle()`, XP calculation (lines 502-526), achievement criteria updates (lines 552-565), and quest credit (lines 568-576) are processed BEFORE `SendFinalRound()` sends the final round display to the client. This causes achievement popups and XP gains to appear before the last round's animation has finished playing, spoiling the outcome of captures and battle victories. Fix: defer reward processing until client acknowledges the final round.
- [x] **Delayed activation abilities not executing second hit** (e.g., Elementium Bolt): The ability is cast on turn 1 with correct animation and aura applied — aura (debuff with cooldown timer) shows on the enemy. When the timer reaches zero (expected delayed activation), the debuff is removed but NO damage or second animation plays. The expected behavior is: when the timer expires, the delayed hit should execute (damage + animation), then the debuff is removed. Currently, `BattlePetAura::Process()` handles the timed expiry but the `PET_BATTLE_ABILITY_PROC_ON_AURA_REMOVED` proc path in `BattlePetAura::Expire()` may not correctly trigger the ability's damage effects. Additionally, the `ActiveAbility` multi-turn mechanism (`DoCasts()` calling the ability with `TurnsPassed`) may also need investigation — the second call might not be reaching the client as a visible effect.
- [ ] **Elementium bolt not casting if enemy is currently unattackable, like with Burrow** This ability is CAST on turn 1, but only comes into effect when the debuff expires 3 turns later. The hit chance (and enemy avoidance effects) should only come into effect when the attack triggers.
### Other
- [ ] **Cocoon shield ability cooldown**: Attacks like the cocoon one that protects the caster should immediately become available (no cooldown) after use.

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
