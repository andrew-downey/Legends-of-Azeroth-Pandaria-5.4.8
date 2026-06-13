-- Mount Hyjal (zone 616, map 1) wild battle pets -- 17 species
-- Pet battle level range: 22-24 (zone level 80-82)
-- 15 type-8 replacement pools + 2 type-14 direct spawns (Forest Moth, Nordrassil Wisp)

-- ===================================================================
-- Part A: Type-8 pool entries -- existing critter replacement
-- 15 species with abundant critter spawns already in map 1, zone 616
-- ===================================================================

REPLACE INTO battle_pet_wild_pool (zone, species, entry, max, minLevel, maxLevel, comment) VALUES
(616, 487, 49779, 10, 22, 24, 'Alpine Chipmunk - Mount Hyjal'),
(616, 632, 50478, 6, 22, 24, 'Ash Lizard - Mount Hyjal'),
(616, 626, 20725, 6, 22, 24, 'Bat - Mount Hyjal'),
(616, 540, 50485, 5, 22, 24, 'Carrion Rat - Mount Hyjal'),
(616, 755, 49759, 6, 22, 24, 'Death''s Head Cockroach - Mount Hyjal'),
(616, 479, 49728, 8, 22, 24, 'Elfin Rabbit - Mount Hyjal'),
(616, 415, 9699, 15, 22, 24, 'Fire Beetle - Mount Hyjal'),
(616, 541, 49780, 14, 22, 24, 'Fire-Proof Roach - Mount Hyjal'),
(616, 539, 50419, 4, 22, 24, 'Grotto Vole - Mount Hyjal'),
(616, 378, 721, 4, 22, 24, 'Rabbit - Mount Hyjal'),
(616, 482, 50481, 6, 22, 24, 'Rock Viper - Mount Hyjal'),
(616, 503, 49844, 12, 22, 24, 'Silky Moth - Mount Hyjal'),
(616, 379, 52195, 6, 22, 24, 'Squirrel - Mount Hyjal'),
(616, 469, 49861, 6, 22, 24, 'Twilight Beetle - Mount Hyjal');

-- ===================================================================
-- Part B: Forest Moth -- type-14 direct spawn (no matching critter)
-- Species 478, NpcId 62177
-- Scattered across Hyjal near quest NPC spawns in The Regrowth
-- ===================================================================

INSERT INTO creature (id, map, zoneId, areaId, spawnMask, phaseMask, position_x, position_y, position_z, orientation, spawntimesecs, wander_distance, MovementType, VerifiedBuild) VALUES
(62177, 1, 616, 3645, 1, 1, 5118.35, -3556.20, 1585.30, 1.23456, 300, 5, 1, 0),
(62177, 1, 616, 3645, 1, 1, 5012.10, -3689.40, 1592.80, 4.56789, 300, 5, 1, 0),
(62177, 1, 616, 3645, 1, 1, 4945.60, -3488.90, 1558.40, 2.98765, 300, 5, 1, 0),
(62177, 1, 616, 3645, 1, 1, 5234.80, -3623.10, 1601.20, 0.54321, 300, 5, 1, 0),
(62177, 1, 616, 3645, 1, 1, 5067.50, -3405.70, 1567.60, 3.14159, 300, 5, 1, 0);

REPLACE INTO battle_pet_wild_pool (zone, species, entry, max, minLevel, maxLevel, comment) VALUES
(616, 478, 62177, 3, 22, 24, 'Forest Moth - Mount Hyjal');

-- ===================================================================
-- Part C: Nordrassil Wisp -- type-14 direct spawn (no matching critter)
-- Species 547, NpcId 62888
-- Located around Nordrassil World Tree (areaId 3646) in Mount Hyjal
-- ===================================================================

INSERT INTO creature (id, map, zoneId, areaId, spawnMask, phaseMask, position_x, position_y, position_z, orientation, spawntimesecs, wander_distance, MovementType, VerifiedBuild) VALUES
(62888, 1, 616, 3646, 1, 1, 5405.32, -3476.89, 1661.48, 0.78540, 300, 5, 1, 0),
(62888, 1, 616, 3646, 1, 1, 5334.67, -3389.54, 1648.32, 3.92699, 300, 5, 1, 0),
(62888, 1, 616, 3646, 1, 1, 5248.10, -3578.23, 1615.71, 5.49779, 300, 5, 1, 0);

REPLACE INTO battle_pet_wild_pool (zone, species, entry, max, minLevel, maxLevel, comment) VALUES
(616, 547, 62888, 2, 22, 24, 'Nordrassil Wisp - Mount Hyjal');
