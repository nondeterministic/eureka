-- -----------------------------------------------
-- Axe definition file for simplicissimus
-- Copyright (c) Andreas Bauer <baueran@gmail.com>
-- -----------------------------------------------

-- dofile(simpl_datapath() .. "weapons/defs.lua")

Weapons["axe"]  = { 
   name         = "axe", 
   plural_name  = "axes", 
   hands        = 1, 
   range        = 10,
   damage_min   = 2,
   damage_max   = 3,
   damage_bonus = 0,
   weight       = 1,
   icon         = 571,
   gold         = 7
}

-- simpl_pushweapon(Weapons.axe)
-- print("lua: axe.lua: added weapon " .. Weapons.axe.name)
