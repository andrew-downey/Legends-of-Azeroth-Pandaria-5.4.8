-- Uldum (zone 5034, map 1) wild battle pets -- 9 species
-- Pet battle level range: 23-24 (zone level 83-84)
-- 8 type-8 replacement pools + 1 type-14 direct spawn (Tol'vir Scarab)

-- ===================================================================
-- Part A: Type-8 pool entries -- existing critter replacement
-- 8 species with abundant critter spawns already in map 1, zone 5034
-- ===================================================================

REPLACE INTO battle_pet_wild_pool (zone, species, entry, max, minLevel, maxLevel, comment) VALUES
(5034, 467, 49743, 22, 23, 24, 'Dung Beetle - Uldum'),
(5034, 484, 49835, 22, 23, 24, 'Desert Spider - Uldum'),
(5034, 851, 49732, 18, 23, 24, 'Horned Lizard - Uldum'),
(5034, 545, 49832, 18, 23, 24, 'Leopard Scorpid - Uldum'),
(5034, 543, 50490, 20, 23, 24, 'Locust - Uldum'),
(5034, 542, 50491, 26, 23, 24, 'Mac Frog - Uldum'),
(5034, 544, 50496, 26, 23, 24, 'Oasis Moth - Uldum'),
(5034, 511, 49727, 18, 23, 24, 'Sidewinder - Uldum');

-- ===================================================================
-- Part B: Tol'vir Scarab -- type-14 direct spawn (no matching critter)
-- Species 546, NpcId 62899
-- Located near Ramkahen (areaId 5035) and Orsis (areaId 5033) in Uldum
-- ===================================================================

INSERT INTO creature (id, map, zoneId, areaId, spawnMask, phaseMask, position_x, position_y, position_z, orientation, spawntimesecs, wander_distance, MovementType, VerifiedBuild) VALUES
(62899, 1, 5034, 5035, 1, 1, -11053.3, -1873.76, 4.32, 6.24995, 300, 8, 1, 0),
(62899, 1, 5034, 5035, 1, 1, -10837.8, -1407.00, 15.44, 1.69694, 300, 8, 1, 0),
(62899, 1, 5034, 5035, 1, 1, -11171.1, -2012.27, 3.57, 6.09769, 300, 8, 1, 0),
(62899, 1, 5034, 5035, 1, 1, -10725.7, -1501.33, 13.73, 4.08216, 300, 8, 1, 0);

REPLACE INTO battle_pet_wild_pool (zone, species, entry, max, minLevel, maxLevel, comment) VALUES
(5034, 546, 62899, 3, 23, 24, 'Tol''vir Scarab - Uldum');
