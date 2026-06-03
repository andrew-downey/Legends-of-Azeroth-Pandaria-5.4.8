-- Tillers Farm: Ensure shrine (215705) and bowls (215706) are always at phaseMask=1
-- These gameobjects exist statically in the world and should be visible to all players
-- in the Valley of Four Winds regardless of farm phase.

-- Ensure shrine is at phaseMask=1
UPDATE gameobject SET phaseMask = 1 WHERE id = 215705;

-- Ensure all offering bowls are at phaseMask=1
UPDATE gameobject SET phaseMask = 1 WHERE id = 215706;
