-- -----------------------------------------------
-- MiscItems definition file for eureka
-- Copyright (c) Andreas Bauer <baueran@gmail.com>
-- -----------------------------------------------

Potions["healing potion"]  = { 
   name                   = "healing potion",
   plural_name            = "healing potion",
   description            = "",
   
   food_up                = 0,
   healing_power          = "some",
   poison_healing_power   = "some",
   poisonous              = "none",
   magic_herb             = false,
   intoxicating_effect    = "none",

   gold                   = 1,
   weight                 = 0,
   icon                   = 988,

   ingredients            = { Edibles.garlic.name, Edibles.sulphur.name },

   -- The above data entries are identical to Edibles, hence Potions is derived from Edibles in the code.

   effect = function(chosen_player)
      if (simpl_add_hp(chosen_player, math.random(3,7))) then
	 simpl_printcon(simpl_get_player_name(chosen_player) .. " feels the full effect of the healing potion.")
      else
	 simpl_printcon("The effect of the healing potion was positive, but not overwhelmingly so.")
      end
   end
}
