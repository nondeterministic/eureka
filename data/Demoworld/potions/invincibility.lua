-- -----------------------------------------------
-- MiscItems definition file for eureka
-- Copyright (c) Andreas Bauer <baueran@gmail.com>
-- -----------------------------------------------

Potions["potion of invincibility"]  = { 
   name                   = "potion of invincibility",
   plural_name            = "potions of invincibility",
   description            = "",
   
   food_up                = 0,
   healing_power          = "none",
   poison_healing_power   = "none",
   poisonous              = "none",
   magic_herb             = false,
   intoxicating_effect    = "none",

   gold                   = 1,
   weight                 = 0,
   icon                   = 706,

   ingredients            = { Edibles.nightshade.name, Edibles.sulphur.name },

   -- The above data entries are identical to Edibles, hence Potions is derived from Edibles in the code.
   
   effect = function(chosen_player)
      -- simpl_play_sound(get_sound_path())
      -- simpl_add_hp(targets, simpl_rand(1, 5))
      
      -- Lasts 25 rounds.
      simpl_walk_through_fields(25)
      simpl_printcon("Your party feels strangely immune...")
   end
}
