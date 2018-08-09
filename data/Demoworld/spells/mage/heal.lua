-- -----------------------------------------------
-- Spell definition file for eureka
-- Copyright (c) Andreas Bauer <a@pspace.org>
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
   local targets     = -1                     -- see comments inside spell.hh

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

   function is_attack_spell()
     return false
   end

   function is_choose_function_defined()
      if (choose == nil) then
	 return false
      else
	 return true
      end
   end   

   function get_targets()
      return targets
   end
   
   function get_sound_path()
      return "spell_normal.wav"
   end
   
   -- ---------------------------------------------------------------------------------
   -- Cast spell

   -- Does the actual casting of spell, after choose() was called
   
   function cast()
      if (targets >= 0) then
         player_name = simpl_get_player_name(targets)
         
         simpl_play_sound(get_sound_path())
         simpl_add_hp(targets, simpl_rand(1, 5))
         
         -- If in combat, then the printcon is showing letter by letter, otherwise all at once.
         simpl_printcon(caster .. " casts a spell, and " .. player_name .. " feels much better.", simpl_party_in_combat())
      else
         simpl_printcon("No suitable recipient chosen.")
      end
   end

   -- Choose target for attack or party member, e.g., for healing. Is empty for, say, a light spell.

   function choose()
      chosen_player = simpl_choose_player()
      
      if (chosen_player >= 0) then
         if (not(simpl_is_dead(chosen_player))) then
            targets = chosen_player
         else
            simpl_printcon("That would require more than healing of light wounds...")
         end
      else
         simpl_printcon("Changed your mind then?")
      end
   end
   
   -- Spells that have a duration for multiple rounds need to define the following 'constructor' and 'destructor' functions. Other spells leave this blank!
   
   function init()
   end
   
   function finish(player)
   end
   
   -- ---------------------------------------------------------------------------------
   
end
