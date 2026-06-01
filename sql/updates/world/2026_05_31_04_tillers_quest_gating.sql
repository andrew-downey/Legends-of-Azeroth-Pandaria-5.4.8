-- ===========================================================================
-- Tillers quest gating: fix quest_template_addon PrevQuestID chains
-- ===========================================================================
-- TrinityCore handles quest prerequisites via:
--   quest_template_addon.PrevQuestID  (linear chain: positive=rewarded, negative=active)
--   quest_template_addon.ExclusiveGroup (mutual exclusivity)
--   quest_template_addon.RequiredMinRepFaction/Value (reputation gates)
--   conditions table, SourceType=19 (complex conditions)
-- No C++ script needed — all handled by Player::CanTakeQuest() → SatisfyQuestDependentQuests()
-- ===========================================================================

-- ---------------------------------------------------------------------------
-- 1. Fix Learn and Grow chain
--    30252 (A Helping Hand) → 30535 (LG I: Seeds) → 30254 (LG II) → ... → 31945 (LG VI)
--    30254 currently has PrevQuestID=30535 which is correct, but needs explicit setting
-- ---------------------------------------------------------------------------

REPLACE INTO `quest_template_addon` (`ID`, `PrevQuestID`, `NextQuestID`, `ExclusiveGroup`) VALUES
(30252, 0, 0, 0),       -- A Helping Hand: root quest, no prerequisites
(30535, 30252, 0, 0),   -- Learn and Grow I: Seeds (RewardNextQuest=30254 in quest_template)
(30254, 30535, 0, 0),   -- Learn and Grow II: Tilling and Planting
(30255, 30254, 0, 0),   -- Learn and Grow III: Tending Crops
(30256, 30255, 0, 0),   -- Learn and Grow IV: Harvesting
(30257, 30256, 0, 0),   -- Learn and Grow V: Halfhill Market
(31945, 30257, 0, 0);   -- Learn and Grow VI: Gina's Vote

-- ---------------------------------------------------------------------------
-- 2. Growing the Farm chain
--    LG VI (31945) → Growing the Farm I (30260) → 30516 → Growing the Farm II (30523) → 30524 → Growing the Farm III (30529)
-- ---------------------------------------------------------------------------

REPLACE INTO `quest_template_addon` (`ID`, `PrevQuestID`, `NextQuestID`, `ExclusiveGroup`) VALUES
(30260, 31945, 0, 0),   -- Growing the Farm I: The Weeds (RewardNextQuest=30516 in quest_template)
(30516, 30260, 0, 0),   -- Growing the Farm I: A Little Problem
(30523, 30516, 0, 0),   -- Growing the Farm II: The Broken Wagon (RewardNextQuest=30524 in quest_template)
(30524, 30523, 0, 0),   -- Growing the Farm II: Knock on Wood
(30529, 30524, 0, 0);   -- Growing the Farm III: The Mossy Boulder

-- ---------------------------------------------------------------------------
-- 3. Vote chains — all start after Learn and Grow VI (31945)
--    These are parallel chains; each can be done in any order
-- ---------------------------------------------------------------------------

REPLACE INTO `quest_template_addon` (`ID`, `PrevQuestID`, `NextQuestID`, `ExclusiveGroup`) VALUES
-- Mung-Mung's Vote chain
(30258, 31945, 0, 0),   -- Mung-Mung's Vote I: A Hozen's Problem (RewardNextQuest=30259 in quest_template)
(30259, 30258, 0, 0),   -- Mung-Mung's Vote II: Rotten to the Core
(31946, 30259, 0, 0),   -- Mung-Mung's Vote III: The Great Carrot Caper

-- Farmer Fung's Vote chain
(30517, 31945, 0, 0),   -- Farmer Fung's Vote I: Yak Attack (RewardNextQuest=30518 in quest_template)
(30518, 30517, 0, 0),   -- Farmer Fung's Vote II: On the Loose
(31947, 30518, 0, 0),   -- Farmer Fung's Vote III: Crazy For Cabbage

-- Nana's Vote chain
(30519, 31945, 0, 0),   -- Nana's Vote I: Nana's Secret Recipe (RewardNextQuest=31948 in quest_template)
(31948, 30519, 0, 0),   -- Nana's Vote II: The Sacred Springs
(31949, 31948, 0, 0);   -- Nana's Vote III: Witchberry Julep

-- ---------------------------------------------------------------------------
-- 4. Haohan's Vote chain (5 quests)
--    30521 → 30522 → [30525, 30527] → 30528
--    30525 and 30527 are parallel branches from 30522
-- ---------------------------------------------------------------------------

REPLACE INTO `quest_template_addon` (`ID`, `PrevQuestID`, `NextQuestID`, `ExclusiveGroup`) VALUES
(30521, 31945, 0, 0),   -- Haohan's Vote I: Bungalow Break-In (RewardNextQuest=30522 in quest_template)
(30522, 30521, 0, 0),   -- Haohan's Vote II: The Real Culprits
(30525, 30522, 0, 0),   -- Haohan's Vote III: Pure Poison
(30527, 30522, 0, 0),   -- Haohan's Vote IV: Melons For Felons
(30528, 30527, 0, 0);   -- Haohan's Vote V: Chief Yip-Yip

-- ---------------------------------------------------------------------------
-- 5. Lost and Lonely — standalone quest, no prerequisites
-- ---------------------------------------------------------------------------

REPLACE INTO `quest_template_addon` (`ID`, `PrevQuestID`, `NextQuestID`, `ExclusiveGroup`) VALUES
(30526, 0, 0, 0);        -- Lost and Lonely: available anytime

-- ---------------------------------------------------------------------------
-- 6. Inherit the Earth — final quest
--    Requires: Growing the Farm III (30529) rewarded + Exalted with Tillers (faction 1272)
--    Exalted = standing 22000+ in the reputation system
-- ---------------------------------------------------------------------------

REPLACE INTO `quest_template_addon` (`ID`, `PrevQuestID`, `NextQuestID`, `ExclusiveGroup`, `RequiredMinRepFaction`, `RequiredMinRepValue`) VALUES
(32682, 30529, 0, 0, 1272, 22000);
