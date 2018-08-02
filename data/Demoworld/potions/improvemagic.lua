-- -----------------------------------------------
-- MiscItems definition file for eureka
-- Copyright (c) Andreas Bauer <a@pspace.org>
-- -----------------------------------------------

Potions["improve-magic potion"]  = { 
   name                   = "improve-magic potion",
   plural_name            = "improve-magic potions",
   description            = "",
   
   food_up                = 0,
   healing_power          = "none",
   poison_healing_power   = "none",
   poisonous              = "none",
   magic_herb             = false,
   intoxicating_effect    = "none",

   gold                   = 20,
   weight                 = 0,
   icon                   = 703,

   ingredients            = { Edibles.garlic.name, Edibles.arnica.name,
                              Edibles.sulphur.name, Edibles.thuja.name,
                              Edibles.gelsemium.name },

   -- The above data entries are identical to Edibles, hence Potions is derived from Edibles in the code.

   effect = function(chosen_player)
      if (simpl_add_sp(chosen_player, math.random(15,35))) then
         simpl_play_sound("spell_normal.wav")
         simpl_printcon(simpl_get_player_name(chosen_player) .. " feels a lot more focused suddenly..")
         simpl_ztatsupdate()
      else
         simpl_printcon("The effect of the potion was not noticeable.")
      end
   end
}
