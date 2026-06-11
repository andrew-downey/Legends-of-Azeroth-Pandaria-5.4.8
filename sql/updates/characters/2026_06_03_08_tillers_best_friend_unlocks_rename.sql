-- Tillers Farm: Rename companions to best_friend_unlocks and ensure default rows exist

ALTER TABLE player_farm_state RENAME COLUMN companions TO best_friend_unlocks;

-- Ensure all players with farm data have a state row
INSERT INTO player_farm_state (guid, farm_phase, plots_unlocked, last_growth_tick, best_friend_unlocks)
SELECT guid, 14, 4, 0, 0 FROM characters WHERE guid NOT IN (SELECT guid FROM player_farm_state);
