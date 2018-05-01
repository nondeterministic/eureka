-- -----------------------------------------------
-- MiscItems definition file for eureka
-- Copyright (c) Andreas Bauer <baueran@gmail.com>
-- -----------------------------------------------

Potions["heal-poison potion"]  = { 
   name                   = "heal-poison potion",
   plural_name            = "heal-poison potions",
   description            = "",
   
   food_up                = 0,
   healing_power          = "some",
   poison_healing_power   = "some",
   poisonous              = "none",
   magic_herb             = false,
   intoxicating_effect    = "none",

   gold                   = 1,
   weight                 = 0,
   icon                   = 702,

   ingredients            = { Edibles.garlic.name, Edibles.thuja.name, Edibles.chamomilla.name },

   -- The above data entries are identical to Edibles, hence Potions is derived from Edibles in the code.

   effect = function(chosen_player)
      if (simpl_add_hp(chosen_player, math.random(3,7))) then
         simpl_play_sound("spell_normal.wav")
         simpl_printcon(simpl_get_player_name(chosen_player) .. " feels the full effect of the healing potion.")
         simpl_ztatsupdate()
      else
         simpl_printcon("The effect of the heal-poison potion was positive, but not overwhelmingly so.")
      end
   end
}
