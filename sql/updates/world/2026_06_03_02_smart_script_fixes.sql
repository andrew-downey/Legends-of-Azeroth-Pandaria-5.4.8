-- SmartAIMgr log warning fixes
-- Categories:
--   1. Wrong AIName (13 creatures with SmartAI scripts but empty AIName)
--   2. Broken min/max params (entries where min > max in event/timing params)
--   3. Missing Quest SpecialFlags (quests need FLAGS_EXPLORATION_OR_EVENT=2)
--
-- See TODO.md "Smart Script Fixes" for non-trivial items still needing research.

-- 1. Fix creature_template AIName for creatures that have SmartAI scripts but empty AIName
--    Log: "Creature entry X has SmartAI scripts, but its AIName is not 'SmartAI'"
UPDATE `creature_template` SET `AIName` = 'SmartAI'
WHERE `entry` IN (18166, 25967, 31848, 35589, 35604, 37063, 64267, 64360, 64362, 64363, 64364, 64656, 66654);

-- 2. Fix inverted min/max event parameters
--    Log: "Entry X ... uses min/max params wrong (A/B), skipped."

-- 25073 (Darkspine Siren): HEALTH_PCT event - HP threshold range was 15%–0% (impossible)
-- Fix: swap to 0%–15% (triggers when health drops below 15%)
UPDATE `smart_scripts`
SET `event_param1` = 0, `event_param2` = 15
WHERE `entryorguid` = 25073 AND `id` = 4 AND `action_type` = 25;

-- 17214 (Anchorite Fateema): REWARD_QUEST event - cooldown min=2, max=0 (inverted)
-- Fix: swap to min=0, max=2 (no minimum cooldown, 2-second max)
UPDATE `smart_scripts`
SET `event_param2` = 0, `event_param3` = 2
WHERE `entryorguid` = 17214 AND `id` = 0 AND `action_type` = 80;

-- 17215 (Daedal): REWARD_QUEST event - cooldown min=2, max=0 (inverted)
-- Fix: swap to min=0, max=2
UPDATE `smart_scripts`
SET `event_param2` = 0, `event_param3` = 2
WHERE `entryorguid` = 17215 AND `id` = 2 AND `action_type` = 80;

-- 32423 (Matthias Lehner): REWARD_QUEST event - cooldown min=1, max=0 (inverted)
-- Fix: swap to min=0, max=1
UPDATE `smart_scripts`
SET `event_param2` = 0, `event_param3` = 1
WHERE `entryorguid` = 32423 AND `id` = 0 AND `action_type` = 12;

-- 3. Fix Quest SpecialFlags for exploration/event quests
--    Log: "SpecialFlags for Quest entry X does not include FLAGS_EXPLORATION_OR_EVENT(2)"
--    QUEST_SPECIAL_FLAGS_EXPLORATION_OR_EVENT = 2 is required for SMART_ACTION_CALL_AREAEXPLOREDOREVENTHAPPENS
UPDATE `quest_template_addon`
SET `SpecialFlags` = `SpecialFlags` | 2
WHERE `Id` IN (14482, 25924);
