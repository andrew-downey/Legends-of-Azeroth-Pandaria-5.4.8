-- Tillers Vote Questlines: ScriptName bindings for Mung-Mung and Nana Mudclaw
-- These NPCs do NOT overlap with npc_tillers_friendship (10 friendship NPCs).
-- Mung-Mung (58733) and Nana Mudclaw (64597) are separate vote-chain NPCs.

UPDATE `creature_template` SET `ScriptName` = 'npc_mung_mung' WHERE `entry` = 58733;
UPDATE `creature_template` SET `ScriptName` = 'npc_nana_mudclaw' WHERE `entry` = 64597;
