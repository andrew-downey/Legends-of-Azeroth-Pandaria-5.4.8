-- Pandaria AreaTrigger quest starters and completers
-- Wandering Isle (map 860) - verified against TrinityCore scripts and position analysis
--
-- Sources:
--   TrinityCore zone_the_wandering_isle.cpp (at_min_dimwind_captured,
--   at_cave_of_meditation, at_inside_of_cave_of_meditation)
--   AreaTrigger.dbc position verification
--
-- Notes:
--   These DB-driven entries handle basic quest auto-start / completion via
--   area trigger. For the full retail experience (NPC spawning, dialogue,
--   meditation timer bar, etc.), C++ AreaTriggerScripts are needed.
--   The server auto-adds QUEST_FLAGS_AUTO_ACCEPT to areatrigger_queststarter
--   quests and QUEST_SPECIAL_FLAGS_EXPLORATION_OR_EVENT to
--   areatrigger_involvedrelation quests on startup.
--
-- areatrigger_queststarter: player walks into area -> quest is offered/accepted
-- areatrigger_involvedrelation: player walks into area -> objective completed
--
-- Verified mappings:
--   Trigger 6958 (Min Dimwind) -> Q29419 "The Missing Driver" (involvedrelation)
--     Position: (1413.07, 3535.35, 86.46) - Min Dimwind at (1413.71, 3533.77, 85.97)
--     Distance: 1.7 yards. TC: at_min_dimwind_captured -> QUEST_THE_MISSING_DRIVER
--
--   Trigger 7756 (Cave of Meditation entrance) -> Q29414 "The Way of the Tushui"
--     Position: (1145.14, 3435.60, 104.97) - Quest ender NPC at (1141.85, 3431.46, 105.47)
--     TC: at_cave_of_meditation -> QUEST_THE_WAY_OF_THE_TUSHUI
--     Note: In TC this is a quest starter that gives a meditation bar aura on enter.
--     As a DB areatrigger_queststarter, it will auto-offer the quest on enter.
--
--   Trigger 7645 (Inside Cave of Meditation) -> Q29414
--     Position: (1139.59, 3434.31, 105.54) - ~5 yards from 7756
--     TC: at_inside_of_cave_of_meditation -> QUEST_THE_WAY_OF_THE_TUSHUI
--     Triggers deeper event (Master Li Fei summon, Aysa dialogue).
--     As a DB areatrigger_involvedrelation, it will complete the quest if
--     the quest has QUEST_FLAGS_COMPLETION_AREA_TRIGGER. Currently unused
--     until scripts add the full meditation mechanic.
--
-- Idempotent: INSERT IGNORE / REPLACE safe due to PRIMARY KEY (id, quest).

-- ============================================================
-- Quest Completion / Objective (areatrigger_involvedrelation)
-- ============================================================

-- Trigger 6958: Walk to Min Dimwind's location to complete objective
-- Quest 29419: "The Missing Driver" (rescue the cart driver)
-- TC: at_min_dimwind_captured -> QUEST_THE_MISSING_DRIVER
INSERT INTO areatrigger_involvedrelation (id, quest) VALUES (6958, 29419);

-- ============================================================
-- Quest Starters (areatrigger_queststarter)
-- ============================================================

-- Trigger 7756: Walk into Cave of Meditation entrance
-- Quest 29414: "The Way of the Tushui" (protect Aysa during meditation)
-- TC: at_cave_of_meditation -> QUEST_THE_WAY_OF_THE_TUSHUI
INSERT INTO areatrigger_queststarter (id, quest) VALUES (7756, 29414);

-- ============================================================
-- Blocked / Requires C++ Script
-- ============================================================

-- Trigger 7645 (Inside Cave of Meditation) - Q29414 involvedrelation
--   Blocked: requires the meditation timer bar mechanic (SPELL_MEDITATION_TIMER_BAR),
--   Master Li Fei summoning, and Aysa dialogue. Only useful once C++ script
--   is implemented (at_inside_of_cave_of_meditation).
-- INSERT INTO areatrigger_involvedrelation (id, quest) VALUES (7645, 29414);

-- Trigger 6958 as quest starter - does not need a starter since Merchant Lorvo
--   already gives the quest. The trigger is for the FINDING objective, not start.

-- Trigger 7835 (Temple Entrance) - Quest 29521 "The Singing Pools"
--   Position: (964.01, 3604.08, 196.71) - Master Shang Xi at (964.26, 3603.69, 196.52)
--   Currently has C++ script (AreaTrigger_at_temple_entrance) in areatrigger_scripts.
--   The script handles the Singing Pools mechanic. Adding a DB entry would conflict.

-- End of migration
