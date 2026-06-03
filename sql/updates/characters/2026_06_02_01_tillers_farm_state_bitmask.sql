-- Tillers Farm State Bitmask Migration
-- Replaces 16-phase enum with 4 bitmask states for obstacle visibility
-- farmState values: 14 (all obstacles), 12 (weeds cleared), 8 (wagon cleared), 0 (all cleared)

-- Reset farm_phase to new default value (14 = FARM_STATE_FULL)
UPDATE player_farm_state SET farm_phase = 14 WHERE farm_phase != 14 AND farm_phase != 12 AND farm_phase != 8 AND farm_phase != 0;

-- Update plots_unlocked to match farm_phase
UPDATE player_farm_state SET plots_unlocked = 4 WHERE farm_phase = 14 AND plots_unlocked != 4;
UPDATE player_farm_state SET plots_unlocked = 8 WHERE farm_phase = 12 AND plots_unlocked != 8;
UPDATE player_farm_state SET plots_unlocked = 12 WHERE farm_phase = 8 AND plots_unlocked != 12;
UPDATE player_farm_state SET plots_unlocked = 16 WHERE farm_phase = 0 AND plots_unlocked != 16;
