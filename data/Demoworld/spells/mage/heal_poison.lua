-- -----------------------------------------------
-- Spell definition file for eureka
-- Copyright (c) Andreas Bauer <baueran@gmail.com>
-- -----------------------------------------------

require "string"
require "math"

-- NOTE THAT:
-- If you change something here, look at the spell.cc function that loads the spell file!

do
   -- ---------------------------------------------------------------------------------
   -- Standard variables

   local name        = "heal poison"
   local level       = 3
   local sp          = 8
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
	 
	 if (simpl_get_player_condition(player_name) == "POISONED") then
	    simpl_play_sound(get_sound_path())
	    simpl_set_player_condition(player_name, "GOOD")
	    
	    -- If in combat, then the printcon is showing letter by letter, otherwise all at once.
	    simpl_printcon(caster .. " casts a spell, and " .. player_name .. " feels much better.", simpl_party_in_combat())
	 else
	    simpl_printcon(caster .. " casts a spell, and nothing happens.", simpl_party_in_combat())
	 end   
      else
	 simpl_printcon("Something went wrong, casting the spell. It is a program error.")
      end
   end

   -- Choose target for attack or party member, e.g., for healing. Is empty for, say, a light spell.

   function choose()
      chosen_player = simpl_choose_player()
       
      if (chosen_player >= 0) then
         if (not(simpl_is_dead(chosen_player))) then
	    targets = chosen_player
	 else
	    simpl_printcon("That would require more than healing of poison...")
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
