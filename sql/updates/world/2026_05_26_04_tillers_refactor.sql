-- Tillers Phase 4B Refactor: Standard quest system migration
-- Quests 32000-32023 are daily quests (must have QuestInfo ID = 1 for dailies)

-- Farmer Yoon: crop dailies (32000-32009)
INSERT IGNORE INTO `creature_queststarter` (`id`, `quest`) VALUES
(58646, 32000), (58646, 32001), (58646, 32002), (58646, 32003),
(58646, 32004), (58646, 32005), (58646, 32006), (58646, 32007),
(58646, 32008), (58646, 32009);

-- Farmer Yoon: kill dailies (32019-32023)
INSERT IGNORE INTO `creature_queststarter` (`id`, `quest`) VALUES
(58646, 32019), (58646, 32020), (58646, 32021), (58646, 32022),
(58646, 32023);

-- Farmer Yoon: all as quest finishers too
INSERT IGNORE INTO `creature_questender` (`id`, `quest`) VALUES
(58646, 32000), (58646, 32001), (58646, 32002), (58646, 32003),
(58646, 32004), (58646, 32005), (58646, 32006), (58646, 32007),
(58646, 32008), (58646, 32009),
(58646, 32019), (58646, 32020), (58646, 32021), (58646, 32022),
(58646, 32023);

-- Andi: gift daily (32010)
INSERT IGNORE INTO `creature_queststarter` (`id`, `quest`) VALUES (58708, 32010);
INSERT IGNORE INTO `creature_questender` (`id`, `quest`) VALUES (58708, 32010);

-- Visiting farmers at Halfhill: creature_queststarter entries
INSERT IGNORE INTO `creature_queststarter` (`id`, `quest`) VALUES
(58734, 32011), (58732, 32012), (58730, 32013), (58735, 32014),
(58736, 32015), (58737, 32016), (58738, 32017), (58739, 32018);

-- Visiting farmers: creature_questender entries
INSERT IGNORE INTO `creature_questender` (`id`, `quest`) VALUES
(58734, 32011), (58732, 32012), (58730, 32013), (58735, 32014),
(58736, 32015), (58737, 32016), (58738, 32017), (58739, 32018);

-- Remove old ScriptName from Andi (now handled by DB quest system)
UPDATE `creature_template` SET `ScriptName` = '' WHERE `entry` = 58708;

-- Ensure friendship NPCs still have their script
UPDATE `creature_template` SET `ScriptName` = 'npc_tillers_friendship' WHERE `entry` IN (58712, 58703, 58707, 58706, 58547, 58746, 58704, 58705, 58709, 58711);
