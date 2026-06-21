-- Tillers Companion NPCs and GameObjects
-- Creates template entries for all 10 best friend unlocks (7 creatures + 3 GOs)
-- Adds spawn positions on farm map (870) for position loading

-- ========================================
-- 7 Companion Creatures
-- ========================================

-- Helper template: use Farm Sheep (59601) as base for all companions
INSERT IGNORE INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `difficulty_entry_4`, `difficulty_entry_5`, `KillCredit1`, `KillCredit2`, `name`, `femaleName`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `exp_unk`, `faction`, `npcflag`, `npcflag2`, `speed_walk`, `speed_run`, `scale`, `rank`, `mindmg`, `maxdmg`, `dmgschool`, `attackpower`, `dmg_multiplier`, `BaseAttackTime`, `RangeAttackTime`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_class`, `trainer_race`, `minrangedmg`, `maxrangedmg`, `rangedattackpower`, `type`, `type_flags`, `type_flags2`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `HoverHeight`, `Health_mod`, `Mana_mod`, `Mana_mod_extra`, `Armor_mod`, `RacialLeader`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `movementId`, `RegenHealth`, `VignetteID`, `TrackingQuestID`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES
(85814, 0, 0, 0, 0, 0, 0, 0, 'Shaggy the Yak', '0', 'Farmer Fung\'s Best Friend', 'NULL', 0, 90, 90, 4, 0, 35, 0, 0, 1, 1.14286, 1, 0, 14666, 24933, 0, 42296, 1, 2000, 2000, 1, 32768, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, '', 18414),
(85791, 0, 0, 0, 0, 0, 0, 0, 'Miss Fifi', '0', 'Haohan Mudclaw\'s Best Friend', 'NULL', 0, 90, 90, 4, 0, 35, 0, 0, 1, 1.14286, 1, 0, 14666, 24933, 0, 42296, 1, 2000, 2000, 1, 32768, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, '', 18414),
(85820, 0, 0, 0, 0, 0, 0, 0, 'Hillpaw Chickens', '0', 'Old Hillpaw\'s Best Friend', 'NULL', 0, 90, 90, 4, 0, 35, 0, 0, 1, 1.14286, 1, 0, 14666, 24933, 0, 42296, 1, 2000, 2000, 1, 32768, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, '', 18414),
(85808, 0, 0, 0, 0, 0, 0, 0, 'Farm Sheep', '0', 'Chee Chee\'s Best Friend', 'NULL', 0, 90, 90, 4, 0, 35, 0, 0, 1, 1.14286, 1, 0, 14666, 24933, 0, 42296, 1, 2000, 2000, 1, 32768, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, '', 18414),
(85818, 0, 0, 0, 0, 0, 0, 0, 'Luna the Cat', '0', 'Ella\'s Best Friend', 'NULL', 0, 90, 90, 4, 0, 35, 0, 0, 1, 1.14286, 0.6, 0, 14666, 24933, 0, 42296, 1, 2000, 2000, 1, 32768, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, '', 18414),
(85802, 0, 0, 0, 0, 0, 0, 0, 'Piggy Pigs', '0', 'Fish Fellreed\'s Best Friend', 'NULL', 0, 90, 90, 4, 0, 35, 0, 0, 1, 1.14286, 1, 0, 14666, 24933, 0, 42296, 1, 2000, 2000, 1, 32768, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, '', 18414),
(85826, 0, 0, 0, 0, 0, 0, 0, 'Lost Dog', '0', 'Lost Dog (Quest 30526)', 'NULL', 0, 90, 90, 4, 0, 35, 0, 0, 1, 1.14286, 1, 0, 14666, 24933, 0, 42296, 1, 2000, 2000, 1, 32768, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, '', 18414);

-- Display models (creature_template_model)
INSERT IGNORE INTO `creature_template_model` (`CreatureID`, `Idx`, `CreatureDisplayID`, `DisplayScale`, `Probability`, `VerifiedBuild`) VALUES
(85814, 0, 40703, 1, 1, 18414),
(85814, 1, 40694, 1, 1, 18414),
(85814, 2, 40695, 1, 1, 18414),
(85814, 3, 40733, 1, 1, 18414),
(85791, 0, 40876, 1, 1, 18414),
(85791, 1, 41074, 1, 1, 18414),
(85820, 0, 304, 1, 1, 18414),
(85820, 1, 5369, 1, 1, 18414),
(85808, 0, 856, 1, 1, 18414),
(85818, 0, 5586, 1, 1, 18414),
(85818, 1, 5585, 1, 1, 18414),
(85818, 2, 5555, 1, 1, 18414),
(85818, 3, 5556, 1, 1, 18414),
(85802, 0, 16356, 1, 1, 18414),
(85802, 1, 16357, 1, 1, 18414),
(85802, 2, 16358, 1, 1, 18414),
(85826, 0, 1100, 1, 1, 18414);

-- Creature template addon (minimal — no auras needed for companions)
INSERT IGNORE INTO `creature_template_addon` (`entry`, `mount`, `bytes1`, `bytes2`, `emote`, `aiAnimKit`, `movementAnimKit`, `meleeAnimKit`, `auras`) VALUES
(85814, 0, 0, 1, 0, 0, 0, 0, ''),
(85791, 0, 0, 1, 0, 0, 0, 0, ''),
(85820, 0, 0, 1, 0, 0, 0, 0, ''),
(85808, 0, 0, 1, 0, 0, 0, 0, ''),
(85818, 0, 0, 1, 0, 0, 0, 0, ''),
(85802, 0, 0, 1, 0, 0, 0, 0, ''),
(85826, 0, 0, 1, 0, 0, 0, 0, '');

-- ========================================
-- 3 Companion GameObjects
-- ========================================

INSERT IGNORE INTO `gameobject_template` (`entry`, `type`, `displayId`, `name`, `IconName`, `castBarCaption`, `unk1`, `faction`, `flags`, `size`, `Data0`, `Data1`, `Data2`, `Data3`, `Data4`, `Data5`, `Data6`, `Data7`, `Data8`, `Data9`, `Data10`, `Data11`, `Data12`, `Data13`, `Data14`, `Data15`, `Data16`, `Data17`, `Data18`, `Data19`, `Data20`, `Data21`, `Data22`, `Data23`, `Data24`, `Data25`, `Data26`, `Data27`, `Data28`, `Data29`, `Data30`, `Data31`, `Data32`, `AIName`, `ScriptName`, `VerifiedBuild`) VALUES
(237243, 0, 12810, 'Orange Tree (Sho\'s Best Friend)', '', '', '', 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 18414),
(237244, 0, 336, 'Farm Furniture (Tina\'s Best Friend)', '', '', '', 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 18414),
(237242, 19, 1907, 'Farm Mailbox (Gina\'s Best Friend)', '', '', '', 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 18414);

-- ========================================
-- Spawn positions on farm map 870
-- ========================================

-- Creature spawns (ground level, z ~165.5)
INSERT IGNORE INTO `creature` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseId`, `phaseGroup`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawntimesecs_max`, `wander_distance`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `dynamicflags`, `ScriptName`, `walk_mode`, `VerifiedBuild`) VALUES
(516068, 85814, 870, 0, 0, 1, 1, 0, 0, 0, 0, -190.0, 648.0, 165.5, 0.0, 300, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, '', 0, 18414),
(516069, 85791, 870, 0, 0, 1, 1, 0, 0, 0, 0, -165.0, 648.0, 165.5, 0.0, 300, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, '', 0, 18414),
(516070, 85820, 870, 0, 0, 1, 1, 0, 0, 0, 0, -195.0, 635.0, 165.5, 0.0, 300, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, '', 0, 18414),
(516071, 85808, 870, 0, 0, 1, 1, 0, 0, 0, 0, -162.0, 637.0, 165.5, 0.0, 300, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, '', 0, 18414),
(516072, 85818, 870, 0, 0, 1, 1, 0, 0, 0, 0, -173.0, 626.0, 165.5, 0.0, 300, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, '', 0, 18414),
(516073, 85802, 870, 0, 0, 1, 1, 0, 0, 0, 0, -183.0, 653.0, 165.5, 0.0, 300, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, '', 0, 18414),
(516074, 85826, 870, 0, 0, 1, 1, 0, 0, 0, 0, -177.0, 643.0, 165.5, 0.0, 300, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, '', 0, 18414);

-- GameObject spawns (ground level, z ~165.5)
INSERT IGNORE INTO `gameobject` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseId`, `phaseGroup`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `ScriptName`, `VerifiedBuild`) VALUES
(516075, 237243, 870, 0, 0, 1, 1, 0, 0, -198.0, 642.0, 165.5, 0.0, 0, 0, 0, 1, 300, 100, 1, '', 18414),
(516076, 237244, 870, 0, 0, 1, 1, 0, 0, -177.0, 623.0, 165.5, 0.0, 0, 0, 0, 1, 300, 100, 1, '', 18414),
(516077, 237242, 870, 0, 0, 1, 1, 0, 0, -157.0, 644.0, 165.5, 0.0, 0, 0, 0, 1, 300, 100, 1, '', 18414);
