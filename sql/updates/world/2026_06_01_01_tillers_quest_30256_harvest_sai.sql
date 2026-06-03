-- Tillers Farm: Configure 59833 (Ripe Cabbage) and 58719 (Unbudding Rock) for quest completion
-- 59833: spellclick → give item 80314 (EZ-Gro Green Cabbage) to player → despawn after 1s
-- 58719: spellclick → existing SAI handles kill credit + despawn

-- 1. Update 59833 template for spellclick + SmartAI
UPDATE creature_template SET npcflag = 16777216, AIName = 'SmartAI' WHERE entry = 59833;

-- 2. Register spellclick for 59833 (clicker casts spell 111108)
INSERT INTO npc_spellclick_spells (npc_entry, spell_id, cast_flags, user_type)
VALUES (59833, 111108, 1, 0)
ON DUPLICATE KEY UPDATE cast_flags = 1, user_type = 0;

-- 3. Register spellclick for 58719 (clicker casts spell 111108)
INSERT INTO npc_spellclick_spells (npc_entry, spell_id, cast_flags, user_type)
VALUES (58719, 111108, 1, 0)
ON DUPLICATE KEY UPDATE cast_flags = 1, user_type = 0;

-- 4. Condition: 59833 spellclick only works while quest 30256 is active
INSERT INTO conditions (SourceTypeOrReferenceId, SourceGroup, SourceEntry, SourceId, ElseGroup, ConditionTypeOrReference, ConditionTarget, ConditionValue1, ConditionValue2, ConditionValue3, NegativeCondition, ErrorType, ErrorTextId, ScriptName, Comment)
VALUES (18, 59833, 111108, 0, 0, 9, 0, 30256, 0, 0, 0, 0, 0, '', '59833 Spellclick Only Work With Quest 30256')
ON DUPLICATE KEY UPDATE ConditionTypeOrReference = 9, ConditionValue1 = 30256;

-- 5. Condition: 58719 spellclick only works while quest 30252 is active
INSERT INTO conditions (SourceTypeOrReferenceId, SourceGroup, SourceEntry, SourceId, ElseGroup, ConditionTypeOrReference, ConditionTarget, ConditionValue1, ConditionValue2, ConditionValue3, NegativeCondition, ErrorType, ErrorTextId, ScriptName, Comment)
VALUES (18, 58719, 111108, 0, 0, 9, 0, 30252, 0, 0, 0, 0, 0, '', '58719 Spellclick Only Work With Quest 30252')
ON DUPLICATE KEY UPDATE ConditionTypeOrReference = 9, ConditionValue1 = 30252;

-- 6. SAI for 59833: On spellclick → give item 80314 to player → despawn after 1s
DELETE FROM smart_scripts WHERE entryorguid = 59833 AND source_type = 0;

INSERT INTO smart_scripts (entryorguid, source_type, id, link, event_type, event_phase_mask, event_chance, event_flags, event_param1, event_param2, event_param3, event_param4, event_param5, action_type, action_param1, action_param2, action_param3, action_param4, action_param5, action_param6, target_type, target_param1, target_param2, target_param3, target_param4, target_x, target_y, target_z, target_o, comment)
VALUES 
(59833, 0, 0, 0, 73, 0, 100, 1, 0, 0, 0, 0, 0, 56, 80314, 1, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 'Ripe EZ-Gro Green Cabbage - On SpellClick Give Item To Player'),
(59833, 0, 1, 0, 73, 0, 100, 1, 0, 0, 0, 0, 0, 41, 1000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Ripe EZ-Gro Green Cabbage - On SpellClick Despawn After 1s');
