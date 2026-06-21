-- Northrend wild battle pets -- Part 2: Grizzly Hills + Sholazar Basin + Zul'Drak + Crystalsong Forest + Icecrown + The Storm Peaks
-- All zones on map 571
-- Pet level ranges per zone: Grizzly Hills 21-22, Sholazar Basin 21-22, Zul'Drak 22-23,
--   Crystalsong Forest 22-23, Icecrown 22-23, The Storm Peaks 22-23

-- ===================================================================
-- Part A: Grizzly Hills (zone 394, pet level 21-22)
-- Imperial Eagle Chick + Unborn Val'kyr
-- ===================================================================

-- Imperial Eagle Chick (species 534, NPC 62819) -- Grizzlemaw area (areaId 394)
INSERT INTO creature (id, map, zoneId, areaId, spawnMask, phaseMask, position_x, position_y, position_z, orientation, spawntimesecs, wander_distance, MovementType, VerifiedBuild) VALUES
(62819, 571, 394, 394, 1, 1, 3514.03, -3129.65, 244.62, 4.71239, 300, 8, 1, 0),
(62819, 571, 394, 394, 1, 1, 3650.00, -2950.00, 240.00, 2.09440, 300, 8, 1, 0),
(62819, 571, 394, 394, 1, 1, 3695.14, -3481.35, 246.42, 5.49779, 300, 8, 1, 0);

INSERT INTO battle_pet_wild_pool (zone, species, entry, max, minLevel, maxLevel, comment) VALUES
(394, 534, 62819, 2, 21, 22, 'Imperial Eagle Chick - Grizzly Hills');

-- Unborn Val'kyr (species 1238, NPC 71163) -- Grizzly Hills
INSERT INTO creature (id, map, zoneId, areaId, spawnMask, phaseMask, position_x, position_y, position_z, orientation, spawntimesecs, wander_distance, MovementType, VerifiedBuild) VALUES
(71163, 571, 394, 394, 1, 1, 3500.00, -3100.00, 240.00, 3.14159, 300, 10, 1, 0),
(71163, 571, 394, 394, 1, 1, 3800.00, -2700.00, 260.00, 1.57080, 300, 10, 1, 0);

INSERT INTO battle_pet_wild_pool (zone, species, entry, max, minLevel, maxLevel, comment) VALUES
(394, 1238, 71163, 2, 21, 22, 'Unborn Val''kyr - Grizzly Hills');

-- ===================================================================
-- Part B: Sholazar Basin (zone 3711, pet level 21-22)
-- Stunted Shardhorn + Unborn Val'kyr
-- ===================================================================

-- Stunted Shardhorn (species 532, NPC 62816) -- Nesingwary Base Camp (areaId 4284)
INSERT INTO creature (id, map, zoneId, areaId, spawnMask, phaseMask, position_x, position_y, position_z, orientation, spawntimesecs, wander_distance, MovementType, VerifiedBuild) VALUES
(62816, 571, 3711, 4284, 1, 1, 5584.46, 5749.00, -72.01, 0.78540, 300, 8, 1, 0),
(62816, 571, 3711, 3711, 1, 1, 5700.00, 5730.00, -75.00, 2.35619, 300, 8, 1, 0),
(62816, 571, 3711, 3711, 1, 1, 5713.79, 5717.34, -75.81, 3.92699, 300, 8, 1, 0);

INSERT INTO battle_pet_wild_pool (zone, species, entry, max, minLevel, maxLevel, comment) VALUES
(3711, 532, 62816, 2, 21, 22, 'Stunted Shardhorn - Sholazar Basin');

-- Unborn Val'kyr (species 1238, NPC 71163) -- Sholazar Basin
INSERT INTO creature (id, map, zoneId, areaId, spawnMask, phaseMask, position_x, position_y, position_z, orientation, spawntimesecs, wander_distance, MovementType, VerifiedBuild) VALUES
(71163, 571, 3711, 3711, 1, 1, 5600.00, 5800.00, -70.00, 0.00000, 300, 10, 1, 0),
(71163, 571, 3711, 3711, 1, 1, 5400.00, 5500.00, -70.00, 4.71239, 300, 10, 1, 0);

INSERT INTO battle_pet_wild_pool (zone, species, entry, max, minLevel, maxLevel, comment) VALUES
(3711, 1238, 71163, 2, 21, 22, 'Unborn Val''kyr - Sholazar Basin');

-- ===================================================================
-- Part C: Zul'Drak (zone 66, pet level 22-23)
-- Water Waveling + Snake + Spider + Unborn Val'kyr
-- ===================================================================

-- Water Waveling (species 535, NPC 62820) -- central water areas (areaId 4320/66)
INSERT INTO creature (id, map, zoneId, areaId, spawnMask, phaseMask, position_x, position_y, position_z, orientation, spawntimesecs, wander_distance, MovementType, VerifiedBuild) VALUES
(62820, 571, 66, 66, 1, 1, 4883.78, -2902.88, 289.16, 1.23456, 300, 5, 1, 0),
(62820, 571, 66, 66, 1, 1, 4903.34, -2924.68, 289.76, 4.56789, 300, 5, 1, 0),
(62820, 571, 66, 4320, 1, 1, 5705.29, -2986.61, 300.20, 2.98765, 300, 5, 1, 0);

INSERT INTO battle_pet_wild_pool (zone, species, entry, max, minLevel, maxLevel, comment) VALUES
(66, 535, 62820, 2, 22, 23, 'Water Waveling - Zul''Drak');

-- Snake (species 387, NPC 61142) -- Drakkari jungle area (areaId 4276)
INSERT INTO creature (id, map, zoneId, areaId, spawnMask, phaseMask, position_x, position_y, position_z, orientation, spawntimesecs, wander_distance, MovementType, VerifiedBuild) VALUES
(61142, 571, 66, 4276, 1, 1, 6350.25, -2602.01, 318.54, 5.65487, 300, 5, 1, 0),
(61142, 571, 66, 4276, 1, 1, 6348.68, -2623.54, 318.53, 3.14159, 300, 5, 1, 0),
(61142, 571, 66, 66, 1, 1, 5400.00, -3700.00, 360.00, 2.09440, 300, 5, 1, 0);

INSERT INTO battle_pet_wild_pool (zone, species, entry, max, minLevel, maxLevel, comment) VALUES
(66, 387, 61142, 2, 22, 23, 'Snake - Zul''Drak');

-- Spider (species 412, NPC 61327) -- southern Zul'Drak (areaId 66)
INSERT INTO creature (id, map, zoneId, areaId, spawnMask, phaseMask, position_x, position_y, position_z, orientation, spawntimesecs, wander_distance, MovementType, VerifiedBuild) VALUES
(61327, 571, 66, 66, 1, 1, 6711.42, -4186.12, 456.85, 4.71239, 300, 5, 1, 0),
(61327, 571, 66, 66, 1, 1, 6417.33, -3658.22, 375.63, 1.57080, 300, 5, 1, 0),
(61327, 571, 66, 66, 1, 1, 5325.95, -4181.59, 362.38, 0.00000, 300, 5, 1, 0);

INSERT INTO battle_pet_wild_pool (zone, species, entry, max, minLevel, maxLevel, comment) VALUES
(66, 412, 61327, 2, 22, 23, 'Spider - Zul''Drak');

-- Unborn Val'kyr (species 1238, NPC 71163) -- Zul'Drak
INSERT INTO creature (id, map, zoneId, areaId, spawnMask, phaseMask, position_x, position_y, position_z, orientation, spawntimesecs, wander_distance, MovementType, VerifiedBuild) VALUES
(71163, 571, 66, 66, 1, 1, 5500.00, -3000.00, 300.00, 5.49779, 300, 10, 1, 0),
(71163, 571, 66, 66, 1, 1, 5300.00, -2800.00, 280.00, 2.35619, 300, 10, 1, 0);

INSERT INTO battle_pet_wild_pool (zone, species, entry, max, minLevel, maxLevel, comment) VALUES
(66, 1238, 71163, 2, 22, 23, 'Unborn Val''kyr - Zul''Drak');

-- ===================================================================
-- Part D: Crystalsong Forest (zone 2817, pet level 22-23)
-- Unborn Val'kyr only (no matching critters for type-8)
-- ===================================================================

-- Unborn Val'kyr (species 1238, NPC 71163) -- Crystalsong Forest
INSERT INTO creature (id, map, zoneId, areaId, spawnMask, phaseMask, position_x, position_y, position_z, orientation, spawntimesecs, wander_distance, MovementType, VerifiedBuild) VALUES
(71163, 571, 2817, 4553, 1, 1, 5500.00, 400.00, 155.00, 3.14159, 300, 10, 1, 0),
(71163, 571, 2817, 4553, 1, 1, 5700.00, 200.00, 160.00, 6.28319, 300, 10, 1, 0);

INSERT INTO battle_pet_wild_pool (zone, species, entry, max, minLevel, maxLevel, comment) VALUES
(2817, 1238, 71163, 2, 22, 23, 'Unborn Val''kyr - Crystalsong Forest');

-- ===================================================================
-- Part E: Icecrown (zone 210, pet level 22-23)
-- Scourged Whelpling + Unborn Val'kyr
-- ===================================================================

-- Scourged Whelpling (species 538, NPC 62854) -- near Icecrown Citadel (areaId 4510)
INSERT INTO creature (id, map, zoneId, areaId, spawnMask, phaseMask, position_x, position_y, position_z, orientation, spawntimesecs, wander_distance, MovementType, VerifiedBuild) VALUES
(62854, 571, 210, 4510, 1, 1, 7612.97, 2059.63, 506.25, 2.71828, 300, 5, 1, 0),
(62854, 571, 210, 4510, 1, 1, 7584.21, 2060.32, 507.02, 1.23456, 300, 5, 1, 0),
(62854, 571, 210, 210, 1, 1, 6850.00, 1650.00, 390.00, 4.56789, 300, 5, 1, 0);

INSERT INTO battle_pet_wild_pool (zone, species, entry, max, minLevel, maxLevel, comment) VALUES
(210, 538, 62854, 2, 22, 23, 'Scourged Whelpling - Icecrown');

-- Unborn Val'kyr (species 1238, NPC 71163) -- Icecrown
INSERT INTO creature (id, map, zoneId, areaId, spawnMask, phaseMask, position_x, position_y, position_z, orientation, spawntimesecs, wander_distance, MovementType, VerifiedBuild) VALUES
(71163, 571, 210, 210, 1, 1, 7000.00, 1700.00, 400.00, 4.71239, 300, 10, 1, 0),
(71163, 571, 210, 210, 1, 1, 7200.00, 1900.00, 450.00, 1.57080, 300, 10, 1, 0);

INSERT INTO battle_pet_wild_pool (zone, species, entry, max, minLevel, maxLevel, comment) VALUES
(210, 1238, 71163, 2, 22, 23, 'Unborn Val''kyr - Icecrown');

-- ===================================================================
-- Part F: The Storm Peaks (zone 67, pet level 22-23)
-- No type-8 critters exist in this zone -- all type-14 direct spawns
-- Arctic Hare + Spider + Unborn Val'kyr
-- ===================================================================

-- Arctic Hare (species 641, NPC 62693) -- snowy areas (areaId 4419)
INSERT INTO creature (id, map, zoneId, areaId, spawnMask, phaseMask, position_x, position_y, position_z, orientation, spawntimesecs, wander_distance, MovementType, VerifiedBuild) VALUES
(62693, 571, 67, 4419, 1, 1, 6373.12, -1191.06, 426.35, 2.09440, 300, 8, 1, 0),
(62693, 571, 67, 4419, 1, 1, 6100.00, -900.00, 390.00, 5.49779, 300, 8, 1, 0),
(62693, 571, 67, 67, 1, 1, 7029.20, -1392.19, 984.20, 3.14159, 300, 8, 1, 0);

INSERT INTO battle_pet_wild_pool (zone, species, entry, max, minLevel, maxLevel, comment) VALUES
(67, 641, 62693, 3, 22, 23, 'Arctic Hare - The Storm Peaks');

-- Spider (species 412, NPC 61327) -- rocky/cave areas (areaId 4418/67)
INSERT INTO creature (id, map, zoneId, areaId, spawnMask, phaseMask, position_x, position_y, position_z, orientation, spawntimesecs, wander_distance, MovementType, VerifiedBuild) VALUES
(61327, 571, 67, 4418, 1, 1, 6183.21, -1130.08, 414.46, 0.78540, 300, 5, 1, 0),
(61327, 571, 67, 4421, 1, 1, 6465.30, -1750.45, 432.43, 3.92699, 300, 5, 1, 0),
(61327, 571, 67, 4421, 1, 1, 6323.71, -1470.12, 427.24, 5.65487, 300, 5, 1, 0);

INSERT INTO battle_pet_wild_pool (zone, species, entry, max, minLevel, maxLevel, comment) VALUES
(67, 412, 61327, 2, 22, 23, 'Spider - The Storm Peaks');

-- Unborn Val'kyr (species 1238, NPC 71163) -- The Storm Peaks
INSERT INTO creature (id, map, zoneId, areaId, spawnMask, phaseMask, position_x, position_y, position_z, orientation, spawntimesecs, wander_distance, MovementType, VerifiedBuild) VALUES
(71163, 571, 67, 4424, 1, 1, 7350.00, 150.00, 780.00, 2.35619, 300, 10, 1, 0),
(71163, 571, 67, 67, 1, 1, 7200.00, 50.00, 800.00, 0.00000, 300, 10, 1, 0);

INSERT INTO battle_pet_wild_pool (zone, species, entry, max, minLevel, maxLevel, comment) VALUES
(67, 1238, 71163, 2, 22, 23, 'Unborn Val''kyr - The Storm Peaks');
