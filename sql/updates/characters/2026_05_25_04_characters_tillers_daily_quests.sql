-- Tillers daily quests table
CREATE TABLE IF NOT EXISTS `character_tillers_daily_quests` (
    `guid` INT UNSIGNED NOT NULL,
    `daily_quests_mask` INT UNSIGNED NOT NULL DEFAULT 0,
    `last_daily_reset` INT UNSIGNED NOT NULL DEFAULT 0,
    `active_crop_daily` INT UNSIGNED NOT NULL DEFAULT 0,
    `active_kill_daily` INT UNSIGNED NOT NULL DEFAULT 0,
    `andi_gift_target` INT UNSIGNED NOT NULL DEFAULT 0,
    PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
