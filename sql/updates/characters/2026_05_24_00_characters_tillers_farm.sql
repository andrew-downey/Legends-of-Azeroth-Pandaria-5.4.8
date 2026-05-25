-- Tillers Farm System - Character Database Tables
-- Sunsong Ranch personal farm plot persistence

DROP TABLE IF EXISTS `character_tillers_farm`;
CREATE TABLE `character_tillers_farm` (
    `guid` INT UNSIGNED NOT NULL COMMENT 'Player GUID',
    `plot_id` TINYINT UNSIGNED NOT NULL COMMENT 'Plot slot (0-15)',
    `state` TINYINT UNSIGNED NOT NULL DEFAULT 0 COMMENT '0=untilled,1=tilled,2=growing,3=ripe',
    `crop` TINYINT UNSIGNED NOT NULL DEFAULT 0 COMMENT 'Crop type (0=none, 1-12=seed types)',
    `planted_at` INT UNSIGNED NOT NULL DEFAULT 0 COMMENT 'Unix timestamp when planted',
    `condition` TINYINT UNSIGNED NOT NULL DEFAULT 0 COMMENT '0=healthy,1=parched,2=pests,3=weeds',
    `is_special` TINYINT UNSIGNED NOT NULL DEFAULT 0 COMMENT '0=normal,1=plump,2=bursting',
    PRIMARY KEY (`guid`, `plot_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='Player farm plot state per slot';

DROP TABLE IF EXISTS `character_tillers_farm_data`;
CREATE TABLE `character_tillers_farm_data` (
    `guid` INT UNSIGNED NOT NULL PRIMARY KEY COMMENT 'Player GUID',
    `unlocked_plots` TINYINT UNSIGNED NOT NULL DEFAULT 4 COMMENT 'Number of unlocked plots (4/8/12/16)',
    `has_irrigation` TINYINT UNSIGNED NOT NULL DEFAULT 0 COMMENT 'Has Jinyu Princess Sprinkler system',
    `has_antipest` TINYINT UNSIGNED NOT NULL DEFAULT 0 COMMENT 'Has anti-pest system',
    `has_plow` TINYINT UNSIGNED NOT NULL DEFAULT 0 COMMENT 'Has Earth-Slasher plow',
    `votes_mask` TINYINT UNSIGNED NOT NULL DEFAULT 0 COMMENT 'Bitmask for 5 NPC votes'
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='Player farm upgrade data';
