-- Move static farm entities back to phase 1 for natural phase-transition visibility
-- 58646/58721 (Farmer Yoon): visible at PHASEMASK_NORMAL for pre-quest players
-- 58719 (Unbudging Rocks): visible at PHASEMASK_NORMAL, invisible in farm custom phase
-- These are dynamically hidden by the per-player farm phase mask which excludes PHASEMASK_NORMAL
-- 55626 (bunnies) and 69098 (chickens) remain at UNREACHABLE (not visible in any phase)

UPDATE `creature` SET `phaseMask` = 1
WHERE `map` = 870
  AND `id` IN (58646, 58721, 58719)
  AND `phaseMask` = 4294901760;
