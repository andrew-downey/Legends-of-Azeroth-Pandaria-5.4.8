-- Kalimdor wild battle pets batch
-- Type-8 replacement zones (existing critters, names match species)
-- Type-14 direct spawns (unique pets with no matching critter)

-- ===================================================================
-- Part A: Type-8 pool entries — simple critter replacement
-- ===================================================================

-- Thunder Bluff (1638): Prairie Dog (species 386, critter 2620)
INSERT INTO battle_pet_wild_pool (zone, species, entry, max, minLevel, maxLevel, comment) VALUES
(1638, 386, 2620, 2, 1, 3, 'Prairie Dog - Thunder Bluff');

-- Ahn'Qiraj: The Fallen Kingdom (5695): Scarab Hatchling + Sidewinder
-- Both have existing creature spawns in the zone (49746 x4, 49727 x10)
INSERT INTO battle_pet_wild_pool (zone, species, entry, max, minLevel, maxLevel, comment) VALUES
(5695, 512, 49746, 2, 16, 17, 'Scarab Hatchling - Ahn''Qiraj: The Fallen Kingdom'),
(5695, 511, 49727, 3, 16, 17, 'Sidewinder - Ahn''Qiraj: The Fallen Kingdom');

-- Darkmoon Island (5861): Sea Gull + Crow (type-8, existing critters)
INSERT INTO battle_pet_wild_pool (zone, species, entry, max, minLevel, maxLevel, comment) VALUES
(5861, 560, 55749, 3, 1, 10, 'Sea Gull - Darkmoon Island'),
(5861, 1068, 55741, 3, 1, 10, 'Crow - Darkmoon Island');

-- ===================================================================
-- Part B: Darkmoon Glowfly — type-14 direct spawn (no matching critter)
-- Species 1062, NpcId 67329
-- Coordinates from Moonweb Spider (55487) spawns in Darkmoon Island (map 974)
-- ===================================================================

INSERT INTO creature (id, map, zoneId, areaId, spawnMask, phaseMask, position_x, position_y, position_z, orientation, spawntimesecs, wander_distance, MovementType, VerifiedBuild) VALUES
(67329, 974, 5861, 5870, 1, 1, -4282.13, 6268.06, 13.225, 4.71239, 120, 0, 0, 0),
(67329, 974, 5861, 5861, 1, 1, -4286.64, 6225.49, 18.4168, 3.14159, 120, 0, 0, 0),
(67329, 974, 5861, 5870, 1, 1, -4306.70, 6167.31, 13.2334, 0.00000, 120, 0, 0, 0);

INSERT INTO battle_pet_wild_pool (zone, species, entry, max, minLevel, maxLevel, comment) VALUES
(5861, 1062, 67329, 2, 1, 10, 'Darkmoon Glowfly - Darkmoon Island');

-- ===================================================================
-- Part C: Searing Scorchling — type-14 spawn via Death's Head Cockroach (49759)
-- Species 755, NpcId 62887 (replaces type-8 critter 49759 like Mount Hyjal pattern)
-- Coordinates from Cinderling (53697) spawns in Molten Front (map 861, area 5742)
-- ===================================================================

INSERT INTO creature (id, map, zoneId, areaId, spawnMask, phaseMask, position_x, position_y, position_z, orientation, spawntimesecs, wander_distance, MovementType, VerifiedBuild) VALUES
(49759, 861, 5733, 5742, 1, 1, 1177.81, 198.906, 54.6682, 2.09440, 120, 0, 0, 0),
(49759, 861, 5733, 5742, 1, 1, 1214.18, 227.902, 55.1440, 5.23599, 120, 0, 0, 0);

INSERT INTO battle_pet_wild_pool (zone, species, entry, max, minLevel, maxLevel, comment) VALUES
(5733, 755, 49759, 1, 24, 24, 'Searing Scorchling - Molten Front');
