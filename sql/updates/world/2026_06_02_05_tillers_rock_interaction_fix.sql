-- Fix Unbudging Rock (58719) interaction: switch from gossip to spellclick
-- Replaces GOSSIP_HELLO → CLOSE_GOSSIP with ON_SPELLCLICK → CALL_KILLEDMONSTER
-- Uses existing npc_spellclick_spells entry (spell 111108 - Rusty Watering Can)
-- Changes:
--   npcflag 1 (gossip) → 16777216 (spellclick)
--   SAI: event 64→73, action 72→33, FORCE_DESPAWN uses 1ms delay to avoid death-state
--   Hides pre-farm static Tillers NPCs (they should appear after the quest dynamically)

-- 1. Switch rock from gossip to spellclick
UPDATE creature_template SET npcflag = 16777216 WHERE entry = 58719;

-- 2. Replace SAI with spellclick chain
DELETE FROM smart_scripts WHERE entryorguid = 58719 AND source_type = 0;
INSERT INTO smart_scripts (entryorguid, source_type, id, link, event_type, event_phase_mask, event_chance, event_flags, event_param1, event_param2, event_param3, event_param4, event_param5, action_type, action_param1, action_param2, action_param3, action_param4, action_param5, action_param6, target_type, target_param1, target_param2, target_param3, target_param4, target_x, target_y, target_z, target_o, comment) VALUES
(58719, 0, 0, 1, 73, 0, 100, 0, 0, 0, 0, 0, 0, 33, 58719, 1, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 'Unbudging Rock - On SpellClick - Kill Credit 58719'),
(58719, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 41, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Unbudging Rock - Linked - Force Despawn (1ms)');

-- 3. Hide pre-farm static NPCs at the farm (they appear dynamically after A Helping Hand)
-- Haohan Mudclaw (57402) guid 514107, Farmer Fung (57298) guid 514153,
-- Fish Fellreed (58705) guid 514085, Gina Mudclaw (58706) guid 514083,
-- Jogu the Drunk (58710) guid 514088
UPDATE creature SET phaseMask = 4294901760 WHERE guid IN (514153, 514107, 514085, 514083, 514088);

-- ============================================================
-- 4. Fix quest 30254 (Learn and Grow II: Tilling and Planting)
--    Replace auto-complete (CALL_KILLEDMONSTER) with interactive soil/seed system
-- ============================================================

-- 4a. Replace 58646 SAI events 3-4: summon soil creatures instead of auto-credit
DELETE FROM smart_scripts WHERE entryorguid = 58646 AND source_type = 0 AND id IN (3, 4);
INSERT INTO smart_scripts (entryorguid, source_type, id, link, event_type, event_phase_mask, event_chance, event_flags, event_param1, event_param2, event_param3, event_param4, event_param5, action_type, action_param1, action_param2, action_param3, action_param4, action_param5, action_param6, target_type, target_param1, target_param2, target_param3, target_param4, target_x, target_y, target_z, target_o, comment) VALUES
(58646, 0, 3, 0, 19, 0, 100, 0, 30254, 0, 0, 0, 0, 12, 59985, 7, 0, 0, 0, 0, 8, 0, 0, 0, 0, -164.0, 638.0, 165.4, 0, 'Farmer Yoon - On Quest Accept Summon Untilled Soil 1'),
(58646, 0, 4, 0, 19, 0, 100, 0, 30254, 0, 0, 0, 0, 12, 59985, 7, 0, 0, 0, 0, 8, 0, 0, 0, 0, -164.5, 642.0, 165.4, 0, 'Farmer Yoon - On Quest Accept Summon Untilled Soil 2');

-- 4b. Set up 59985 (Untilled Soil) - spellclick to transform into tilled soil
UPDATE creature_template SET npcflag = 16777216, AIName = 'SmartAI', type_flags = 1048576 WHERE entry = 59985;

DELETE FROM npc_spellclick_spells WHERE npc_entry = 59985;
INSERT INTO npc_spellclick_spells (npc_entry, spell_id, cast_flags, user_type) VALUES
(59985, 111108, 1, 0);

-- Conditions: player must be on quest 30254
DELETE FROM conditions WHERE SourceTypeOrReferenceId = 18 AND SourceGroup = 59985;
INSERT INTO conditions (SourceTypeOrReferenceId, SourceGroup, SourceEntry, SourceId, ElseGroup, ConditionTypeOrReference, ConditionTarget, ConditionValue1, ConditionValue2, ConditionValue3, NegativeCondition, ErrorType, ErrorTextId, ScriptName, Comment) VALUES
(18, 59985, 111108, 0, 0, 9, 0, 30254, 0, 0, 0, 0, 0, '', 'Spellclick 59985 only while on quest 30254');

DELETE FROM smart_scripts WHERE entryorguid = 59985 AND source_type = 0;
INSERT INTO smart_scripts (entryorguid, source_type, id, link, event_type, event_phase_mask, event_chance, event_flags, event_param1, event_param2, event_param3, event_param4, event_param5, action_type, action_param1, action_param2, action_param3, action_param4, action_param5, action_param6, target_type, target_param1, target_param2, target_param3, target_param4, target_x, target_y, target_z, target_o, comment) VALUES
(59985, 0, 0, 1, 73, 0, 100, 0, 0, 0, 0, 0, 0, 33, 59985, 1, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 'Untilled Soil - On SpellClick - Kill Credit'),
(59985, 0, 1, 2, 61, 0, 100, 0, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Untilled Soil - Linked - Force Despawn'),
(59985, 0, 2, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 12, 59990, 7, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Untilled Soil - Linked - Summon Tilled Soil');

-- 4c. Set up 59990 (Tilled Soil) - spellclick with conditions to plant seeds
UPDATE creature_template SET npcflag = 16777216, AIName = 'SmartAI', type_flags = 1048576 WHERE entry = 59990;

DELETE FROM npc_spellclick_spells WHERE npc_entry = 59990;
INSERT INTO npc_spellclick_spells (npc_entry, spell_id, cast_flags, user_type) VALUES
(59990, 111108, 1, 0);

-- Conditions: player must be on quest 30254 AND have EZ-Gro Green Cabbage Seeds (item 80302)
DELETE FROM conditions WHERE SourceTypeOrReferenceId = 18 AND SourceGroup = 59990;
INSERT INTO conditions (SourceTypeOrReferenceId, SourceGroup, SourceEntry, SourceId, ElseGroup, ConditionTypeOrReference, ConditionTarget, ConditionValue1, ConditionValue2, ConditionValue3, NegativeCondition, ErrorType, ErrorTextId, ScriptName, Comment) VALUES
(18, 59990, 111108, 0, 0, 9, 0, 30254, 0, 0, 0, 0, 0, '', 'Spellclick 59990 only while on quest 30254'),
(18, 59990, 111108, 0, 0, 2, 0, 80302, 1, 0, 0, 0, 0, '', 'Spellclick 59990 only if player has 1x EZ-Gro Green Cabbage Seeds');

DELETE FROM smart_scripts WHERE entryorguid = 59990 AND source_type = 0;
INSERT INTO smart_scripts (entryorguid, source_type, id, link, event_type, event_phase_mask, event_chance, event_flags, event_param1, event_param2, event_param3, event_param4, event_param5, action_type, action_param1, action_param2, action_param3, action_param4, action_param5, action_param6, target_type, target_param1, target_param2, target_param3, target_param4, target_x, target_y, target_z, target_o, comment) VALUES
(59990, 0, 0, 1, 73, 0, 100, 0, 0, 0, 0, 0, 0, 33, 59990, 1, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 'Tilled Soil - On SpellClick - Kill Credit'),
(59990, 0, 1, 2, 61, 0, 100, 0, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Tilled Soil - Linked - Force Despawn'),
(59990, 0, 2, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 12, 59987, 7, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Tilled Soil - Linked - Summon Cabbage');

-- 4d. Fix creature_template flags for Parched Cabbage
UPDATE creature_template SET type_flags = 1048576 WHERE entry = 59987;

-- ============================================================
-- 5. Fix quest 30256 (Learn and Grow IV: Harvesting)
--    Spawn ripe cabbages so the player can harvest them
-- ============================================================

-- 5a. Fix type_flags on Ripe Cabbage (59833)
UPDATE creature_template SET type_flags = 1048576 WHERE entry = 59833;

-- 5b. Farmer Yoon summons 2x Ripe Cabbage when quest 30256 is accepted
--     Uses same plot positions as the original untilled soil (section 4a)
DELETE FROM smart_scripts WHERE entryorguid = 58646 AND source_type = 0 AND id IN (15, 16);
INSERT INTO smart_scripts (entryorguid, source_type, id, link, event_type, event_phase_mask, event_chance, event_flags, event_param1, event_param2, event_param3, event_param4, event_param5, action_type, action_param1, action_param2, action_param3, action_param4, action_param5, action_param6, target_type, target_param1, target_param2, target_param3, target_param4, target_x, target_y, target_z, target_o, comment) VALUES
(58646, 0, 15, 0, 19, 0, 100, 0, 30256, 0, 0, 0, 0, 12, 59833, 7, 0, 0, 0, 0, 8, 0, 0, 0, 0, -164.0, 638.0, 165.4, 0, 'Farmer Yoon - On Quest Accept Summon Ripe Cabbage 1'),
(58646, 0, 16, 0, 19, 0, 100, 0, 30256, 0, 0, 0, 0, 12, 59833, 7, 0, 0, 0, 0, 8, 0, 0, 0, 0, -164.5, 642.0, 165.4, 0, 'Farmer Yoon - On Quest Accept Summon Ripe Cabbage 2');
