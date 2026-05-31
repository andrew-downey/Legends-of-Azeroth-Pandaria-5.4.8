-- Tillers crop creature templates for spellclick interaction
-- These creatures appear as small plants growing from soil patches at different growth stages

-- Crop plant - initial seedling stage (growth 0-25%)
DELETE FROM `creature_template` WHERE `entry` = 89001;
INSERT INTO `creature_template` (`entry`, `name`, `subname`, `type`, `faction`, `scale`, `npcflag`, `unit_class`, `rank`, `mindmg`, `maxdmg`, `attackpower`, `Health_mod`, `Mana_mod`, `Armor_mod`, `flags_extra`, `ScriptName`) VALUES
(89001, 'Seedling', '', 7, 35, 0.50, 2, 1, 0, 0, 0, 0, 0, 0, 0, 268435456, '');

-- Crop plant - early growth stage (growth 25-50%)
DELETE FROM `creature_template` WHERE `entry` = 89002;
INSERT INTO `creature_template` (`entry`, `name`, `subname`, `type`, `faction`, `scale`, `npcflag`, `unit_class`, `rank`, `mindmg`, `maxdmg`, `attackpower`, `Health_mod`, `Mana_mod`, `Armor_mod`, `flags_extra`, `ScriptName`) VALUES
(89002, 'Young Sprout', '', 7, 35, 0.60, 2, 1, 0, 0, 0, 0, 0, 0, 0, 268435456, '');

-- Crop plant - mid growth stage (growth 50-75%)
DELETE FROM `creature_template` WHERE `entry` = 89003;
INSERT INTO `creature_template` (`entry`, `name`, `subname`, `type`, `faction`, `scale`, `npcflag`, `unit_class`, `rank`, `mindmg`, `maxdmg`, `attackpower`, `Health_mod`, `Mana_mod`, `Armor_mod`, `flags_extra`, `ScriptName`) VALUES
(89003, 'Growing Crop', '', 7, 35, 0.75, 2, 1, 0, 0, 0, 0, 0, 0, 0, 268435456, '');

-- Crop plant - late growth stage (growth 75-99%)
DELETE FROM `creature_template` WHERE `entry` = 89004;
INSERT INTO `creature_template` (`entry`, `name`, `subname`, `type`, `faction`, `scale`, `npcflag`, `unit_class`, `rank`, `mindmg`, `maxdmg`, `attackpower`, `Health_mod`, `Mana_mod`, `Armor_mod`, `flags_extra`, `ScriptName`) VALUES
(89004, 'Nearly Ripe Crop', '', 7, 35, 0.85, 2, 1, 0, 0, 0, 0, 0, 0, 0, 268435456, '');

-- Crop plant - fully mature (READY_TO_HARVEST)
DELETE FROM `creature_template` WHERE `entry` = 89005;
INSERT INTO `creature_template` (`entry`, `name`, `subname`, `type`, `faction`, `scale`, `npcflag`, `unit_class`, `rank`, `mindmg`, `maxdmg`, `attackpower`, `Health_mod`, `Mana_mod`, `Armor_mod`, `flags_extra`, `ScriptName`) VALUES
(89005, 'Mature Crop', '', 7, 35, 1.00, 2, 1, 0, 0, 0, 0, 0, 0, 0, 268435456, '');

-- Crop plant - pest infested
DELETE FROM `creature_template` WHERE `entry` = 89006;
INSERT INTO `creature_template` (`entry`, `name`, `subname`, `type`, `faction`, `scale`, `npcflag`, `unit_class`, `rank`, `mindmg`, `maxdmg`, `attackpower`, `Health_mod`, `Mana_mod`, `Armor_mod`, `flags_extra`, `ScriptName`) VALUES
(89006, 'Pest Infested Plant', '', 7, 35, 0.70, 2, 1, 0, 0, 0, 0, 0, 0, 0, 268435456, '');

-- Crop plant - dried out (needs watering)
DELETE FROM `creature_template` WHERE `entry` = 89007;
INSERT INTO `creature_template` (`entry`, `name`, `subname`, `type`, `faction`, `scale`, `npcflag`, `unit_class`, `rank`, `mindmg`, `maxdmg`, `attackpower`, `Health_mod`, `Mana_mod`, `Armor_mod`, `flags_extra`, `ScriptName`) VALUES
(89007, 'Drying Plant', '', 7, 35, 0.65, 2, 1, 0, 0, 0, 0, 0, 0, 0, 268435456, '');
