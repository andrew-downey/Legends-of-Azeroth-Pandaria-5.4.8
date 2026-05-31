-- ==============================================
--  Tillers of the Valley — Data Audit Script
--  Run against: world database (unless noted)
--  Purpose: Verify existing Tillers base data
-- ==============================================

SELECT '=== 1. TILLERS QUESTS ===' AS '';
-- All Tillers Union quest IDs
SELECT id, QuestType, QuestLevel, Flags, `LogTitle` FROM quest_template WHERE id IN
  (30252,30254,30255,30256,30257,30258,30259,30260,
   30516,30517,30518,30519,30521,30522,30523,30524,30525,30526,30527,30528,30529,
   31945,31946,31947,31948,31949,32682)
  ORDER BY id;

SELECT '=== 2. TILLERS NPCS ===' AS '';
-- All Tillers NPCs from the SAI dump + additional known NPCs
SELECT entry, name, subname, npcflag, type_flags, faction, `rank` FROM creature_template WHERE entry IN
  (58705,58706,58710,58729,58730,58731,58732,58733,58734,58735,  -- Tillers Union NPCs
   64593,64594,64595,64596,64597,64598,  -- Friend-rank NPCs
   58425,  -- Farmer Yoon (quest giver)
   58712,  -- Andi (gift daily)
   58704,  -- Gina Mudclaw
   58707,  -- Haohan Mudclaw
   58708,  -- Tina Mudclaw
   58709,  -- Ella
   58715,  -- Chee Chee
   58716,  -- Sho
   58717,  -- Old Hillpaw
   58718,  -- Fish Fellreed
   58719,  -- Farmer Fung
   58720)  -- Jogu the Drunk
  ORDER BY entry;

SELECT '=== 3a. TILLERS GAMEOBJECTS (Soil Patch) ===' AS '';
-- Confirmed soil patch: 186314 = "Loosely Turned Soil" (displayId 49)
-- NOTE: 16 soil patches are needed; only 1 GO entry confirmed in DB.
-- Additional patches must be spawned as phased copies of 186314.
SELECT gt.entry, gt.type, gt.displayId, gt.name,
       gs.ScriptName
FROM gameobject_template gt
LEFT JOIN gameobject_scripts gs ON gt.entry = gs.id
WHERE gt.entry IN (186314)
ORDER BY gt.entry;

SELECT '=== 3b. TILLERS CROP CREATURES (Spellclick) ===' AS '';
-- Tillers crops are CREATURES (not gameobjects) with spellclick mechanics.
-- Known crop creatures from farming quests:
--   58566 = Growing Green Cabbage
--   59987 = Parched EZ-Gro Green Cabbage
--   66129 = Ripe Striped Melon
SELECT ct.entry, ct.name, ct.subname, ct.type_flags, ct.faction, ct.`rank`,
       ct.HealthModifier, ct.DamageModifier
FROM creature_template ct
WHERE ct.entry IN (58566, 59987, 66129)
ORDER BY ct.entry;

SELECT '=== 3c. NPC SPELLCLICK SPELLS (Crop Interaction) ===' AS '';
-- Spellclick binds crop creatures to farming spells (e.g. spell 111108)
SELECT npc_spellclick_spells.*
FROM npc_spellclick_spells
WHERE npc_id IN (58566, 59987, 66129)
ORDER BY npc_id;

SELECT '=== 4. GIFT ITEMS & SCRIPT BINDINGS ===' AS '';
-- Check if gift items exist and have script bindings
SELECT i.entry, i.name, i.description, i.class, i.subclass, i.`maxcount`,
       isc.ScriptName
FROM item_template i
LEFT JOIN item_script_names isc ON i.entry = isc.Id
WHERE i.entry IN (79264, 79265, 79266, 79267, 79268)
ORDER BY i.entry;

SELECT '=== 5. FARM TOOL ITEMS ===' AS '';
-- Farm tools (NPCs 79104, 80513, 89880)
SELECT entry, name, description, class, subclass, `maxcount`
FROM item_template WHERE entry IN (79104, 80513, 89880)
ORDER BY entry;

SELECT '=== 5b. TILLERS SEED ITEMS ===' AS '';
SELECT entry, name, description, class, subclass, maxcount
FROM item_template WHERE entry IN (79102)
ORDER BY entry;

SELECT '=== 6. DISABLED FACTIONS ===' AS '';
-- Tillers NPC friendship factions (1273-1283)
SELECT sourceType, entry, flags FROM disables
WHERE sourceType=11 AND entry BETWEEN 1273 AND 1283
ORDER BY entry;

SELECT '=== 7. TILLERS FACTION DATA (1272-1283) ===' AS '';
-- Run against characters database
SELECT 'Run against: characters' AS '';
SELECT faction, `flags`, standing FROM character_reputation
WHERE faction BETWEEN 1272 AND 1283;

SELECT '=== 8. EXISTING SMART AI FOR TILLERS NPCS ===' AS '';
SELECT s.entryorguid, s.source_type, s.id, s.event_type, s.action_type,
       s.event_param1, s.event_param2, s.action_param1, s.action_param2,
       s.comment
FROM smart_scripts s
WHERE s.entryorguid IN
  (58705,58706,58710,58729,58730,58731,58732,58733,58734,58735,
   64593,64594,64595,64596,64597,64598,
   58425,58712,58704,58707,58708,58709,58715,58716,58717,58718,58719,58720)
  AND s.source_type=0
ORDER BY s.entryorguid, s.id;

SELECT '=== 9. POOL QUEST ENTRIES ===' AS '';
SELECT pool_entry, entry, description FROM pool_quest
WHERE entry IN
  (30252,30254,30255,30256,30257,30258,30259,30260,
   30516,30517,30518,30519,30521,30522,30523,30524,30525,30526,30527,30528,30529,
   31945,31946,31947,31948,31949,32682);

SELECT '=== 10. CONDITIONS TABLE (quest prerequisites) ===' AS '';
SELECT SourceTypeOrReferenceId, SourceGroup, SourceEntry, ConditionTypeOrReference, ConditionValue1, ConditionValue2, Comment
FROM conditions
WHERE SourceTypeOrReferenceId=19  -- CONDITION_SOURCE_TYPE_QUEST_SHOW_MARK
  AND SourceEntry IN
  (30252,30254,30255,30256,30257,30258,30259,30260,
   30516,30517,30518,30519,30521,30522,30523,30524,30525,30526,30527,30528,30529,
   31945,31946,31947,31948,31949,32682);

SELECT '=== 11. SPELL DATA FOR TILLERS SPELLS ===' AS '';
SELECT Id, `SpellName`, `Description`, `Effect1`, `EffectMiscValue1`, `EffectBasePoints1`
FROM spell_dbc WHERE Id IN
  (126987,  -- Tiller quest credit
    113244,  -- Old Hillpaw friendship
    125496,  -- Farm soil summon
    125516,  -- Plant crop
    125529,  -- Water crop
    125535,  -- Harvest crop
    125545,  -- Apply fertilizer
    125553,  -- Defect: Alluring
    125555,  -- Defect: Infested
    125556,  -- Defect: Parched
    125557,  -- Defect: Runty
    125558,  -- Defect: Smothered
    125559,  -- Defect: Tangled
    125560,  -- Defect: Wiggling
    125561,  -- Defect: Wild
    111108,  -- Spellclick spell (crop interaction)
    125848,  -- Jogu the Drunk crop prediction
    131280,  -- Halfhill Hearthstone
    125574,  -- Farm teleport
    127971)  -- Kelari Featherfoot roll course
ORDER BY Id;

SELECT '=== 12. SPELL SCRIPT NAMES (Farming) ===' AS '';
SELECT spell_id, ScriptName FROM spell_script_names
WHERE spell_id IN (125557, 126987, 111108, 125496, 125516, 125529, 125535, 125545)
ORDER BY spell_id;

SELECT '=== 13. SUNSONG RANCH NPC SPAWNS (map 870) ===' AS '';
-- Verify farm NPCs exist in spawn data
SELECT cg.guid, cg.id, ct.name, cg.position_x, cg.position_y, cg.position_z, cg.map, cg.phaseMask
FROM creature cg
JOIN creature_template ct ON cg.id = ct.entry
WHERE cg.map = 870
  AND cg.id IN (58646, 58719, 58721, 59536, 59599, 60072, 63062, 63064,
                64294, 64296, 64302, 64449, 64464, 65760, 65877, 66129,
                69098, 69142, 69550)
ORDER BY cg.id;

SELECT '=== AUDIT COMPLETE ===' AS '';
SELECT 'Review any sections with "0 rows" — those indicate MISSING data.' AS '';
