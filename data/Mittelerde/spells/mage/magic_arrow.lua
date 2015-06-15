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

   local name        = "magic arrow"
   local level       = 1
   local sp          = 5
   local caster      = ""
   local targets     = 1                     -- see comments inside spell.hh
   
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
      return simpl_datapath() .. "sound/shallow_water.wav"
   end

   function do_attack(param)
      simpl_magic_attack(param.foes, 
						 param.resistance, 
						 param.range, 
			 			 param.dmg, 
	         			 param.spell_lasts)
   end

   -- ---------------------------------------------------------------------------------
   -- Cast spell

   function cast()
      if (simpl_party_in_combat()) then
         attack_monsters()
      else
      	 simpl_printcon(string.format("%s casts a magic arrow spell, but nothing happens", caster), true)
	 	 do return end
   	  end
   end

   function attack_monsters()
      simpl_printcon(string.format("%s casts a magic arrow spell...", caster), true)
      do_attack{
  	  	foes = 1,                 -- how many opponents can be attacked in one round by the spell
	 	resistance = -10,         -- if negative, then the opponent has less resistance against spell
		range = 30,               -- max range of the spell
		dmg = simpl_rand(1, 5),   -- damage
		spell_lasts = 1           -- spell could last multiple rounds and do damage again and again
		}
   end

   -- ---------------------------------------------------------------------------------
      
end
