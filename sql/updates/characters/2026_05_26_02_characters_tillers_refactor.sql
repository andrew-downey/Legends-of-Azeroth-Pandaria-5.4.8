-- Tillers Phase 4B Refactor: Create food timer table

CREATE TABLE `character_tillers_npc_timers` (
    `guid` INT UNSIGNED NOT NULL,
    `npc_entry` INT UNSIGNED NOT NULL,
    `last_food_day` INT UNSIGNED NOT NULL DEFAULT 0,
    PRIMARY KEY (`guid`, `npc_entry`)
);
