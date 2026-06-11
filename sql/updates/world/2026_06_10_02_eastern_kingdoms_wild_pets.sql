-- Eastern Kingdoms wild battle pets batch
-- 4 zones, 30 pools (24 type-8, 6 type-14)

-- ===================================================================
-- Arathi Highlands (45) — 7 pools (4 type-8, 3 type-14)
-- ===================================================================

-- Type-8: Grasslands Cottontail (species 443, critter 49999)
-- Type-8: Prairie Dog (species 386, critter 2620)
-- Type-8: Rat (species 417, critter 4075)
-- Type-8: Small Frog (species 419, critter 13321)
INSERT INTO battle_pet_wild_pool (zone, species, entry, max, minLevel, maxLevel, comment) VALUES
(45, 443, 49999, 20, 7, 8, 'Grasslands Cottontail - Arathi Highlands'),
(45, 386, 2620, 30, 7, 8, 'Prairie Dog - Arathi Highlands'),
(45, 417, 4075, 20, 7, 8, 'Rat - Arathi Highlands'),
(45, 419, 13321, 15, 7, 8, 'Small Frog - Arathi Highlands');

-- Type-14: Tiny Twister (species 445, NpcId 61703)
-- No matching type-8 critter — added as direct spawn near Dabyrie's Farmstead
INSERT INTO creature (id, map, zoneId, areaId, spawnMask, phaseMask, position_x, position_y, position_z, orientation, spawntimesecs, wander_distance, MovementType, VerifiedBuild) VALUES
(61703, 0, 45, 314, 1, 1, -1340.25, -3050.31, 41.9413, 3.08295, 120, 0, 0, 0),
(61703, 0, 45, 314, 1, 1, -1336.76, -3050.71, 42.2529, 3.10450, 120, 0, 0, 0);

INSERT INTO battle_pet_wild_pool (zone, species, entry, max, minLevel, maxLevel, comment) VALUES
(45, 445, 61703, 2, 7, 8, 'Tiny Twister - Arathi Highlands');

-- Type-14: Hare (species 448, NpcId 61751)
-- No Hare critter spawns in zone — added near Hammerfall
INSERT INTO creature (id, map, zoneId, areaId, spawnMask, phaseMask, position_x, position_y, position_z, orientation, spawntimesecs, wander_distance, MovementType, VerifiedBuild) VALUES
(61751, 0, 45, 45, 1, 1, -728.912, -1791.07, 61.2834, 3.83826, 120, 0, 0, 0),
(61751, 0, 45, 45, 1, 1, -721.286, -1787.38, 61.6525, 0.97142, 120, 0, 0, 0);

INSERT INTO battle_pet_wild_pool (zone, species, entry, max, minLevel, maxLevel, comment) VALUES
(45, 448, 61751, 2, 7, 8, 'Hare - Arathi Highlands');

-- Type-14: Cat (species 459, NpcId 62019)
-- No Cat critter spawns in zone — added near Refuge Pointe area
INSERT INTO creature (id, map, zoneId, areaId, spawnMask, phaseMask, position_x, position_y, position_z, orientation, spawntimesecs, wander_distance, MovementType, VerifiedBuild) VALUES
(62019, 0, 45, 45, 1, 1, -1069.82, -2021.07, 67.7058, 3.30757, 120, 0, 0, 0),
(62019, 0, 45, 1858, 1, 1, -1082.74, -2021.16, 67.5808, 0.10958, 120, 0, 0, 0);

INSERT INTO battle_pet_wild_pool (zone, species, entry, max, minLevel, maxLevel, comment) VALUES
(45, 459, 62019, 2, 7, 8, 'Cat - Arathi Highlands');

-- ===================================================================
-- Northern Stranglethorn (33) — 10 pools (9 type-8, 1 type-14)
-- ===================================================================

-- Type-8: Beetle (species 406, critter 15475)
-- Type-8: Crimson Moth (species 421, critter 49928)
-- Type-8: Forest Spiderling (species 407, critter 50487)
-- Type-8: Lizard Hatchling (species 408, critter 50479)
-- Type-8: Long-tailed Mole (species 404, critter 48972)
-- Type-8: Parrot (species 403, critter 9600)
-- Type-8: Roach (species 424, critter 4076)
-- Type-8: Strand Crab (species 401, critter 6827)
-- Type-8: Tree Python (species 405, critter 49722)
INSERT INTO battle_pet_wild_pool (zone, species, entry, max, minLevel, maxLevel, comment) VALUES
(33, 406, 15475, 10, 7, 9, 'Beetle - Northern Stranglethorn'),
(33, 421, 49928, 8, 7, 9, 'Crimson Moth - Northern Stranglethorn'),
(33, 407, 50487, 10, 7, 9, 'Forest Spiderling - Northern Stranglethorn'),
(33, 408, 50479, 10, 7, 9, 'Lizard Hatchling - Northern Stranglethorn'),
(33, 404, 48972, 3, 7, 9, 'Long-tailed Mole - Northern Stranglethorn'),
(33, 403, 9600, 6, 7, 9, 'Parrot - Northern Stranglethorn'),
(33, 424, 4076, 5, 7, 9, 'Roach - Northern Stranglethorn'),
(33, 401, 6827, 12, 7, 9, 'Strand Crab - Northern Stranglethorn'),
(33, 405, 49722, 12, 7, 9, 'Tree Python - Northern Stranglethorn');

-- Type-14: Polly (species 409, NpcId 61322)
-- No type-8 Polly critter — added near Nek'mani Wellspring (area 100)
INSERT INTO creature (id, map, zoneId, areaId, spawnMask, phaseMask, position_x, position_y, position_z, orientation, spawntimesecs, wander_distance, MovementType, VerifiedBuild) VALUES
(61322, 0, 33, 100, 1, 1, -11638.1, -154.819, 15.6580, 3.20711, 120, 0, 0, 0),
(61322, 0, 33, 104, 1, 1, -11610.5, -28.3361, 12.2466, 3.72526, 120, 0, 0, 0);

INSERT INTO battle_pet_wild_pool (zone, species, entry, max, minLevel, maxLevel, comment) VALUES
(33, 409, 61322, 2, 7, 9, 'Polly - Northern Stranglethorn');

-- ===================================================================
-- Deadwind Pass (41) — 2 pools (both type-14)
-- ===================================================================

-- Type-14: Restless Shadeling (species 439, NpcId 61375)
-- Master's Cellar under Karazhan — time-limited spawn (early morning)
INSERT INTO creature (id, map, zoneId, areaId, spawnMask, phaseMask, position_x, position_y, position_z, orientation, spawntimesecs, wander_distance, MovementType, VerifiedBuild) VALUES
(61375, 0, 41, 2837, 1, 1, -11162.2, -1904.72, -17.7047, 1.22173, 120, 0, 0, 0),
(61375, 0, 41, 2837, 1, 1, -11169.0, -1926.59, -17.6768, 4.74729, 120, 0, 0, 0);

INSERT INTO battle_pet_wild_pool (zone, species, entry, max, minLevel, maxLevel, comment) VALUES
(41, 439, 61375, 2, 17, 18, 'Restless Shadeling - Deadwind Pass');

-- Type-14: Arcane Eye (species 1160, NpcId 68819)
-- Near Karazhan grounds
INSERT INTO creature (id, map, zoneId, areaId, spawnMask, phaseMask, position_x, position_y, position_z, orientation, spawntimesecs, wander_distance, MovementType, VerifiedBuild) VALUES
(68819, 0, 41, 2562, 1, 1, -11123.3, -2006.77, 47.2725, 4.46804, 120, 0, 0, 0),
(68819, 0, 41, 2938, 1, 1, -10648.3, -2094.00, 107.156, 4.78766, 120, 0, 0, 0);

INSERT INTO battle_pet_wild_pool (zone, species, entry, max, minLevel, maxLevel, comment) VALUES
(41, 1160, 68819, 2, 17, 18, 'Arcane Eye - Deadwind Pass');

-- ===================================================================
-- The Cape of Stranglethorn (5287) — 11 pools (10 type-8, 1 type-14)
-- ===================================================================

-- Type-8: Beetle (species 406, critter 15475)
-- Type-8: Crimson Moth (species 421, critter 49928)
-- Type-8: Forest Spiderling (species 407, critter 50487)
-- Type-8: Lizard Hatchling (species 408, critter 50479)
-- Type-8: Long-tailed Mole (species 404, critter 48972)
-- Type-8: Parrot (species 403, critter 9600)
-- Type-8: Rat (species 417, critter 4075)
-- Type-8: Roach (species 424, critter 4076)
-- Type-8: Strand Crab (species 401, critter 6827)
-- Type-8: Tree Python (species 405, critter 49722)
-- Type-8: Wharf Rat (species 410, critter 47667)
INSERT INTO battle_pet_wild_pool (zone, species, entry, max, minLevel, maxLevel, comment) VALUES
(5287, 406, 15475, 10, 9, 10, 'Beetle - The Cape of Stranglethorn'),
(5287, 421, 49928, 10, 9, 10, 'Crimson Moth - The Cape of Stranglethorn'),
(5287, 407, 50487, 5, 9, 10, 'Forest Spiderling - The Cape of Stranglethorn'),
(5287, 408, 50479, 7, 9, 10, 'Lizard Hatchling - The Cape of Stranglethorn'),
(5287, 404, 48972, 8, 9, 10, 'Long-tailed Mole - The Cape of Stranglethorn'),
(5287, 403, 9600, 12, 9, 10, 'Parrot - The Cape of Stranglethorn'),
(5287, 417, 4075, 1, 9, 10, 'Rat - The Cape of Stranglethorn'),
(5287, 424, 4076, 10, 9, 10, 'Roach - The Cape of Stranglethorn'),
(5287, 401, 6827, 25, 9, 10, 'Strand Crab - The Cape of Stranglethorn'),
(5287, 405, 49722, 8, 9, 10, 'Tree Python - The Cape of Stranglethorn'),
(5287, 410, 47667, 2, 9, 10, 'Wharf Rat - The Cape of Stranglethorn');

-- Type-14: Baby Ape (species 411, NpcId 61324)
-- Jaguero Isle (area 1737) — rain-only spawn. Coordinates from Elder Mistvale Gorilla (1557)
INSERT INTO creature (id, map, zoneId, areaId, spawnMask, phaseMask, position_x, position_y, position_z, orientation, spawntimesecs, wander_distance, MovementType, VerifiedBuild) VALUES
(61324, 0, 5287, 1737, 1, 1, -14017.0, 76.5086, 9.93680, 3.14598, 120, 0, 0, 0),
(61324, 0, 5287, 1737, 1, 1, -14037.1, 122.655, 10.7904, 0.13526, 120, 0, 0, 0);

INSERT INTO battle_pet_wild_pool (zone, species, entry, max, minLevel, maxLevel, comment) VALUES
(5287, 411, 61324, 2, 9, 10, 'Baby Ape - The Cape of Stranglethorn');
