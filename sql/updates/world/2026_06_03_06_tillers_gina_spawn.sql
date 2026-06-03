-- Spawn Gina Mudclaw at Halfhill Market when player accepts Learn and Grow V
-- Despawn her when the quest is rewarded

INSERT INTO smart_scripts (entryorguid, source_type, id, link, event_type, event_phase_mask, event_chance, event_flags, event_param1, event_param2, event_param3, event_param4, event_param5, action_type, action_param1, action_param2, action_param3, action_param4, action_param5, action_param6, target_type, target_param1, target_param2, target_param3, target_param4, target_x, target_y, target_z, target_o, comment) VALUES
(58646, 0, 18, 0, 19, 0, 100, 0, 30257, 0, 0, 0, 0, 1, 58706, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -169.0, 650.0, 165.0, 0, 'Farmer Yoon - On Quest Accept 30257 Spawn Gina at Halfhill'),
(58706, 0, 3, 0, 34, 0, 100, 0, 30257, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Gina - On Quest Reward 30257 Despawn');
