-- -----------------------------------------------
-- Torch definition file for simplicissimus
-- Copyright (c) Andreas Bauer <baueran@gmail.com>
-- -----------------------------------------------

-- dofile(simpl_datapath() .. "weapons/defs.lua")

Weapons["torch"]  = { 
   name          = "torch", 
   plural_name   = "torches", 
   hands         = 1, 
   range         = 10,
   damage_min    = 1,
   damage_max    = 2,
   damage_bonus  = 0,
   weight        = 1,
   icon          = 147,
   gold          = 2,
   light_radius  = 4,
   destroy_after = 10 
}

-- simpl_pushweapon(Weapons.axe)
-- print("lua: axe.lua: added weapon " .. Weapons.axe.name)