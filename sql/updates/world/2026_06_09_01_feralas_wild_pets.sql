-- Feralas (zone 357) wild battle pets — 5 species, hybrid approach
-- 3 common pets via type 8 critter replacement (existing spawns reused)
-- 2 unique pets via type 14 direct spawns (MoP-style)

-- Part A: Pool entries for existing type 8 critters (simple one-to-one replacement)
INSERT INTO battle_pet_wild_pool (zone, species, entry, max, minLevel, maxLevel, comment) VALUES
(357, 378, 721, 10, 11, 12, 'Rabbit - Feralas'),
(357, 379, 1412, 10, 11, 12, 'Squirrel - Feralas'),
(357, 387, 2914, 10, 11, 12, 'Snake - Feralas');

-- Part B: Nether Faerie Dragon (species 557, NpcId 62395)
-- Retail location: "outside Dire Maul instance portals"
-- Coordinates from Feralas Wisp (40079) spawns in Twin Colossals (area 1108)
INSERT INTO creature (id, map, zoneId, areaId, spawnMask, phaseMask, position_x, position_y, position_z, orientation, spawntimesecs, wander_distance, MovementType, VerifiedBuild) VALUES
(62395, 1, 357, 1108, 1, 1, -4674.47, 1973.56, 71.185, 5.43369, 300, 5, 1, 0),
(62395, 1, 357, 1108, 1, 1, -4693.73, 1943.40, 74.981, 1.23958, 300, 5, 1, 0),
(62395, 1, 357, 1108, 1, 1, -4251.85, 2125.86, 80.958, 3.99725, 300, 5, 1, 0),
(62395, 1, 357, 1108, 1, 1, -4383.95, 2107.42, 60.714, 3.92380, 300, 5, 1, 0),
(62395, 1, 357, 1108, 1, 1, -4453.33, 2026.67, 48.189, 4.08029, 300, 5, 1, 0);

INSERT INTO battle_pet_wild_pool (zone, species, entry, max, minLevel, maxLevel, comment) VALUES
(357, 557, 62395, 3, 11, 12, 'Nether Faerie Dragon - Feralas');

-- Part C: Stunted Yeti (species 1158, NpcId 68805)
-- Retail location: "near Feral Scar Yetis south of Dire Maul" (minimap ~52,60)
-- Coordinates from Feral Scar Yeti (40224) spawns in The Writhing Deep (area 1105)
INSERT INTO creature (id, map, zoneId, areaId, spawnMask, phaseMask, position_x, position_y, position_z, orientation, spawntimesecs, wander_distance, MovementType, VerifiedBuild) VALUES
(68805, 1, 357, 1105, 1, 1, -5051.77, 1808.02, 69.922, 5.48033, 300, 5, 1, 0),
(68805, 1, 357, 1105, 1, 1, -5138.83, 1895.44, 98.574, 2.82533, 300, 5, 1, 0),
(68805, 1, 357, 1105, 1, 1, -5200.21, 1809.93, 115.94, 2.97104, 300, 5, 1, 0),
(68805, 1, 357, 1105, 1, 1, -5164.32, 1838.36, 113.51, 1.12301, 300, 5, 1, 0),
(68805, 1, 357, 1105, 1, 1, -5112.57, 1914.81, 90.991, 0.90976, 300, 5, 1, 0);

INSERT INTO battle_pet_wild_pool (zone, species, entry, max, minLevel, maxLevel, comment) VALUES
(357, 1158, 68805, 3, 11, 12, 'Stunted Yeti - Feralas');
