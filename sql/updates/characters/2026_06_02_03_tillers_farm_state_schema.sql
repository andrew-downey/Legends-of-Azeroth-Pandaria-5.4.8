-- Update player_farm_state schema to reflect new farm_phase bitmask values
-- Default changed from 0 to 14 (FARM_STATE_FULL)

ALTER TABLE player_farm_state MODIFY COLUMN farm_phase tinyint unsigned NOT NULL DEFAULT '14' COMMENT 'Farm state bitmask: 14=all obstacles, 12=weeds cleared, 8=wagon cleared, 0=all cleared';
ALTER TABLE player_farm_state MODIFY COLUMN plots_unlocked tinyint unsigned NOT NULL DEFAULT '4' COMMENT 'Number of unlocked plots (4, 8, 12, or 16)';
