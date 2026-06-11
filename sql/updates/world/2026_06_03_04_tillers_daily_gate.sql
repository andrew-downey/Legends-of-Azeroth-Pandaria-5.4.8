-- Gate Farmer Yoon's daily quests behind Learn and Grow V (30257)
-- Retail: dailies unlock only after completing "Learn and Grow V: Halfhill Market"

INSERT IGNORE INTO conditions (SourceTypeOrReferenceId, SourceGroup, SourceEntry, SourceId, ElseGroup, ConditionTypeOrReference, ConditionValue1, ConditionValue2, NegativeCondition, Comment) VALUES
-- Daily planting quests (10)
(19, 0, 31669, 0, 0, 8, 30257, 0, 0, 'Farm daily - requires Learn and Grow V rewarded'),
(19, 0, 31670, 0, 0, 8, 30257, 0, 0, 'Farm daily - requires Learn and Grow V rewarded'),
(19, 0, 31671, 0, 0, 8, 30257, 0, 0, 'Farm daily - requires Learn and Grow V rewarded'),
(19, 0, 31672, 0, 0, 8, 30257, 0, 0, 'Farm daily - requires Learn and Grow V rewarded'),
(19, 0, 31673, 0, 0, 8, 30257, 0, 0, 'Farm daily - requires Learn and Grow V rewarded'),
(19, 0, 31674, 0, 0, 8, 30257, 0, 0, 'Farm daily - requires Learn and Grow V rewarded'),
(19, 0, 31675, 0, 0, 8, 30257, 0, 0, 'Farm daily - requires Learn and Grow V rewarded'),
(19, 0, 31941, 0, 0, 8, 30257, 0, 0, 'Farm daily - requires Learn and Grow V rewarded'),
(19, 0, 31942, 0, 0, 8, 30257, 0, 0, 'Farm daily - requires Learn and Grow V rewarded'),
(19, 0, 31943, 0, 0, 8, 30257, 0, 0, 'Farm daily - requires Learn and Grow V rewarded'),
-- Killing/tutorial dailies (5)
(19, 0, 30333, 0, 0, 8, 30257, 0, 0, 'Farm daily - requires Learn and Grow V rewarded'),
(19, 0, 30334, 0, 0, 8, 30257, 0, 0, 'Farm daily - requires Learn and Grow V rewarded'),
(19, 0, 30335, 0, 0, 8, 30257, 0, 0, 'Farm daily - requires Learn and Grow V rewarded'),
(19, 0, 30336, 0, 0, 8, 30257, 0, 0, 'Farm daily - requires Learn and Grow V rewarded'),
(19, 0, 30337, 0, 0, 8, 30257, 0, 0, 'Farm daily - requires Learn and Grow V rewarded');
