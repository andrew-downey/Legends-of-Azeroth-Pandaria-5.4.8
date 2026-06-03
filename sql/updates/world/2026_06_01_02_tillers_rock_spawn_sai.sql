-- Tillers Farm: Spawn Unbudding Rocks (58719) when quest 30252 is accepted
-- Rocks spawn at 8 positions around the farm area, visible in default phase
-- Player clicks them for "A Helping Hand" quest completion

-- Idempotent: remove old entries (id 7-14) then re-insert
DELETE FROM smart_scripts WHERE entryorguid = 58646 AND source_type = 0 AND id BETWEEN 7 AND 14;

INSERT INTO smart_scripts (entryorguid, source_type, id, link, event_type, event_phase_mask, event_chance, event_flags, event_param1, event_param2, event_param3, event_param4, event_param5, action_type, action_param1, action_param2, action_param3, action_param4, action_param5, action_param6, target_type, target_param1, target_param2, target_param3, target_param4, target_x, target_y, target_z, target_o, comment)
VALUES 
(58646, 0, 7, 0, 19, 0, 100, 0, 30252, 0, 0, 0, 0, 12, 58719, 7, 0, 0, 0, 0, 8, 0, 0, 0, 0, -160.729, 634.592, 165.493, 0, 'Farmer Yoon - On Quest Accept Spawn Unbudding Rock 1'),
(58646, 0, 8, 0, 19, 0, 100, 0, 30252, 0, 0, 0, 0, 12, 58719, 7, 0, 0, 0, 0, 8, 0, 0, 0, 0, -179.028, 650.283, 165.508, 0.890118, 'Farmer Yoon - On Quest Accept Spawn Unbudding Rock 2'),
(58646, 0, 9, 0, 19, 0, 100, 0, 30252, 0, 0, 0, 0, 12, 58719, 7, 0, 0, 0, 0, 8, 0, 0, 0, 0, -151.29, 629.465, 166.738, 0, 'Farmer Yoon - On Quest Accept Spawn Unbudding Rock 3'),
(58646, 0, 10, 0, 19, 0, 100, 0, 30252, 0, 0, 0, 0, 12, 58719, 7, 0, 0, 0, 0, 8, 0, 0, 0, 0, -170.748, 642.601, 165.493, 1.50098, 'Farmer Yoon - On Quest Accept Spawn Unbudding Rock 4'),
(58646, 0, 11, 0, 19, 0, 100, 0, 30252, 0, 0, 0, 0, 12, 58719, 7, 0, 0, 0, 0, 8, 0, 0, 0, 0, -192.471, 623.967, 166.416, 5.75959, 'Farmer Yoon - On Quest Accept Spawn Unbudding Rock 5'),
(58646, 0, 12, 0, 19, 0, 100, 0, 30252, 0, 0, 0, 0, 12, 58719, 7, 0, 0, 0, 0, 8, 0, 0, 0, 0, -194.368, 655.786, 164.555, 0, 'Farmer Yoon - On Quest Accept Spawn Unbudding Rock 6'),
(58646, 0, 13, 0, 19, 0, 100, 0, 30252, 0, 0, 0, 0, 12, 58719, 7, 0, 0, 0, 0, 8, 0, 0, 0, 0, -209.031, 605.566, 169.547, 0, 'Farmer Yoon - On Quest Accept Spawn Unbudding Rock 7'),
(58646, 0, 14, 0, 19, 0, 100, 0, 30252, 0, 0, 0, 0, 12, 58719, 7, 0, 0, 0, 0, 8, 0, 0, 0, 0, -190.561, 641.505, 165.493, 1.06465, 'Farmer Yoon - On Quest Accept Spawn Unbudding Rock 8');
