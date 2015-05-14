-- -----------------------------------------------
-- Edibles definition file for simplicissimus
-- Copyright (c) Andreas Bauer <baueran@gmail.com>
-- -----------------------------------------------

-- dofile(simpl_datapath() .. "weapons/defs.lua")

Edibles["iron ration"]  = { 
   name          = "iron ration", 
   plural_name   = "iron rations", 
   food_up       = 200,
   hp_up         = 0,
   heal_poison   = "false",
   cause_poison  = "false",
   magic_herb    = "false",
   rounds_drunk  = 0,
   gold          = 3,
   weight        = 2
}

-- simpl_pushweapon(Weapons.axe)
-- print("lua: axe.lua: added weapon " .. Weapons.axe.name)
