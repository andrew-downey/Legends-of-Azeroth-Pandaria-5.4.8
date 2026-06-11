-- Tillers Farm: Add ScriptName to soil gameobject for interaction spells
-- Without this, OnDummyEffect never fires and tilling/harvesting doesn't work

UPDATE gameobject_template SET ScriptName = 'tillers_soil_patch' WHERE entry = 186314;

