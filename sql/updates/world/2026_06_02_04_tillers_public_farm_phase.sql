-- Tillers Farm: Public Farm Phase Architecture
-- Static farm objects at PUBLIC_FARM_MASK (128 = bit 7) for pre-farm players
-- Farm owners use SetCustomPhase((guid<<8)|1) which bypasses phase definitions
--
-- Visibility:
--   Pre-30252 player: phase = 129 (PHASEMASK_NORMAL | PUBLIC_FARM_MASK) via zone definition
--     → sees normal world (bit 0) + static farm objects (bit 7)
--   Post-30252 player: phase = (guid<<8) | 1 (custom, bypasses zone definition)
--     → normal world visible, bit 7 not present → static farm invisible
--   phasemask=129 (not 128) preserves PHASEMASK_NORMAL so world NPCs/objects aren't lost
--
-- Zone note: AreaTable DBC has ParentAreaID=0 for farm micro-zones (5805, 5840),
-- so GetZoneId() returns those instead of 1023. Definitions must target the
-- actual zone ID the player resolves to at runtime.

-- Farmhouse Yoon (516066) and Tools Yoon (58721) to UNREACHABLE phase
-- NOTE: phaseMask 0 gets auto-corrected to 1 by ObjectMgr on load, must use 4294901760
UPDATE creature SET phaseMask = 4294901760 WHERE id = 58646 AND guid = 516066;
UPDATE creature SET phaseMask = 4294901760 WHERE id = 58721 AND map = 870;

-- Ground-level Farmer Yoon (516067) to public farm mask
UPDATE creature SET phaseMask = 128 WHERE id = 58646 AND guid = 516067;

-- Obstacles (weeds, wagon, boulder) to public farm mask
UPDATE gameobject SET phaseMask = 128 WHERE id IN (210443, 210444, 210445, 210446, 210447, 210448, 210462, 210451, 209572) AND map = 870 AND position_x BETWEEN -250 AND -100 AND position_y BETWEEN 580 AND 700;

-- Phase definitions: every farm-area entrant gets PUBLIC_FARM_MASK
-- 1023 = Valley of the Four Winds (parent zone)
-- 5805 = The Heartland (farm approach, ParentAreaID=0 in DBC)
-- 5840 = Sunsong Ranch (farm proper, ParentAreaID=0 in DBC)
-- No conditions needed — SetCustomPhase bypasses definitions for farm owners
REPLACE INTO phase_definitions (zoneId, entry, phasemask, phaseId, terrainswapmap, worldMapArea, flags) VALUES (1023, 1, 129, 0, 0, 0, 0);
REPLACE INTO phase_definitions (zoneId, entry, phasemask, phaseId, terrainswapmap, worldMapArea, flags) VALUES (5805, 1, 129, 0, 0, 0, 0);
REPLACE INTO phase_definitions (zoneId, entry, phasemask, phaseId, terrainswapmap, worldMapArea, flags) VALUES (5840, 1, 129, 0, 0, 0, 0);

-- Hide static rocks (58719) — SAI on 58646 handles dynamic spawning on quest 30252 accept
-- Without this, 8 static rocks at phase 1 are always visible to all players
UPDATE creature SET phaseMask = 4294901760 WHERE id = 58719 AND map = 870;

-- Fix quest starter: 30252 "A Helping Hand" must start from 58646 (Farmer Yoon) not 58721 (Tools Yoon)
-- SAI actions 7-14 (rock spawning) are on entry 58646 and require the quest to be accepted from it
DELETE FROM creature_queststarter WHERE id = 58721 AND quest = 30252;
REPLACE INTO creature_queststarter (id, quest) VALUES (58646, 30252);
