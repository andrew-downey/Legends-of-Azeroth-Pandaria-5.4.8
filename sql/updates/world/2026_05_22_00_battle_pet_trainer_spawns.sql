-- Battle Pet Trainer Creature Spawns
-- Places all 16 talkable pet trainers (npcflag & 2) that were missing from world.creature
-- Each trainer is placed +4 units east of their nearest innkeeper for easy discovery
-- curhealth is level-appropriate (based on innkeeper health/level ratio)
-- Idempotent: can be re-run to update positions (deletes old, inserts fresh)
-- Total NPCs: 16

SET @CGUID = (SELECT MAX(guid) + 1 FROM world.creature);

-- Clean slate for idempotency
DELETE FROM `creature` WHERE `id` IN (63194, 65651, 65655, 65656, 66135, 66136, 66137, 66372, 66422, 66466, 66522, 66557, 66675, 66730, 66824, 73138);

INSERT INTO `creature` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseId`, `phaseGroup`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawntimesecs_max`, `wander_distance`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `dynamicflags`, `ScriptName`, `walk_mode`, `VerifiedBuild`) VALUES

-- 1. Steven Lisbane (34) -> Innkeeper Thulbek (Badlands, map 0)
(@CGUID+0,  63194,  0, 0, 0, 1, 1, 0, 0, 0, 0, -12430.4,   211.559,  2.44848,  3.14159, 300, 0, 0, 0, 1020,   0, 0, 0, 0, 0, 0, 0, '', 0, 0),
-- 2. Lindsay (18) -> Innkeeper Brianna (Elwynn Forest, map 0)
(@CGUID+1,  65651,  0, 0, 0, 1, 1, 0, 0, 0, 0, -9219.98,  -2157.12,  64.0168,  3.14159, 300, 0, 0, 0, 540,    0, 0, 0, 0, 0, 0, 0, '', 0, 0),
-- 3. Eric Davidson (30) -> Innkeeper Heather (Duskwood, map 0)
(@CGUID+2,  65655,  0, 0, 0, 1, 1, 0, 0, 0, 0, -10649.3,   1166.43,  34.9278,  3.14159, 300, 0, 0, 0, 900,    0, 0, 0, 0, 0, 0, 0, '', 0, 0),
-- 4. Bill Buckler (32) -> Innkeeper Skindle (Booty Bay, map 0)
(@CGUID+3,  65656,  0, 0, 0, 1, 1, 0, 0, 0, 0, -14453.7,   495.348,  15.2128,  3.14159, 300, 0, 0, 0, 960,    0, 0, 0, 0, 0, 0, 0, '', 0, 0),
-- 5. Dagra the Fierce (13) -> Innkeeper Kerntis (Crossroads, map 1)
(@CGUID+4,  66135,  1, 0, 0, 1, 1, 0, 0, 0, 0, 282.771,   -3029.59,  97.4772,  3.14159, 300, 0, 0, 0, 390,    0, 0, 0, 0, 0, 0, 0, '', 0, 0),
-- 6. Analynn (18) -> Innkeeper Duras (Thunder Bluff, map 1)
(@CGUID+5,  66136,  1, 0, 0, 1, 1, 0, 0, 0, 0, 3364.08,   950.816,  7.4876,   3.14159, 300, 0, 0, 0, 540,    0, 0, 0, 0, 0, 0, 0, '', 0, 0),
-- 7. Zonya the Sadist (30) -> Innkeeper Jayka (Orgrimmar, map 1)
(@CGUID+6,  66137,  1, 0, 0, 1, 1, 0, 0, 0, 0, 897.652,   927.946,  106.364,  3.14159, 300, 0, 0, 0, 900,    0, 0, 0, 0, 0, 0, 0, '', 0, 0),
-- 8. Merda Stronghoof (32) -> Innkeeper Lyshaerya (Stonetalon, map 1)
(@CGUID+7,  66372,  1, 0, 0, 1, 1, 0, 0, 0, 0, 259.615,   1253.76,  192.224,  3.14159, 300, 0, 0, 0, 960,    0, 0, 0, 0, 0, 0, 0, '', 0, 0),
-- 9. Cassandra Kaboom (35) -> Innkeeper Lhakadd (Camp Mojache, map 1)
(@CGUID+8,  66422,  1, 0, 0, 1, 1, 0, 0, 0, 0, -3216.69,  -1663.86,  99.9925,  3.14159, 300, 0, 0, 0, 1050,   0, 0, 0, 0, 0, 0, 0, '', 0, 0),
-- 10. Stone Cold Trixxy (50) -> Innkeeper Vizzie (Lost Rigger, Tanaris, map 1)
(@CGUID+9,  66466,  1, 0, 0, 1, 1, 0, 0, 0, 0, 6699.15,   -4673.04,  721.65,   3.14159, 300, 0, 0, 0, 2000,   0, 0, 0, 0, 0, 0, 0, '', 0, 0),
-- 11. Lydia Accoste (56) -> Innkeeper Trelayne (Redridge, map 0)
(@CGUID+10, 66522,  0, 0, 0, 1, 1, 0, 0, 0, 0, -10512,    -1161.21,  28.1161,  3.14159, 300, 0, 0, 0, 2240,   0, 0, 0, 0, 0, 0, 0, '', 0, 0),
-- 12. Bloodknight Antari (70) -> Innkeeper Biribi (Area 52, map 530)
(@CGUID+11, 66557,  530, 0, 0, 1, 1, 0, 0, 0, 0, -2913.88,   4021.48,  0.510786, 3.14159, 300, 0, 0, 0, 9100,   0, 0, 0, 0, 0, 0, 0, '', 0, 0),
-- 13. Major Payne (80) -> Innkeeper Celeste (Borean Tundra, map 571)
(@CGUID+12, 66675,  571, 0, 0, 1, 1, 0, 0, 0, 0, 1449.08,   -3263.72,  167.302,  3.14159, 300, 0, 0, 0, 8000,   0, 0, 0, 0, 0, 0, 0, '', 0, 0),
-- 14. Hyuna of the Shrines (86) -> Innkeeper Lin Windfur (Tavern in the Mists, map 870)
(@CGUID+13, 66730,  870, 0, 0, 1, 1, 0, 0, 0, 0, 1610.8,    -1733.81,  274.067,  3.14159, 300, 0, 0, 0, 11610,  0, 0, 0, 0, 0, 0, 0, '', 0, 0),
-- 15. Obalis (85) -> Innkeeper Kazemde (Ramkahen, map 1)
(@CGUID+14, 66824,  1, 0, 0, 1, 1, 0, 0, 0, 0, -9385.1,   -944.342,  113.847,  3.14159, 300, 0, 0, 0, 11475,  0, 0, 0, 0, 0, 0, 0, '', 0, 0),
-- 16. Wrathion (90) -> Shrine of Two Moons (Vale of Eternal Blossoms, map 870)
(@CGUID+15, 73138,  870, 0, 0, 1, 1, 0, 0, 0, 0, 1540,       958,       488,      0,       300, 0, 0, 0, 12160,  0, 0, 0, 0, 0, 0, 0, '', 0, 0);

-- ===========================================================================
-- Gossip menu for battle pet trainers
-- NPCs with gossip_menu_id=0 cause the server to send textId=0xffffff
-- (DEFAULT_GOSSIP_MESSAGE) in SMSG_GOSSIP_MESSAGE, which triggers the
-- client's hardcoded "Help help I'm being repressed" fallback and the
-- gossip window never renders when no quests accompany the packet.
-- Fix: assign a gossip_menu with a real npc_text ID (TextID=1 exists
-- in both the server npc_text table and the client's NPCText.db2).
-- Idempotent: DELETE + INSERT for gossip_menu, WHERE gossip_menu_id=0 for UPDATE.
-- ===========================================================================

DELETE FROM `gossip_menu` WHERE `MenuID` = 900001;
INSERT INTO `gossip_menu` (`MenuID`, `TextID`, `VerifiedBuild`) VALUES
(900001, 1, 0);

UPDATE `creature_template` SET `gossip_menu_id` = 900001
WHERE `entry` IN (63194, 65651, 65655, 65656, 66126, 66135, 66136, 66137,
                  66372, 66422, 66466, 66522, 66557, 66675, 66730, 66824, 73138)
  AND `gossip_menu_id` = 0;

-- ===========================================================================
-- Daily quests for battle pet trainers
-- NPCs with QUEST_GIVER flag but zero quests in creature_queststarter
-- don't show interaction. Each trainer needs at least one daily quest
-- to defeat them in a pet battle (type 11 = WINPETBATTLEAGAINSTNPC).
-- Idempotent: REPLACE INTO.
-- ===========================================================================

REPLACE INTO `creature_queststarter` (`id`, `quest`) VALUES
(63194, 31852),   -- Steven Lisbane
(65651, 31781),   -- Lindsay
(65655, 31850),   -- Eric Davidson
(65656, 31851),   -- Bill Buckler
(66135, 31819),   -- Dagra the Fierce
(66136, 31854),   -- Analynn
(66137, 31862),   -- Zonya the Sadist
(66372, 31872),   -- Merda Stronghoof
(66422, 31904),   -- Cassandra Kaboom
(66522, 31916),   -- Lydia Accoste
(66557, 31926),   -- Bloodknight Antari
(66675, 31935),   -- Major Payne
(66824, 31971);   -- Obalis

REPLACE INTO `creature_questender` (`id`, `quest`) VALUES
(63194, 31852),   -- Steven Lisbane
(65651, 31781),   -- Lindsay
(65655, 31850),   -- Eric Davidson
(65656, 31851),   -- Bill Buckler
(66135, 31819),   -- Dagra the Fierce
(66136, 31854),   -- Analynn
(66137, 31862),   -- Zonya the Sadist
(66372, 31872),   -- Merda Stronghoof
(66422, 31904),   -- Cassandra Kaboom
(66522, 31916),   -- Lydia Accoste
(66557, 31926),   -- Bloodknight Antari
(66675, 31935),   -- Major Payne
(66824, 31971);   -- Obalis

-- Fix Elena Flutterfly quest objective (had objectId=0 instead of 66412)
UPDATE `quest_objective` SET `objectId` = 66412
WHERE `questId` = 31908 AND `type` = 11;

-- ===========================================================================
-- Global curhealth fix for pre-existing pet trainer spawns
-- Many had curhealth=1, causing them to appear dead/uninteractable.
-- Proper values calculated as level * zone_ratio (~30 for EK, ~135 for Pandaria)
-- ===========================================================================

UPDATE `creature` SET `curhealth` = 390   WHERE `guid` = 563694;   -- Old MacDonald (lvl 13)
UPDATE `creature` SET `curhealth` = 11745 WHERE `guid` = 518828;   -- Mo'ruk (lvl 87)
UPDATE `creature` SET `curhealth` = 11745 WHERE `guid` = 515164;   -- Farmer Nishi (lvl 87)
UPDATE `creature` SET `curhealth` = 11880 WHERE `guid` = 509405;   -- Courageous Yon (lvl 88)
UPDATE `creature` SET `curhealth` = 11880 WHERE `guid` = 527473;   -- Wastewalker Shu (lvl 88)
UPDATE `creature` SET `curhealth` = 12150 WHERE `guid` = 534562;   -- Aki the Chosen (lvl 90)
UPDATE `creature` SET `curhealth` = 12015 WHERE `guid` = 526271;   -- Seeker Zusshi (lvl 89)
UPDATE `creature` SET `curhealth` = 12160 WHERE `guid` = 572358;   -- Flowing Pandaren Spirit (lvl 90)

-- ===========================================================================
-- World pet tamers (non-master NPCs) - 21 creature spawns
-- These are the pet battle target NPCs referenced in quest_objective type=11
-- but lacking creature spawns. Placed +4 units east of nearest innkeeper.
-- curhealth = level * zone_multiplier (30 Classic, 130 Outland, 100 NR, 135 Cata)
-- Idempotent: DELETE old + INSERT fresh.
-- ===========================================================================

SET @CGUID2 = (SELECT MAX(guid) + 1 FROM world.creature);

DELETE FROM `creature` WHERE `id` IN (66352, 66478, 66436, 66452, 66442, 66412, 66512, 66515, 66520, 66518, 66550, 66551, 66552, 66553, 66635, 66636, 66638, 66639, 66815, 66819, 66822);

INSERT INTO `creature` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseId`, `phaseGroup`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawntimesecs_max`, `wander_distance`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `dynamicflags`, `ScriptName`, `walk_mode`, `VerifiedBuild`) VALUES

-- 1. Traitor Gluk (34) -> Innkeeper Sikewa (Desolace, map 1)
(@CGUID2+0,  66352, 1, 0, 0, 1, 1, 0, 0, 0, 0, -1588.84, 3150.27, 47.0414, 3.14159, 300, 0, 0, 0, 1020, 0, 0, 0, 0, 0, 0, 0, '', 0, 0),
-- 2. David Kosse (34) -> Innkeeper Helbrek (Southshore, Hillsbrad, map 0)
(@CGUID2+1,  66478, 0, 0, 0, 1, 1, 0, 0, 0, 0, -3821.19, -833.29, 0.731596, 3.14159, 300, 0, 0, 0, 1020, 0, 0, 0, 0, 0, 0, 0, '', 0, 0),
-- 3. Grazzle the Great (39) -> Innkeeper Hurnahet (Thousand Needles, map 1)
(@CGUID2+2,  66436, 1, 0, 0, 1, 1, 0, 0, 0, 0, -781.29, -1552.46, 140.49, 3.14159, 300, 0, 0, 0, 1170, 0, 0, 0, 0, 0, 0, 0, '', 0, 0),
-- 4. Kela Grimtotem (42) -> Innkeeper Shyria (Feralas, map 1)
(@CGUID2+3,  66452, 1, 0, 0, 1, 1, 0, 0, 0, 0, -4457.78, 2228.63, 27.6743, 3.14159, 300, 0, 0, 0, 1260, 0, 0, 0, 0, 0, 0, 0, '', 0, 0),
-- 5. Zoltan (47) -> Innkeeper Hurnahet (offset +4 north to avoid Grazzle overlap, map 1)
(@CGUID2+4,  66442, 1, 0, 0, 1, 1, 0, 0, 0, 0, -785.29, -1548.46, 140.49, 4.71239, 300, 0, 0, 0, 1410, 0, 0, 0, 0, 0, 0, 0, '', 0, 0),
-- 6. Elena Flutterfly (48) -> Grimy Greasefingers (Azshara, map 1)
(@CGUID2+5,  66412, 1, 0, 0, 1, 1, 0, 0, 0, 0, 3533.67, -6518.78, 43.5852, 3.14159, 300, 0, 0, 0, 1440, 0, 0, 0, 0, 0, 0, 0, '', 0, 0),
-- 7. Deiza Plaguehorn (44) -> Innkeeper Hearthstove (Light's Hope, EPL, map 0)
(@CGUID2+6,  66512, 0, 0, 0, 1, 1, 0, 0, 0, 0, -5373.91, -2973.91, 323.252, 3.14159, 300, 0, 0, 0, 1320, 0, 0, 0, 0, 0, 0, 0, '', 0, 0),
-- 8. Kortas Darkhammer (50) -> Innkeeper Shul'kar (Kargath, Badlands, map 0)
(@CGUID2+7,  66515, 0, 0, 0, 1, 1, 0, 0, 0, 0, -6722.68, -2464.54, 274.661, 3.14159, 300, 0, 0, 0, 1500, 0, 0, 0, 0, 0, 0, 0, '', 0, 0),
-- 9. Durin Darkhammer (51) -> Innkeeper Durgens (Burning Steppes, map 0)
(@CGUID2+8,  66520, 0, 0, 0, 1, 1, 0, 0, 0, 0, -603.832, -1083.19, 65.899, 3.14159, 300, 0, 0, 0, 1530, 0, 0, 0, 0, 0, 0, 0, '', 0, 0),
-- 10. Everessa (53) -> Mama Morton (Swamp of Sorrows, map 0)
(@CGUID2+9,  66518, 0, 0, 0, 1, 1, 0, 0, 0, 0, -10921.3, -3417.68, 66.5808, 3.14159, 300, 0, 0, 0, 1590, 0, 0, 0, 0, 0, 0, 0, '', 0, 0),
-- 11. Nicki Tinytech (61) -> Innkeeper Kalarin (Honor Hold, Hellfire, map 530)
(@CGUID2+10, 66550, 530, 0, 0, 1, 1, 0, 0, 0, 0, 7557.37, -6897.35, 96.11, 3.14159, 300, 0, 0, 0, 7930, 0, 0, 0, 0, 0, 0, 0, '', 0, 0),
-- 12. Ras'an (63) -> Innkeeper Delaniel (Orebor Harborage, Zangarmarsh, map 530)
(@CGUID2+11, 66551, 530, 0, 0, 1, 1, 0, 0, 0, 0, 9480.91, -6859.21, 17.4246, 3.14159, 300, 0, 0, 0, 8190, 0, 0, 0, 0, 0, 0, 0, '', 0, 0),
-- 13. Narrok (65) -> Innkeeper Grilka (Shattrath/Terokkar, map 530)
(@CGUID2+12, 66552, 530, 0, 0, 1, 1, 0, 0, 0, 0, -2617.78, 4450.3, 36.1768, 3.14159, 300, 0, 0, 0, 8450, 0, 0, 0, 0, 0, 0, 0, '', 0, 0),
-- 14. Morulu The Elder (67) -> Innkeeper Remi Dodoso (Nagrand, map 530)
(@CGUID2+13, 66553, 530, 0, 0, 1, 1, 0, 0, 0, 0, 3066.15, 3701.81, 142.562, 3.14159, 300, 0, 0, 0, 8710, 0, 0, 0, 0, 0, 0, 0, '', 0, 0),
-- 15. Beegle Blastfuse (71) -> Innkeeper Hazel Lagras (Borean Tundra, map 571)
(@CGUID2+14, 66635, 571, 0, 0, 1, 1, 0, 0, 0, 0, 602.973, -4927.27, 17.8837, 3.14159, 300, 0, 0, 0, 7100, 0, 0, 0, 0, 0, 0, 0, '', 0, 0),
-- 16. Okrut Dragonwaste (73) -> Innkeeper Basil Osgood (Dragonblight, map 571)
(@CGUID2+15, 66638, 571, 0, 0, 1, 1, 0, 0, 0, 0, 443.569, -4550.82, 245.625, 3.14159, 300, 0, 0, 0, 7300, 0, 0, 0, 0, 0, 0, 0, '', 0, 0),
-- 17. Gutretch (75) -> Bori Wintertotem (Grizzly Hills, map 571)
(@CGUID2+16, 66639, 571, 0, 0, 1, 1, 0, 0, 0, 0, 2687.85, -4389.32, 284.363, 3.14159, 300, 0, 0, 0, 7500, 0, 0, 0, 0, 0, 0, 0, '', 0, 0),
-- 18. Nearly Headless Jacob (78) -> Charlie Northtop (Howling Fjord, map 571)
(@CGUID2+17, 66636, 571, 0, 0, 1, 1, 0, 0, 0, 0, 4182.56, 5278.32, 26.6935, 3.14159, 300, 0, 0, 0, 7800, 0, 0, 0, 0, 0, 0, 0, '', 0, 0),
-- 19. Brok (81) -> Grimy Greasefingers (Sanctuary of Malorne, Mount Hyjal, map 648)
(@CGUID2+18, 66819, 648, 0, 0, 1, 1, 0, 0, 0, 0, 603.734, 3118.63, 3.65963, 3.14159, 300, 0, 0, 0, 10935, 0, 0, 0, 0, 0, 0, 0, '', 0, 0),
-- 20. Bordin Steadyfist (82) -> Caretaker Nuunwa (Temple of Earth, Deepholm, map 646)
(@CGUID2+19, 66815, 646, 0, 0, 1, 1, 0, 0, 0, 0, 1036.42, 542.142, -49.3179, 3.14159, 300, 0, 0, 0, 11070, 0, 0, 0, 0, 0, 0, 0, '', 0, 0),
-- 21. Goz Banefury (84) -> Innkeeper Lutz (Twilight Highlands, map 0)
(@CGUID2+20, 66822, 0, 0, 0, 1, 1, 0, 0, 0, 0, -4005.87, -6439.9, 41.4245, 3.14159, 300, 0, 0, 0, 11340, 0, 0, 0, 0, 0, 0, 0, '', 0, 0);

-- Set npcflag=2 (QUEST_GIVER) and gossip_menu_id=900001 for world tamers
UPDATE `creature_template` SET `npcflag` = 2, `gossip_menu_id` = 900001
WHERE `entry` IN (66352, 66478, 66436, 66452, 66442, 66412, 66512, 66515, 66520, 66518, 66550, 66551, 66552, 66553, 66635, 66636, 66638, 66639, 66815, 66819, 66822)
  AND (`npcflag` = 0 OR `gossip_menu_id` = 0);

-- Daily quest links for world tamers
REPLACE INTO `creature_queststarter` (`id`, `quest`) VALUES
(66352, 31871),   -- Traitor Gluk
(66478, 31910),   -- David Kosse
(66436, 31905),   -- Grazzle the Great
(66452, 31906),   -- Kela Grimtotem
(66442, 31907),   -- Zoltan
(66412, 31908),   -- Elena Flutterfly
(66512, 31911),   -- Deiza Plaguehorn
(66515, 31912),   -- Kortas Darkhammer
(66520, 31914),   -- Durin Darkhammer
(66518, 31913),   -- Everessa
(66550, 31922),   -- Nicki Tinytech
(66551, 31923),   -- Ras'an
(66552, 31924),   -- Narrok
(66553, 31925),   -- Morulu The Elder
(66635, 31931),   -- Beegle Blastfuse
(66636, 31932),   -- Nearly Headless Jacob
(66638, 31933),   -- Okrut Dragonwaste
(66639, 31934),   -- Gutretch
(66819, 31972),   -- Brok
(66815, 31973),   -- Bordin Steadyfist
(66822, 31974);   -- Goz Banefury

REPLACE INTO `creature_questender` (`id`, `quest`) VALUES
(66352, 31871),   -- Traitor Gluk
(66478, 31910),   -- David Kosse
(66436, 31905),   -- Grazzle the Great
(66452, 31906),   -- Kela Grimtotem
(66442, 31907),   -- Zoltan
(66412, 31908),   -- Elena Flutterfly
(66512, 31911),   -- Deiza Plaguehorn
(66515, 31912),   -- Kortas Darkhammer
(66520, 31914),   -- Durin Darkhammer
(66518, 31913),   -- Everessa
(66550, 31922),   -- Nicki Tinytech
(66551, 31923),   -- Ras'an
(66552, 31924),   -- Narrok
(66553, 31925),   -- Morulu The Elder
(66635, 31931),   -- Beegle Blastfuse
(66636, 31932),   -- Nearly Headless Jacob
(66638, 31933),   -- Okrut Dragonwaste
(66639, 31934),   -- Gutretch
(66819, 31972),   -- Brok
(66815, 31973),   -- Bordin Steadyfist
(66822, 31974);   -- Goz Banefury

-- Battle pet team for Brok (Mount Hyjal, lvl 81, retail rare quality)
REPLACE INTO `battle_pet_trainer` (`entry`, `species`, `level`, `quality`, `breed`) VALUES
(66819, 981, 25, 3, 3),    -- Incinderous
(66819, 982, 25, 3, 3),    -- Ashtail
(66819, 983, 25, 3, 3);    -- Kali

-- Battle pet team for Jeremy Feasel (Darkmoon Faire, already spawned)
REPLACE INTO `battle_pet_trainer` (`entry`, `species`, `level`, `quality`, `breed`) VALUES
(67370, 1066, 25, 3, 3),   -- Judgment
(67370, 1067, 25, 3, 3),   -- Fezwick
(67370, 1068, 25, 3, 3);   -- Honky-Tonk

-- Battle pet team for Little Tommy Newcomer (Timeless Isle, already spawned)
REPLACE INTO `battle_pet_trainer` (`entry`, `species`, `level`, `quality`, `breed`) VALUES
(73626, 1343, 25, 5, 3);   -- Lil' Oondasta

-- Fix dead Darkmoon Faire Jeremy Feasel spawn
UPDATE `creature` SET `curhealth` = 1650 WHERE `id` = 67370 AND `curhealth` = 1;

-- ===========================================================================
-- First-time quest chain progression (creature_queststarter + ender)
-- Most first-time quests (Flags=524288, 0x80000) had no starter NPC.
-- Each quest can be picked up from the PREVIOUS trainer in the chain AND
-- from the target trainer (self-give). Turn-in is to the defeated trainer.
-- Idempotent: REPLACE INTO.
-- ===========================================================================

-- Alliance EK chain: Audrey Burnhep -> Julia -> Old Mac -> Lindsay ->
-- Eric -> Steven -> Bill -> A Tamer's Homecoming
REPLACE INTO `creature_queststarter` (`id`, `quest`) VALUES
(63596, 32008),   -- Audrey Burnhep gives intro
(64330, 31316),   -- Julia Stevens gives Julia quest
(65648, 31725),   -- Old MacDonald gives Lindsay quest
(65651, 31725),   -- Lindsay herself also gives the quest
(65651, 31726),   -- Lindsay gives Eric quest
(65655, 31726),   -- Eric himself also gives the quest
(65655, 31729),   -- Eric gives Steven quest
(63194, 31729),   -- Steven himself also gives the quest
(63194, 31728),   -- Steven gives Bill quest
(65656, 31728);   -- Bill himself also gives the quest

REPLACE INTO `creature_questender` (`id`, `quest`) VALUES
(63596, 32008),   -- Audrey accepts intro turn-in
(64330, 31316),   -- Julia accepts her own first-time turn-in
(65651, 31725),   -- Lindsay accepts her own first-time turn-in
(65655, 31726),   -- Eric accepts his own first-time turn-in
(63194, 31729),   -- Steven accepts his own first-time turn-in
(65656, 31728);   -- Bill accepts his own first-time turn-in

-- Horde Kalimdor chain: Varzok -> Zunta -> Dagra -> Analynn ->
-- Zonya -> Merda -> Cassandra -> A Tamer's Homecoming
REPLACE INTO `creature_queststarter` (`id`, `quest`) VALUES
(63626, 32009),   -- Varzok gives intro
(66126, 31813),   -- Zunta gives Dagra quest
(66135, 31813),   -- Dagra himself also gives the quest
(66135, 31814),   -- Dagra gives Analynn quest
(66136, 31814),   -- Analynn herself also gives the quest
(66136, 31815),   -- Analynn gives Zonya quest
(66137, 31815),   -- Zonya herself also gives the quest
(66137, 31817),   -- Zonya gives Merda quest
(66372, 31817),   -- Merda herself also gives the quest
(66372, 31870),   -- Merda gives Cassandra quest
(66422, 31870);   -- Cassandra herself also gives the quest

REPLACE INTO `creature_questender` (`id`, `quest`) VALUES
(63626, 32009),   -- Varzok accepts intro turn-in
(66126, 31812),   -- Zunta accepts her own first-time turn-in
(66126, 31813),   -- Zunta also accepts Dagra turn-in (original behavior)
(66135, 31813),   -- Dagra also accepts his own turn-in
(66136, 31814),   -- Analynn accepts her own first-time turn-in
(66137, 31815),   -- Zonya accepts her own first-time turn-in
(66372, 31817),   -- Merda accepts her own first-time turn-in
(66422, 31870);   -- Cassandra accepts her own first-time turn-in

-- Grand Master first-time quests (defeat the GM to unlock their daily)
REPLACE INTO `creature_queststarter` (`id`, `quest`) VALUES
(66522, 31915),   -- Lydia Accoste
(66557, 31920),   -- Bloodknight Antari
(66675, 31928),   -- Major Payne
(66741, 31951),   -- Aki the Chosen
(66824, 31970);   -- Obalis

REPLACE INTO `creature_questender` (`id`, `quest`) VALUES
(66522, 31915),   -- Lydia Accoste
(66557, 31920),   -- Bloodknight Antari
(66675, 31928),   -- Major Payne
(66824, 31970);   -- Obalis

-- Pandarian daily quest for Mo'ruk (was the only missing daily link)
REPLACE INTO `creature_queststarter` (`id`, `quest`) VALUES
(66733, 31954);   -- Mo'ruk gives daily

REPLACE INTO `creature_questender` (`id`, `quest`) VALUES
(66733, 31954);   -- Mo'ruk accepts daily

-- Whispering Pandaren Spirit was missing both its daily quest link and npcflag
REPLACE INTO `creature_queststarter` (`id`, `quest`) VALUES
(68464, 32440);   -- Whispering Spirit gives daily

REPLACE INTO `creature_questender` (`id`, `quest`) VALUES
(68464, 32440);   -- Whispering Spirit accepts daily

UPDATE `creature_template` SET `npcflag` = 2, `gossip_menu_id` = 900001
WHERE `entry` = 68464 AND (`npcflag` = 0 OR `gossip_menu_id` = 0);

-- ===========================================================================
-- Phase 4: Horde daily duplicate quests (Issue 1 fix)
--
-- 5 Classic dailies have PrevQuestID=31917 (Alliance A Tamer's Homecoming)
-- but AllowableRaces=33555378 (Horde). Horde players complete 31918 (their
-- faction version) and can never satisfy PrevQuestID=31917.
--
-- Fix: Create Horde-only copies (33009-33013) with PrevQuestID=31918.
-- Each trainer keeps links to BOTH quests. TC's quest system reads
-- AllowableRaces first, then PrevQuestID, so each faction sees the right one.
-- ===========================================================================

-- 3a: Copy quest_template for each daily with new ID

REPLACE INTO `quest_template`
SELECT 33009, `QuestType`, `QuestLevel`, `MinLevel`, `QuestPackageID`, `QuestSortID`,
       `QuestInfoID`, `SuggestedGroupNum`, `RewardNextQuest`, `RewardXPDifficulty`,
       `RewardMoney`, `RewardBonusMoney`, `RewardDisplaySpell`, `RewardSpell`,
       `RewardHonor`, `RewardKillHonor`, `StartItem`, `Flags`, `FlagsEx`,
       `RewardItem1`, `RewardAmount1`, `RewardItem2`, `RewardAmount2`,
       `RewardItem3`, `RewardAmount3`, `RewardItem4`, `RewardAmount4`,
       `ItemDrop1`, `ItemDropQuantity1`, `ItemDrop2`, `ItemDropQuantity2`,
       `ItemDrop3`, `ItemDropQuantity3`, `ItemDrop4`, `ItemDropQuantity4`,
       `RewardChoiceItemID1`, `RewardChoiceItemQuantity1`,
       `RewardChoiceItemID2`, `RewardChoiceItemQuantity2`,
       `RewardChoiceItemID3`, `RewardChoiceItemQuantity3`,
       `RewardChoiceItemID4`, `RewardChoiceItemQuantity4`,
       `RewardChoiceItemID5`, `RewardChoiceItemQuantity5`,
       `RewardChoiceItemID6`, `RewardChoiceItemQuantity6`,
       `POIContinent`, `POIx`, `POIy`, `POIPriority`,
       `RewardTitle`, `RewardArenaPoints`, `RewardSkillLineID`, `RewardNumSkillUps`,
       `PortraitGiver`, `PortraitTurnIn`,
       `RewardFactionID1`, `RewardFactionValue1`, `RewardFactionOverride1`,
       `RewardFactionID2`, `RewardFactionValue2`, `RewardFactionOverride2`,
       `RewardFactionID3`, `RewardFactionValue3`, `RewardFactionOverride3`,
       `RewardFactionID4`, `RewardFactionValue4`, `RewardFactionOverride4`,
       `RewardFactionID5`, `RewardFactionValue5`, `RewardFactionOverride5`,
       `RewardFactionFlags`,
       `RewardCurrencyID1`, `RewardCurrencyQty1`,
       `RewardCurrencyID2`, `RewardCurrencyQty2`,
       `RewardCurrencyID3`, `RewardCurrencyQty3`,
       `RewardCurrencyID4`, `RewardCurrencyQty4`,
       `AcceptedSoundKitID`, `CompleteSoundKitID`, `AreaGroupID`, `TimeAllowed`,
       `AllowableRaces`, `LogTitle`, `LogDescription`, `QuestDescription`,
       `AreaDescription`, `PortraitGiverText`, `PortraitGiverName`,
       `PortraitTurnInText`, `PortraitTurnInName`, `QuestCompletionLog`, `VerifiedBuild`
FROM `quest_template` WHERE `ID` = 31819;

REPLACE INTO `quest_template`
SELECT 33010, `QuestType`, `QuestLevel`, `MinLevel`, `QuestPackageID`, `QuestSortID`,
       `QuestInfoID`, `SuggestedGroupNum`, `RewardNextQuest`, `RewardXPDifficulty`,
       `RewardMoney`, `RewardBonusMoney`, `RewardDisplaySpell`, `RewardSpell`,
       `RewardHonor`, `RewardKillHonor`, `StartItem`, `Flags`, `FlagsEx`,
       `RewardItem1`, `RewardAmount1`, `RewardItem2`, `RewardAmount2`,
       `RewardItem3`, `RewardAmount3`, `RewardItem4`, `RewardAmount4`,
       `ItemDrop1`, `ItemDropQuantity1`, `ItemDrop2`, `ItemDropQuantity2`,
       `ItemDrop3`, `ItemDropQuantity3`, `ItemDrop4`, `ItemDropQuantity4`,
       `RewardChoiceItemID1`, `RewardChoiceItemQuantity1`,
       `RewardChoiceItemID2`, `RewardChoiceItemQuantity2`,
       `RewardChoiceItemID3`, `RewardChoiceItemQuantity3`,
       `RewardChoiceItemID4`, `RewardChoiceItemQuantity4`,
       `RewardChoiceItemID5`, `RewardChoiceItemQuantity5`,
       `RewardChoiceItemID6`, `RewardChoiceItemQuantity6`,
       `POIContinent`, `POIx`, `POIy`, `POIPriority`,
       `RewardTitle`, `RewardArenaPoints`, `RewardSkillLineID`, `RewardNumSkillUps`,
       `PortraitGiver`, `PortraitTurnIn`,
       `RewardFactionID1`, `RewardFactionValue1`, `RewardFactionOverride1`,
       `RewardFactionID2`, `RewardFactionValue2`, `RewardFactionOverride2`,
       `RewardFactionID3`, `RewardFactionValue3`, `RewardFactionOverride3`,
       `RewardFactionID4`, `RewardFactionValue4`, `RewardFactionOverride4`,
       `RewardFactionID5`, `RewardFactionValue5`, `RewardFactionOverride5`,
       `RewardFactionFlags`,
       `RewardCurrencyID1`, `RewardCurrencyQty1`,
       `RewardCurrencyID2`, `RewardCurrencyQty2`,
       `RewardCurrencyID3`, `RewardCurrencyQty3`,
       `RewardCurrencyID4`, `RewardCurrencyQty4`,
       `AcceptedSoundKitID`, `CompleteSoundKitID`, `AreaGroupID`, `TimeAllowed`,
       `AllowableRaces`, `LogTitle`, `LogDescription`, `QuestDescription`,
       `AreaDescription`, `PortraitGiverText`, `PortraitGiverName`,
       `PortraitTurnInText`, `PortraitTurnInName`, `QuestCompletionLog`, `VerifiedBuild`
FROM `quest_template` WHERE `ID` = 31854;

REPLACE INTO `quest_template`
SELECT 33011, `QuestType`, `QuestLevel`, `MinLevel`, `QuestPackageID`, `QuestSortID`,
       `QuestInfoID`, `SuggestedGroupNum`, `RewardNextQuest`, `RewardXPDifficulty`,
       `RewardMoney`, `RewardBonusMoney`, `RewardDisplaySpell`, `RewardSpell`,
       `RewardHonor`, `RewardKillHonor`, `StartItem`, `Flags`, `FlagsEx`,
       `RewardItem1`, `RewardAmount1`, `RewardItem2`, `RewardAmount2`,
       `RewardItem3`, `RewardAmount3`, `RewardItem4`, `RewardAmount4`,
       `ItemDrop1`, `ItemDropQuantity1`, `ItemDrop2`, `ItemDropQuantity2`,
       `ItemDrop3`, `ItemDropQuantity3`, `ItemDrop4`, `ItemDropQuantity4`,
       `RewardChoiceItemID1`, `RewardChoiceItemQuantity1`,
       `RewardChoiceItemID2`, `RewardChoiceItemQuantity2`,
       `RewardChoiceItemID3`, `RewardChoiceItemQuantity3`,
       `RewardChoiceItemID4`, `RewardChoiceItemQuantity4`,
       `RewardChoiceItemID5`, `RewardChoiceItemQuantity5`,
       `RewardChoiceItemID6`, `RewardChoiceItemQuantity6`,
       `POIContinent`, `POIx`, `POIy`, `POIPriority`,
       `RewardTitle`, `RewardArenaPoints`, `RewardSkillLineID`, `RewardNumSkillUps`,
       `PortraitGiver`, `PortraitTurnIn`,
       `RewardFactionID1`, `RewardFactionValue1`, `RewardFactionOverride1`,
       `RewardFactionID2`, `RewardFactionValue2`, `RewardFactionOverride2`,
       `RewardFactionID3`, `RewardFactionValue3`, `RewardFactionOverride3`,
       `RewardFactionID4`, `RewardFactionValue4`, `RewardFactionOverride4`,
       `RewardFactionID5`, `RewardFactionValue5`, `RewardFactionOverride5`,
       `RewardFactionFlags`,
       `RewardCurrencyID1`, `RewardCurrencyQty1`,
       `RewardCurrencyID2`, `RewardCurrencyQty2`,
       `RewardCurrencyID3`, `RewardCurrencyQty3`,
       `RewardCurrencyID4`, `RewardCurrencyQty4`,
       `AcceptedSoundKitID`, `CompleteSoundKitID`, `AreaGroupID`, `TimeAllowed`,
       `AllowableRaces`, `LogTitle`, `LogDescription`, `QuestDescription`,
       `AreaDescription`, `PortraitGiverText`, `PortraitGiverName`,
       `PortraitTurnInText`, `PortraitTurnInName`, `QuestCompletionLog`, `VerifiedBuild`
FROM `quest_template` WHERE `ID` = 31862;

REPLACE INTO `quest_template`
SELECT 33012, `QuestType`, `QuestLevel`, `MinLevel`, `QuestPackageID`, `QuestSortID`,
       `QuestInfoID`, `SuggestedGroupNum`, `RewardNextQuest`, `RewardXPDifficulty`,
       `RewardMoney`, `RewardBonusMoney`, `RewardDisplaySpell`, `RewardSpell`,
       `RewardHonor`, `RewardKillHonor`, `StartItem`, `Flags`, `FlagsEx`,
       `RewardItem1`, `RewardAmount1`, `RewardItem2`, `RewardAmount2`,
       `RewardItem3`, `RewardAmount3`, `RewardItem4`, `RewardAmount4`,
       `ItemDrop1`, `ItemDropQuantity1`, `ItemDrop2`, `ItemDropQuantity2`,
       `ItemDrop3`, `ItemDropQuantity3`, `ItemDrop4`, `ItemDropQuantity4`,
       `RewardChoiceItemID1`, `RewardChoiceItemQuantity1`,
       `RewardChoiceItemID2`, `RewardChoiceItemQuantity2`,
       `RewardChoiceItemID3`, `RewardChoiceItemQuantity3`,
       `RewardChoiceItemID4`, `RewardChoiceItemQuantity4`,
       `RewardChoiceItemID5`, `RewardChoiceItemQuantity5`,
       `RewardChoiceItemID6`, `RewardChoiceItemQuantity6`,
       `POIContinent`, `POIx`, `POIy`, `POIPriority`,
       `RewardTitle`, `RewardArenaPoints`, `RewardSkillLineID`, `RewardNumSkillUps`,
       `PortraitGiver`, `PortraitTurnIn`,
       `RewardFactionID1`, `RewardFactionValue1`, `RewardFactionOverride1`,
       `RewardFactionID2`, `RewardFactionValue2`, `RewardFactionOverride2`,
       `RewardFactionID3`, `RewardFactionValue3`, `RewardFactionOverride3`,
       `RewardFactionID4`, `RewardFactionValue4`, `RewardFactionOverride4`,
       `RewardFactionID5`, `RewardFactionValue5`, `RewardFactionOverride5`,
       `RewardFactionFlags`,
       `RewardCurrencyID1`, `RewardCurrencyQty1`,
       `RewardCurrencyID2`, `RewardCurrencyQty2`,
       `RewardCurrencyID3`, `RewardCurrencyQty3`,
       `RewardCurrencyID4`, `RewardCurrencyQty4`,
       `AcceptedSoundKitID`, `CompleteSoundKitID`, `AreaGroupID`, `TimeAllowed`,
       `AllowableRaces`, `LogTitle`, `LogDescription`, `QuestDescription`,
       `AreaDescription`, `PortraitGiverText`, `PortraitGiverName`,
       `PortraitTurnInText`, `PortraitTurnInName`, `QuestCompletionLog`, `VerifiedBuild`
FROM `quest_template` WHERE `ID` = 31872;

REPLACE INTO `quest_template`
SELECT 33013, `QuestType`, `QuestLevel`, `MinLevel`, `QuestPackageID`, `QuestSortID`,
       `QuestInfoID`, `SuggestedGroupNum`, `RewardNextQuest`, `RewardXPDifficulty`,
       `RewardMoney`, `RewardBonusMoney`, `RewardDisplaySpell`, `RewardSpell`,
       `RewardHonor`, `RewardKillHonor`, `StartItem`, `Flags`, `FlagsEx`,
       `RewardItem1`, `RewardAmount1`, `RewardItem2`, `RewardAmount2`,
       `RewardItem3`, `RewardAmount3`, `RewardItem4`, `RewardAmount4`,
       `ItemDrop1`, `ItemDropQuantity1`, `ItemDrop2`, `ItemDropQuantity2`,
       `ItemDrop3`, `ItemDropQuantity3`, `ItemDrop4`, `ItemDropQuantity4`,
       `RewardChoiceItemID1`, `RewardChoiceItemQuantity1`,
       `RewardChoiceItemID2`, `RewardChoiceItemQuantity2`,
       `RewardChoiceItemID3`, `RewardChoiceItemQuantity3`,
       `RewardChoiceItemID4`, `RewardChoiceItemQuantity4`,
       `RewardChoiceItemID5`, `RewardChoiceItemQuantity5`,
       `RewardChoiceItemID6`, `RewardChoiceItemQuantity6`,
       `POIContinent`, `POIx`, `POIy`, `POIPriority`,
       `RewardTitle`, `RewardArenaPoints`, `RewardSkillLineID`, `RewardNumSkillUps`,
       `PortraitGiver`, `PortraitTurnIn`,
       `RewardFactionID1`, `RewardFactionValue1`, `RewardFactionOverride1`,
       `RewardFactionID2`, `RewardFactionValue2`, `RewardFactionOverride2`,
       `RewardFactionID3`, `RewardFactionValue3`, `RewardFactionOverride3`,
       `RewardFactionID4`, `RewardFactionValue4`, `RewardFactionOverride4`,
       `RewardFactionID5`, `RewardFactionValue5`, `RewardFactionOverride5`,
       `RewardFactionFlags`,
       `RewardCurrencyID1`, `RewardCurrencyQty1`,
       `RewardCurrencyID2`, `RewardCurrencyQty2`,
       `RewardCurrencyID3`, `RewardCurrencyQty3`,
       `RewardCurrencyID4`, `RewardCurrencyQty4`,
       `AcceptedSoundKitID`, `CompleteSoundKitID`, `AreaGroupID`, `TimeAllowed`,
       `AllowableRaces`, `LogTitle`, `LogDescription`, `QuestDescription`,
       `AreaDescription`, `PortraitGiverText`, `PortraitGiverName`,
       `PortraitTurnInText`, `PortraitTurnInName`, `QuestCompletionLog`, `VerifiedBuild`
FROM `quest_template` WHERE `ID` = 31904;

-- 3b: Quest template addon (PrevQuestID=31918 for Horde duplicates)
REPLACE INTO `quest_template_addon` (`ID`, `PrevQuestID`, `NextQuestID`, `SpecialFlags`) VALUES
(33009, 31918, 0, 1),
(33010, 31918, 0, 1),
(33011, 31918, 0, 1),
(33012, 31918, 0, 1),
(33013, 31918, 0, 1);

-- 3c: Quest objectives (type=11 WINPETBATTLEAGAINSTNPC)
REPLACE INTO `quest_objective` (`questId`, `index`, `type`, `objectId`, `amount`, `flags`, `description`) VALUES
(33009, 0, 11, 66135, 1, 1, 'Defeat Dagra the Fierce'),
(33010, 0, 11, 66136, 1, 1, 'Defeat Analynn'),
(33011, 0, 11, 66137, 1, 1, 'Defeat Zonya the Sadist'),
(33012, 0, 11, 66372, 1, 1, 'Defeat Merda Stronghoof'),
(33013, 1, 11, 66422, 1, 0, 'Defeat Cassandra Kaboom');

-- 3d: Quest offer reward text (copied from originals)
REPLACE INTO `quest_offer_reward` (`ID`, `Emote1`, `Emote2`, `Emote3`, `Emote4`, `EmoteDelay1`, `EmoteDelay2`, `EmoteDelay3`, `EmoteDelay4`, `RewardText`, `VerifiedBuild`) VALUES
(33009, 0, 0, 0, 0, 0, 0, 0, 0, 'Impossible!', 18414),
(33010, 0, 0, 0, 0, 0, 0, 0, 0, 'Your pets are ecstatic about their victory. I hope you are as well.', 18414),
(33011, 0, 0, 0, 0, 0, 0, 0, 0, 'You think yourself victorious, but I be growin'' stronger with each defeat.', 18414),
(33012, 0, 0, 0, 0, 0, 0, 0, 0, 'Your victory is well-deserved. Your skills are superior to my own.', 18414),
(33013, 0, 0, 0, 0, 0, 0, 0, 0, 'I must''ve converted a line wrong somewhere...', 18414);

-- 3e: Creature quest links (self-referential daily — trainer gives and receives)
REPLACE INTO `creature_queststarter` (`id`, `quest`) VALUES
(66135, 33009),   -- Dagra the Fierce
(66136, 33010),   -- Analynn
(66137, 33011),   -- Zonya the Sadist
(66372, 33012),   -- Merda Stronghoof
(66422, 33013);   -- Cassandra Kaboom

REPLACE INTO `creature_questender` (`id`, `quest`) VALUES
(66135, 33009),   -- Dagra the Fierce
(66136, 33010),   -- Analynn
(66137, 33011),   -- Zonya the Sadist
(66372, 33012),   -- Merda Stronghoof
(66422, 33013);   -- Cassandra Kaboom

-- ===========================================================================
-- Phase 5: Eastern Kingdoms completion quest fixes (Issue 2)
--
-- 31903 (Battle Pet Tamers: Eastern Kingdoms) was Horde-only with no
-- quest starter or ender. Fix:
--   1. Add Varzok as starter/ender for existing Horde quest 31903
--   2. Create Alliance version 33014 (Audrey Burnhep as ender)
--
-- Also fix Issue 3: 31891 (Horde Kalimdor completion) missing ender
-- ===========================================================================

-- 5a: Horde EK completion (31903) — Varzok as quest giver and turn-in
REPLACE INTO `creature_queststarter` (`id`, `quest`) VALUES (63626, 31903);
REPLACE INTO `creature_questender` (`id`, `quest`) VALUES (63626, 31903);

-- 5b: Horde Kalimdor completion (31891) — Master Hight as turn-in
REPLACE INTO `creature_questender` (`id`, `quest`) VALUES (66260, 31891);

-- 5c: Create Alliance EK completion quest (33014)
-- Copy from Horde version (31903) but change AllowableRaces=18875469 (Alliance)
REPLACE INTO `quest_template`
SELECT 33014, `QuestType`, `QuestLevel`, `MinLevel`, `QuestPackageID`, `QuestSortID`,
       `QuestInfoID`, `SuggestedGroupNum`, `RewardNextQuest`, `RewardXPDifficulty`,
       `RewardMoney`, `RewardBonusMoney`, `RewardDisplaySpell`, `RewardSpell`,
       `RewardHonor`, `RewardKillHonor`, `StartItem`, `Flags`, `FlagsEx`,
       `RewardItem1`, `RewardAmount1`, `RewardItem2`, `RewardAmount2`,
       `RewardItem3`, `RewardAmount3`, `RewardItem4`, `RewardAmount4`,
       `ItemDrop1`, `ItemDropQuantity1`, `ItemDrop2`, `ItemDropQuantity2`,
       `ItemDrop3`, `ItemDropQuantity3`, `ItemDrop4`, `ItemDropQuantity4`,
       `RewardChoiceItemID1`, `RewardChoiceItemQuantity1`,
       `RewardChoiceItemID2`, `RewardChoiceItemQuantity2`,
       `RewardChoiceItemID3`, `RewardChoiceItemQuantity3`,
       `RewardChoiceItemID4`, `RewardChoiceItemQuantity4`,
       `RewardChoiceItemID5`, `RewardChoiceItemQuantity5`,
       `RewardChoiceItemID6`, `RewardChoiceItemQuantity6`,
       `POIContinent`, `POIx`, `POIy`, `POIPriority`,
       `RewardTitle`, `RewardArenaPoints`, `RewardSkillLineID`, `RewardNumSkillUps`,
       `PortraitGiver`, `PortraitTurnIn`,
       `RewardFactionID1`, `RewardFactionValue1`, `RewardFactionOverride1`,
       `RewardFactionID2`, `RewardFactionValue2`, `RewardFactionOverride2`,
       `RewardFactionID3`, `RewardFactionValue3`, `RewardFactionOverride3`,
       `RewardFactionID4`, `RewardFactionValue4`, `RewardFactionOverride4`,
       `RewardFactionID5`, `RewardFactionValue5`, `RewardFactionOverride5`,
       `RewardFactionFlags`,
       `RewardCurrencyID1`, `RewardCurrencyQty1`,
       `RewardCurrencyID2`, `RewardCurrencyQty2`,
       `RewardCurrencyID3`, `RewardCurrencyQty3`,
       `RewardCurrencyID4`, `RewardCurrencyQty4`,
       `AcceptedSoundKitID`, `CompleteSoundKitID`, `AreaGroupID`, `TimeAllowed`,
       18875469, `LogTitle`, `LogDescription`, `QuestDescription`,
       `AreaDescription`, `PortraitGiverText`, `PortraitGiverName`,
       `PortraitTurnInText`, `PortraitTurnInName`, `QuestCompletionLog`, `VerifiedBuild`
FROM `quest_template` WHERE `ID` = 31903;

-- 5d: Quest template addon for Alliance EK (gated behind 31917)
REPLACE INTO `quest_template_addon` (`ID`, `PrevQuestID`, `NextQuestID`, `SpecialFlags`) VALUES
(33014, 31917, 31915, 0);

-- 5e: Quest objectives (same 5 EK tamers as 31903)
REPLACE INTO `quest_objective` (`questId`, `index`, `type`, `objectId`, `amount`, `flags`, `description`) VALUES
(33014, 5, 11, 66478, 1, 0, 'Defeat David Kosse'),
(33014, 6, 11, 66512, 1, 0, 'Defeat Deiza Plaguehorn'),
(33014, 7, 11, 66515, 1, 0, 'Defeat Kortas Darkhammer'),
(33014, 8, 11, 66518, 1, 0, 'Defeat Everessa'),
(33014, 9, 11, 66520, 1, 0, 'Defeat Durin Darkhammer');

-- 5f: Quest offer reward (Audrey Burnhep turn-in text)
REPLACE INTO `quest_offer_reward` (`ID`, `Emote1`, `Emote2`, `Emote3`, `Emote4`, `EmoteDelay1`, `EmoteDelay2`, `EmoteDelay3`, `EmoteDelay4`, `RewardText`, `VerifiedBuild`) VALUES
(33014, 0, 0, 0, 0, 0, 0, 0, 0, 'Well done, $n. You''ve proven yourself against the tamers of the Eastern Kingdoms. Now it''s time to face Lydia Accoste.', 18414);

-- 5g: Creature quest ender (Audrey Burnhep accepts turn-in)
REPLACE INTO `creature_questender` (`id`, `quest`) VALUES (63596, 33014);

-- ===========================================================================
-- Phase 6: Zone completion quest gating (Step 5)
--
-- Zone completion quests (Kalimdor + EK) should be gated behind their
-- faction's "A Tamer's Homecoming" (31917 Alliance / 31918 Horde).
-- Prevents skipping directly to Grand Masters without completing the
-- local trainer chain first.
-- ===========================================================================

UPDATE `quest_template_addon` SET `PrevQuestID` = 31917 WHERE `ID` = 31889;
UPDATE `quest_template_addon` SET `PrevQuestID` = 31918 WHERE `ID` IN (31891, 31903);
