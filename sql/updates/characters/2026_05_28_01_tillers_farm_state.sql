DROP TABLE IF EXISTS `player_farm_state`;
CREATE TABLE `player_farm_state` (
  `guid` INT(10) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Player GUID (MPS_GUID)',
  `farm_phase` TINYINT(3) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Farm progression phase [0-16], tracks questline progress and plot unlocks',
  `plots_unlocked` TINYINT(3) UNSIGNED NOT NULL DEFAULT '4' COMMENT 'Number of unlocked plots (4, 8, 12, or 16)',
  `last_growth_tick` INT(10) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Unix timestamp of last growth timer tick (for resume on reconnect)',
  PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='Tracks per-player farm-wide state for Sunsong Ranch farming system';
