-- Tillers: Fix Marsh Lily (79269) drop and uniqueness for quest 30257 (Learn and Grow V)
-- - Make item unique so player can only hold 1
-- - Make it always drop from Dark Soil when player is on the quest

UPDATE item_template SET MaxCount = 1 WHERE entry = 79269;
UPDATE gameobject_loot_template SET ChanceOrQuestChance = -100 WHERE entry = 41367 AND item = 79269;
