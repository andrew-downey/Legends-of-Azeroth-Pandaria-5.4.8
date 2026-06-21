-- Taurast NPC -- Orgrimmar Auction House, east entrance
-- Custom IDs: 900001 (creature), 999001 (broadcast_text/gossip_menu)

-- ===================================================================
-- Part A: Broadcast text
-- ===================================================================

DELETE FROM `broadcast_text` WHERE `ID` = 999001;
INSERT INTO `broadcast_text` (`ID`, `LanguageID`, `Text`, `Text1`, `EmoteID1`, `EmoteID2`, `EmoteID3`, `EmoteDelay1`, `EmoteDelay2`, `EmoteDelay3`, `SoundEntriesID`, `EmotesID`, `Flags`, `VerifiedBuild`) VALUES
(999001, 0, 'Hey $n, how''s it going?', '', 0, 0, 0, 0, 0, 0, 0, 0, 1, 0);

-- ===================================================================
-- Part B: Gossip menu
-- ===================================================================

DELETE FROM `gossip_menu` WHERE `MenuID` = 999001 AND `TextID` = 999001;
INSERT INTO `gossip_menu` (`MenuID`, `TextID`, `VerifiedBuild`) VALUES
(999001, 999001, 0);

-- ===================================================================
-- Part C: Creature template + model
-- ===================================================================

DELETE FROM `creature_template` WHERE `entry` = 900001;
INSERT INTO `creature_template` (`entry`, `name`, `subname`, `gossip_menu_id`, `type`, `faction`, `scale`, `npcflag`, `unit_class`, `rank`, `mindmg`, `maxdmg`, `attackpower`, `Health_mod`, `Mana_mod`, `Armor_mod`, `flags_extra`, `ScriptName`) VALUES
(900001, 'Taurast', 'Auction House Regular', 999001, 7, 35, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, '');

DELETE FROM `creature_template_model` WHERE `CreatureID` = 900001;
INSERT INTO `creature_template_model` (`CreatureID`, `Idx`, `CreatureDisplayID`, `DisplayScale`, `Probability`, `VerifiedBuild`) VALUES
(900001, 0, 59, 1, 1, 0);

-- ===================================================================
-- Part D: Spawn
-- ===================================================================

DELETE FROM `creature` WHERE `id` = 900001;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `wander_distance`, `MovementType`, `curhealth`, `curmana`, `VerifiedBuild`) VALUES
(900001, 1, 0, 0, 1, 1, 0, 0, 1650.0, -4446.0, 18.6, 3.14, 300, 0, 0, 32000, 0, 0);
