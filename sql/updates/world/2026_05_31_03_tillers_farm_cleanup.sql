-- Tillers farm cleanup: remove debug world DB spawns from map 870
-- Melons (66129) are replaced by bunnies (55626) as plot position references
-- Debug creatures (bunnies, rocks, chickens) moved to unreachable phase 0xFFFF0000
-- for C++ dynamic spawn reuse on the farm
-- Farmer Yoon (58646, 58721) are static quest givers — kept at phaseMask=1
-- Idempotent: DELETE + INSERT for melons, WHERE phaseMask=1 for UPDATE

-- ===========================================================================
-- Remove melon spawns (66129) — replaced by bunny (55626) plot position references
-- ===========================================================================

DELETE FROM `creature` WHERE `map` = 870 AND `id` = 66129;

-- ===========================================================================
-- Move remaining debug creatures to unreachable phase 0xFFFF0000 (4294901760)
-- This phase has zero overlap with player phase masks: (guidLow << 8) | farmPhase
-- Range: ~0x00000001 to 0x00FFFFFF — 0xFFFF0000 never overlaps
-- Creatures are still spawned in the world but invisible to farm players
-- ===========================================================================

UPDATE `creature` SET `phaseMask` = 4294901760
WHERE `map` = 870
  AND `id` IN (55626, 58719, 69098)
  AND `phaseMask` = 1;

-- ===========================================================================
-- Fix Farmer Yoon visibility — was incorrectly moved to unreachable phase
-- 58646 (farmhouse/shrine): npcflag=3, 29 quest starters, 27 quest enders
-- 58721 (farm tools): npcflag=2, 1 quest starter (30252)
-- Both are static quest givers, not debug content
-- ===========================================================================

UPDATE `creature` SET `phaseMask` = 1
WHERE `map` = 870
  AND `id` IN (58646, 58721)
  AND `phaseMask` = 4294901760;

-- ===========================================================================
-- Fix 58646 unit_flags — remove incorrect SPIRITGUIDE flag (32768)
-- npcflag=3 (GOSSIP+QUESTGIVER) is correct, unit_flags should be 0
-- ===========================================================================

UPDATE `creature_template` SET `unit_flags` = 0
WHERE `entry` = 58646
  AND `unit_flags` = 32768;
