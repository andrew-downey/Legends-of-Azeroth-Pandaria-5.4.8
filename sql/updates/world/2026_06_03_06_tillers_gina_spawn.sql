-- Gina Mudclaw is always present at Halfhill Market (phase 1)
-- No longer dynamically spawned/despawned by SAI

DELETE FROM smart_scripts WHERE entryorguid=58646 AND source_type=0 AND id=18;
DELETE FROM smart_scripts WHERE entryorguid=58706 AND source_type=0 AND id=3;
UPDATE creature SET phaseMask=1 WHERE guid=514083 AND id=58706;
