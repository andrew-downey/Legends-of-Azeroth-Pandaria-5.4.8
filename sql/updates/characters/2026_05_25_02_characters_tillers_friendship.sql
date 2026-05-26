-- Tillers friendship system table
CREATE TABLE IF NOT EXISTS `character_tillers_friendship` (
    `guid` INT UNSIGNED NOT NULL,
    `npc_entry` INT UNSIGNED NOT NULL,
    `standing` INT NOT NULL DEFAULT 0,
    `daily_food_time` INT UNSIGNED NOT NULL DEFAULT 0,
    `daily_gift_time` INT UNSIGNED NOT NULL DEFAULT 0,
    PRIMARY KEY (`guid`, `npc_entry`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
