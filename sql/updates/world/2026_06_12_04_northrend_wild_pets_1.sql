-- Northrend wild battle pets -- Part 1: Borean Tundra + Howling Fjord + Dragonblight
-- All zones on map 571
-- Pet level ranges per zone: Borean Tundra 20-22, Howling Fjord 20-22, Dragonblight 22-23

-- ===================================================================
-- Part A: New type-8 pool entry -- Howling Fjord Snake
-- Species 387, critter 2914 (5 spawns in zone 495)
-- ===================================================================

INSERT INTO battle_pet_wild_pool (zone, species, entry, max, minLevel, maxLevel, comment) VALUES
(495, 387, 2914, 3, 20, 22, 'Snake - Howling Fjord');

-- ===================================================================
-- Part B: Fix Dragonblight Tundra Penguin -- broken type-8 -> type-14
-- Existing entry (65, 536, 28440, 1, 22, 23) references critter 28440 with 0 spawns in zone 65
-- REPLACE to use type-14 NPC 62835 directly
-- ===================================================================

REPLACE INTO battle_pet_wild_pool (zone, species, entry, max, minLevel, maxLevel, comment) VALUES
(65, 536, 62835, 3, 22, 23, 'Tundra Penguin - Dragonblight');

INSERT INTO creature (id, map, zoneId, areaId, spawnMask, phaseMask, position_x, position_y, position_z, orientation, spawntimesecs, wander_distance, MovementType, VerifiedBuild) VALUES
(62835, 571, 65, 65, 1, 1, 3743.89, 1690.83, 120.87, 3.14159, 300, 5, 1, 0),
(62835, 571, 65, 65, 1, 1, 3504.32, 1989.59, 65.13, 2.09440, 300, 5, 1, 0),
(62835, 571, 65, 65, 1, 1, 3624.28, 1039.60, 58.24, 5.49779, 300, 5, 1, 0);

-- ===================================================================
-- Part C: Borean Tundra type-14 spawns (zone 3537, pet level 20-22)
-- 3 unique pets + Unborn Val'kyr
-- ===================================================================

-- Nexus Whelpling (species 1165, NPC 68845) -- Coldarra (areaId 4024)
INSERT INTO creature (id, map, zoneId, areaId, spawnMask, phaseMask, position_x, position_y, position_z, orientation, spawntimesecs, wander_distance, MovementType, VerifiedBuild) VALUES
(68845, 571, 3537, 4024, 1, 1, 3829.31, 6751.31, 200.00, 1.23456, 300, 8, 1, 0),
(68845, 571, 3537, 4024, 1, 1, 4043.78, 6982.24, 150.00, 4.56789, 300, 8, 1, 0),
(68845, 571, 3537, 4024, 1, 1, 3750.00, 6950.00, 400.00, 3.14159, 300, 8, 1, 0);

INSERT INTO battle_pet_wild_pool (zone, species, entry, max, minLevel, maxLevel, comment) VALUES
(3537, 1165, 68845, 2, 20, 22, 'Nexus Whelpling - Borean Tundra');

-- Oily Slimeling (species 530, NPC 62697) -- Oil Pools (areaId 4116) / Steam Vents (areaId 4117)
INSERT INTO creature (id, map, zoneId, areaId, spawnMask, phaseMask, position_x, position_y, position_z, orientation, spawntimesecs, wander_distance, MovementType, VerifiedBuild) VALUES
(62697, 571, 3537, 4116, 1, 1, 3396.37, 5168.51, 13.04, 2.71828, 300, 5, 1, 0),
(62697, 571, 3537, 4116, 1, 1, 3846.86, 5214.03, 13.04, 0.78540, 300, 5, 1, 0),
(62697, 571, 3537, 4117, 1, 1, 3571.56, 4299.47, 12.71, 5.93412, 300, 5, 1, 0);

INSERT INTO battle_pet_wild_pool (zone, species, entry, max, minLevel, maxLevel, comment) VALUES
(3537, 530, 62697, 2, 20, 22, 'Oily Slimeling - Borean Tundra');

-- Unborn Val'kyr (species 1238, NPC 71163) -- Borean Tundra
INSERT INTO creature (id, map, zoneId, areaId, spawnMask, phaseMask, position_x, position_y, position_z, orientation, spawntimesecs, wander_distance, MovementType, VerifiedBuild) VALUES
(71163, 571, 3537, 3537, 1, 1, 3800.00, 4300.00, 15.00, 4.71239, 300, 10, 1, 0),
(71163, 571, 3537, 3537, 1, 1, 3500.00, 2000.00, 25.00, 1.57080, 300, 10, 1, 0);

INSERT INTO battle_pet_wild_pool (zone, species, entry, max, minLevel, maxLevel, comment) VALUES
(3537, 1238, 71163, 2, 20, 22, 'Unborn Val''kyr - Borean Tundra');

-- ===================================================================
-- Part D: Howling Fjord type-14 spawns (zone 495, pet level 20-22)
-- Unborn Val'kyr only -- other pets use type-8
-- ===================================================================

-- Unborn Val'kyr (species 1238, NPC 71163) -- Howling Fjord
INSERT INTO creature (id, map, zoneId, areaId, spawnMask, phaseMask, position_x, position_y, position_z, orientation, spawntimesecs, wander_distance, MovementType, VerifiedBuild) VALUES
(71163, 571, 495, 3982, 1, 1, 1000.00, -4900.00, 15.00, 0.00000, 300, 10, 1, 0),
(71163, 571, 495, 3982, 1, 1, 930.12, -4906.55, 7.00, 3.14159, 300, 10, 1, 0);

INSERT INTO battle_pet_wild_pool (zone, species, entry, max, minLevel, maxLevel, comment) VALUES
(495, 1238, 71163, 2, 20, 22, 'Unborn Val''kyr - Howling Fjord');

-- ===================================================================
-- Part E: Dragonblight type-14 spawns (zone 65, pet level 22-23)
-- Dragonbone Hatchling + Unborn Val'kyr
-- ===================================================================

-- Dragonbone Hatchling (species 537, NPC 62852) -- near Wyrmrest Temple + Galakrond's Rest
INSERT INTO creature (id, map, zoneId, areaId, spawnMask, phaseMask, position_x, position_y, position_z, orientation, spawntimesecs, wander_distance, MovementType, VerifiedBuild) VALUES
(62852, 571, 65, 4168, 1, 1, 3624.28, 1039.60, 58.24, 0.78540, 300, 5, 1, 0),
(62852, 571, 65, 4173, 1, 1, 4220.84, 546.18, 11.92, 2.09440, 300, 5, 1, 0),
(62852, 571, 65, 65, 1, 1, 3450.00, 1800.00, 65.00, 5.49779, 300, 5, 1, 0);

INSERT INTO battle_pet_wild_pool (zone, species, entry, max, minLevel, maxLevel, comment) VALUES
(65, 537, 62852, 2, 22, 23, 'Dragonbone Hatchling - Dragonblight');

-- Unborn Val'kyr (species 1238, NPC 71163) -- Dragonblight
INSERT INTO creature (id, map, zoneId, areaId, spawnMask, phaseMask, position_x, position_y, position_z, orientation, spawntimesecs, wander_distance, MovementType, VerifiedBuild) VALUES
(71163, 571, 65, 65, 1, 1, 3500.00, 1600.00, 100.00, 2.35619, 300, 10, 1, 0),
(71163, 571, 65, 65, 1, 1, 3800.00, 1200.00, 80.00, 5.75959, 300, 10, 1, 0);

INSERT INTO battle_pet_wild_pool (zone, species, entry, max, minLevel, maxLevel, comment) VALUES
(65, 1238, 71163, 2, 22, 23, 'Unborn Val''kyr - Dragonblight');
