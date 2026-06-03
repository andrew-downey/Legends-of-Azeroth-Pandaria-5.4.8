-- Tillers Farm: Fix quest 30257 (Learn and Grow V: Halfhill Market)
-- Dark Soil (GO 210565) spawns under Farmer Yoon's hut on quest accept.
-- Player right-clicks the Dark Soil to loot Marsh Lily (79269).
-- GO already has loot_template (41367) giving Marsh Lily as quest-only drop.
-- Using GO instead of creature (64279) because creature model was unclickable.

-- 1. Revert creature 64279 (Dark Soil) to default state (no longer used)
UPDATE creature_template SET npcflag = 0, AIName = '' WHERE entry = 64279;
DELETE FROM smart_scripts WHERE entryorguid = 64279 AND source_type = 0;
DELETE FROM npc_spellclick_spells WHERE npc_entry = 64279;
DELETE FROM conditions WHERE SourceTypeOrReferenceId = 18 AND SourceGroup = 64279;

-- 2. SAI for 58646 (Farmer Yoon): On quest 30257 accept → summon gameobject 210565 (Dark Soil)
DELETE FROM smart_scripts WHERE entryorguid = 58646 AND source_type = 0 AND id = 17;

INSERT INTO smart_scripts (entryorguid, source_type, id, link, event_type, event_phase_mask, event_chance, event_flags, event_param1, event_param2, event_param3, event_param4, event_param5, action_type, action_param1, action_param2, action_param3, action_param4, action_param5, action_param6, target_type, target_param1, target_param2, target_param3, target_param4, target_x, target_y, target_z, target_o, comment)
VALUES (58646, 0, 17, 0, 19, 0, 100, 0, 30257, 0, 0, 0, 0, 50, 210565, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, -160.348, 605.14, 168.056, 0, 'Farmer Yoon - On Quest Accept Summon Dark Soil (GO)');
