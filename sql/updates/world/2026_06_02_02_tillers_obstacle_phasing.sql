-- Tillers Obstacle Phasing Migration
-- All obstacles at PUBLIC_FARM_MASK (128) — visible via zone phase definition
-- Pre-farm players (phase=129 = 1|128) see them; farm owners (custom phase) don't
-- Obstacles are dynamically re-spawned by Scene Builder at farm owner's private phase

-- Weeds, wagon, boulder -> all at PUBLIC_FARM_MASK (128)
UPDATE gameobject SET phaseMask = 128 WHERE id IN (210443, 210444, 210445, 210446, 210447, 210448, 210462, 210451, 209572) AND map = 870 AND position_x BETWEEN -250 AND -100 AND position_y BETWEEN 580 AND 700;

-- Plot reference creatures (55626) -> phaseMask 4294901760 (hidden from farm players)
UPDATE creature SET phaseMask = 4294901760 WHERE id = 55626 AND map = 870 AND position_z BETWEEN 164 AND 166;

-- Friendship ranch pets (59536, 59599, 59600) -> phaseMask 4294901760 (hidden from farm players)
UPDATE creature SET phaseMask = 4294901760 WHERE id IN (59536, 59599, 59600) AND map = 870 AND position_x BETWEEN -250 AND -100 AND position_y BETWEEN 580 AND 700;
