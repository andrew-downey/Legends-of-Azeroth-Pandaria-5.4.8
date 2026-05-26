-- Tillers Farm System - Bind friendship NPCs and shrine script
UPDATE `creature_template` SET `ScriptName` = 'npc_tillers_friendship' WHERE `entry` = 58712; -- Chee Chee
UPDATE `creature_template` SET `ScriptName` = 'npc_tillers_friendship' WHERE `entry` = 58703; -- Ella
UPDATE `creature_template` SET `ScriptName` = 'npc_tillers_friendship' WHERE `entry` = 58707; -- Farmer Fung
UPDATE `creature_template` SET `ScriptName` = 'npc_tillers_friendship' WHERE `entry` = 58706; -- Fish Fellreed
UPDATE `creature_template` SET `ScriptName` = 'npc_tillers_friendship' WHERE `entry` = 58547; -- Gina Mudclaw
UPDATE `creature_template` SET `ScriptName` = 'npc_tillers_friendship' WHERE `entry` = 58746; -- Haohan Mudclaw
UPDATE `creature_template` SET `ScriptName` = 'npc_tillers_friendship' WHERE `entry` = 58704; -- Jogu
UPDATE `creature_template` SET `ScriptName` = 'npc_tillers_friendship' WHERE `entry` = 58705; -- Old Hillpaw
UPDATE `creature_template` SET `ScriptName` = 'npc_tillers_friendship' WHERE `entry` = 58709; -- Sho
UPDATE `creature_template` SET `ScriptName` = 'npc_tillers_friendship' WHERE `entry` = 58711; -- Tina Mudclaw

-- Tillers Shrine GameObject
UPDATE `gameobject_template` SET `ScriptName` = 'go_tillers_shrine' WHERE `entry` = 215705;

-- Dark Soil GameObject
UPDATE `gameobject_template` SET `ScriptName` = 'go_dark_soil' WHERE `entry` = 210565;
