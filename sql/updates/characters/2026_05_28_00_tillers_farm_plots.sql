DROP TABLE IF EXISTS `player_farm_plots`;
CREATE TABLE `player_farm_plots` (
  `guid` INT(10) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Player GUID (MPS_GUID)',
  `plot_id` TINYINT(3) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Plot number 0-15 (max 16 plots)',
  `state` TINYINT(3) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Plot state (0=EMPTY, 1=SOIL_PREPARED, 2=SEEDED, 3=GROWING, 4=NEEDS_WATER, 5=NEEDS_PEST_CONTROL, 6=READY_TO_HARVEST, 7=BROKEN)',
  `seed_entry` INT(10) UNSIGNED DEFAULT NULL COMMENT 'Seed item entry ID (e.g., 89873 for wheat seed)',
  `needs_watering` TINYINT(1) NOT NULL DEFAULT '0' COMMENT 'Whether plot needs watering',
  `has_pests` TINYINT(1) NOT NULL DEFAULT '0' COMMENT 'Whether plot has pests to spray',
  `maturity_timestamp` INT(10) UNSIGNED DEFAULT NULL COMMENT 'Unix timestamp when crop will be ready (NULL = not planted)',
  PRIMARY KEY (`guid`, `plot_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='Tracks per-player farm plot states for Sunsong Ranch farming system';
