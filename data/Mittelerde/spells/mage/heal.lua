-- -----------------------------------------------
-- Spell definition file for simplicissimus
-- Copyright (c) Andreas Bauer <baueran@gmail.com>
-- -----------------------------------------------

require "string"
require "math"

-- NOTE THAT:
-- If you change something here, look at the spell.cc function that loads the spell file!

do
   -- ---------------------------------------------------------------------------------
   -- Standard variables

   local name        = "heal light wounds"
   local level       = 1
   local sp          = 5
   local caster      = ""
   local targets     = 0                     -- see comments inside spell.hh
   
   -- ---------------------------------------------------------------------------------
   -- Standard functions

   function set_caster(c_name)
      caster = c_name
   end

   function get_sp()
      return sp
   end

   function get_name()
      return name
   end

   function get_level()
      return level
   end

   function get_targets()
      return targets
   end
   
   function get_sound_path()
      return simpl_datapath() .. "sound/spell_normal.wav"
   end

   -- ---------------------------------------------------------------------------------
   -- Cast spell

   function cast()
      chosen_player = simpl_choose_player()
       
      if (chosen_player >= 0) then
         if (not(simpl_is_dead(chosen_player))) then
         	simpl_play_sound(get_sound_path())
	     	simpl_add_hp(chosen_player, simpl_rand(1, 5))
	     else
	     	simpl_printcon("That would require more than healing of light wounds...")
	     end
	  else
	     simpl_printcon("Changed your mind then?")
	  end
   end

   -- ---------------------------------------------------------------------------------

end
