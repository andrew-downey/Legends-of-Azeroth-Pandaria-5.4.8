## Pet Battles
1. Attacks like the cocoon one that protects the caster, should immediately become available.
2. Attacks like Elementium Bolt are missing their animations when they proc (multi-turn / delayed attacks)

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
