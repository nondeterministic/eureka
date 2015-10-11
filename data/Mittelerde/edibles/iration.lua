-- -----------------------------------------------
-- Edibles definition file for simplicissimus
-- Copyright (c) Andreas Bauer <baueran@gmail.com>
-- -----------------------------------------------

-- dofile(simpl_datapath() .. "weapons/defs.lua")

Edibles["iron ration"]  = { 
   name                   = "iron ration", 
   plural_name            = "iron rations", 

   food_up                = 200,
   healing_power          = "none",
   poison_healing_power   = "none",
   poisonous              = "none",
   magic_herb             = false,
   intoxicating_effect    = "none",

   gold                   = 3,
   weight                 = 2,
   icon                   = 5
}

-- simpl_pushweapon(Weapons.axe)
-- print("lua: axe.lua: added weapon " .. Weapons.axe.name)
