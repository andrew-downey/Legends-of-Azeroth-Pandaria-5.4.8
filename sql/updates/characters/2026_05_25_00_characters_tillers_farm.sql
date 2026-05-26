-- Tillers Farm System - Phase 3: Expansion timer support
-- ALTER must be run after the base tables from 2026_05_24_00

ALTER TABLE `character_tillers_farm_data`
    ADD COLUMN `expansion_timer_end` INT UNSIGNED NOT NULL DEFAULT 0 COMMENT 'Unix timestamp when 15-min expansion timer completes (0=no timer)'
    AFTER `votes_mask`;
