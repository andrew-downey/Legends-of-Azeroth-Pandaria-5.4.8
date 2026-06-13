-- Deepholm (zone 6400, map 646) wild battle pets — 11 species
-- Pet battle level range: 22-24 (zone 30-35)
-- 10 type-8 replacement pools + 1 type-14 direct spawn (Crimson Geode)

-- ===================================================================
-- Part A: Type-8 pool entries — existing critter replacement
-- 10 species with abundant critter spawns already in map 646
-- ===================================================================

INSERT INTO battle_pet_wild_pool (zone, species, entry, max, minLevel, maxLevel, comment) VALUES
(6400, 556, 49771, 30, 22, 24, 'Crystal Beetle - Deepholm'),
(6400, 555, 49770, 30, 22, 24, 'Deepholm Cockroach - Deepholm'),
(6400, 554, 49929, 18, 22, 24, 'Crimson Shale Hatchling - Deepholm'),
(6400, 837, 49857, 18, 22, 24, 'Emerald Shale Hatchling - Deepholm'),
(6400, 838, 49858, 25, 22, 24, 'Amethyst Shale Hatchling - Deepholm'),
(6400, 480, 49859, 15, 22, 24, 'Topaz Shale Hatchling - Deepholm'),
(6400, 756, 49847, 15, 22, 24, 'Fungal Moth - Deepholm'),
(6400, 553, 49758, 3, 22, 24, 'Stowaway Rat - Deepholm'),
(6400, 469, 49861, 6, 22, 24, 'Twilight Beetle - Deepholm'),
(6400, 470, 48692, 8, 22, 24, 'Twilight Spider - Deepholm');

-- ===================================================================
-- Part B: Crimson Geode — type-14 direct spawn (no matching critter)
-- Species 559, NpcId 62927
-- Located in Crimson Expanse (areaId 5291) in Deepholm
-- Coordinates from nearby Crimson Shale Hatchling (49929) spawns
-- ===================================================================

INSERT INTO creature (id, map, zoneId, areaId, spawnMask, phaseMask, position_x, position_y, position_z, orientation, spawntimesecs, wander_distance, MovementType, VerifiedBuild) VALUES
(62927, 646, 6400, 5291, 1, 1, 1243.0, 260.0, -73.0, 3.14159, 300, 5, 1, 0),
(62927, 646, 6400, 5291, 1, 1, 1317.0, 353.0, -73.0, 4.71239, 300, 5, 1, 0),
(62927, 646, 6400, 5291, 1, 1, 1195.0, 461.0, -73.0, 0.00000, 300, 5, 1, 0);

INSERT INTO battle_pet_wild_pool (zone, species, entry, max, minLevel, maxLevel, comment) VALUES
(6400, 559, 62927, 2, 22, 24, 'Crimson Geode - Deepholm');
