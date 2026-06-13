-- Twilight Highlands (zone 4922, map 0) wild battle pets — 2 missing species
-- Pet battle level range: 23-24 (zone 30-35)
-- 1 type-8 replacement + 1 type-14 direct spawn (Twilight Fiendling)

-- ===================================================================
-- Part A: Cockroach — type-8 replacement
-- Species 393, type-8 critter 45439 (283 spawns in map 0, zone 4922)
-- ===================================================================

INSERT INTO battle_pet_wild_pool (zone, species, entry, max, minLevel, maxLevel, comment) VALUES
(4922, 393, 45439, 28, 23, 24, 'Cockroach - Twilight Highlands');

-- ===================================================================
-- Part B: Twilight Fiendling — type-14 direct spawn (no matching critter)
-- Species 552, NpcId 62914
-- Located near Twilight Bonebreaker NPCs in Twilight Highlands
-- Coordinates sourced from nearby Twilight Bonebreaker (45334) spawns
-- ===================================================================

INSERT INTO creature (id, map, zoneId, areaId, spawnMask, phaseMask, position_x, position_y, position_z, orientation, spawntimesecs, wander_distance, MovementType, VerifiedBuild) VALUES
(62914, 0, 4922, 5439, 1, 1, -2712.72, -4497.87, 200.19, 5.65487, 300, 8, 1, 0),
(62914, 0, 4922, 5439, 1, 1, -2787.63, -4695.16, 191.26, 3.14159, 300, 8, 1, 0),
(62914, 0, 4922, 5439, 1, 1, -2652.81, -4724.10, 169.10, 2.09440, 300, 8, 1, 0);

INSERT INTO battle_pet_wild_pool (zone, species, entry, max, minLevel, maxLevel, comment) VALUES
(4922, 552, 62914, 3, 23, 24, 'Twilight Fiendling - Twilight Highlands');
