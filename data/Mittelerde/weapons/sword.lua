-- -----------------------------------------------
-- Definition file for simplicissimus
-- Copyright (c) Andreas Bauer <baueran@gmail.com>
-- -----------------------------------------------

-- dofile(simpl_datapath() .. "weapons/defs.lua")

Weapons["sword"]  = { 
   name         = "sword", 
   plural_name  = "swords", 
   hands        = 1, 
   range        = 10,
   damage_min   = 2,
   damage_max   = 6,
   damage_bonus = 0,
   weight       = 2,
   icon         = 593,
   gold         = 10
}

-- simpl_pushweapon(Weapons.sword)
-- print("lua: sword.lua: added weapon " .. Weapons.sword.name)
