-- -----------------------------------------------
-- MiscItems definition file for eureka
-- Copyright (c) Andreas Bauer <baueran@gmail.com>
-- -----------------------------------------------

Potions["heal"]  = { 
   name                   = "healing potion",
   plural_name            = "healing potion",
   description            = "",
   icon                   = 583,
   
   food_up                = 0,
   healing_power          = "some",
   poison_healing_power   = "some",
   poisonous              = "none",
   magic_herb             = false,
   intoxicating_effect    = "none",

   -- The above data entries are identical to Edibles, hence Potions is derived from Edibles in the code.
   
   weight                 = 1,
   name_of_potion_drinker = "",
   effect = function()
      simpl_play_sound(get_sound_path())
      simpl_add_hp(targets, simpl_rand(1, 5))
	 
      -- If in combat, then the printcon is showing letter by letter, otherwise all at once.
      simpl_printcon(name_of_potion_drinker .. " drinks a healing potion and feels much better.", simpl_party_in_combat())
   end
}
