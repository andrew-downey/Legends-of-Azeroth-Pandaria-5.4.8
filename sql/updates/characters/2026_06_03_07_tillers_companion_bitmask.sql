-- Tillers Farm: Add companion unlock bitmask to player_farm_state
-- Supports up to 16 companions (16 bits in SMALLINT UNSIGNED)
-- Companion bits: Shaggy, Fifi, Chickens, Sheep, Luna, Piggies, Orange Tree, Furniture, Mailbox, Lost Dog

ALTER TABLE player_farm_state
    ADD COLUMN companions SMALLINT UNSIGNED NOT NULL DEFAULT 0 AFTER plots_unlocked;
