-- Tillers Farm: Fix 4-cabbage duplicate bug in the tutorial chain
-- Problem: Tilled Soil (59990) spawned Parched Cabbage (59987) during quest 30254,
--          then Farmer Yoon spawned 2 more on quest 30255 accept → 4 total.
--          Additionally, Yoon's 30255 cabbages were placed between plots.
-- Fix:
--   1. Add quest 30255 condition to Parched Cabbage spellclick (was HasItem only)
--   2. Add SAI: when 30254 is rewarded, despawn old Parched Cabbages (Set A)
--      Set B (from Farmer Yoon on 30255 accept) remains for player to water
--   3. Fix Parched Cabbage spawn coords from Farmer Yoon to match farm plots

-- 1. Add quest gating to Parched Cabbage (59987): require quest 30255 active
INSERT INTO conditions (SourceTypeOrReferenceId, SourceGroup, SourceEntry, SourceId, ElseGroup, ConditionTypeOrReference, ConditionTarget, ConditionValue1, ConditionValue2, ConditionValue3, NegativeCondition, ErrorType, ErrorTextId, ScriptName, Comment)
VALUES (18, 59987, 111108, 0, 0, 9, 0, 30255, 0, 0, 0, 0, 0, '', '59987 Spellclick Only Work With Quest 30255')
ON DUPLICATE KEY UPDATE ConditionTypeOrReference = 9, ConditionValue1 = 30255;

-- 2. Add SAI for 59987: On quest 30254 rewarded → force despawn self (cleans up Set A)
DELETE FROM smart_scripts WHERE entryorguid = 59987 AND source_type = 0 AND id = 3;

INSERT INTO smart_scripts (entryorguid, source_type, id, link, event_type, event_phase_mask, event_chance, event_flags, event_param1, event_param2, event_param3, event_param4, event_param5, action_type, action_param1, action_param2, action_param3, action_param4, action_param5, action_param6, target_type, target_param1, target_param2, target_param3, target_param4, target_x, target_y, target_z, target_o, comment)
VALUES (59987, 0, 3, 0, 21, 0, 100, 0, 30254, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Parched Cabbage - On Quest 30254 Rewarded Despawn Self');

-- 3. Fix Farmer Yoon Parched Cabbage spawn coords to match farm plot positions
UPDATE smart_scripts
SET target_x = '-164', target_y = '638', target_z = '165.4'
WHERE entryorguid = 58646 AND source_type = 0 AND id = 5;

UPDATE smart_scripts
SET target_x = '-164.5', target_y = '642', target_z = '165.4'
WHERE entryorguid = 58646 AND source_type = 0 AND id = 6;
