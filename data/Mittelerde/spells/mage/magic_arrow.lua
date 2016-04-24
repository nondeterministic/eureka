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
   local targets     = -1                     -- see comments inside spell.hh
   local combat_ptr  = ""
   
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
     return true
   end

   function get_targets()
      return targets
   end
   
   function get_sound_path()
      return "shallow_water.wav"
   end

   function set_combat_ptr(ptr)
      combat_ptr = ptr
   end

   function do_attack(param)
      simpl_magic_attack(param.targets,
			 param.attack_whole_group, 
			 param.resistance, 
			 param.range, 
			 param.dmg, 
			 param.spell_lasts,
			 param.playername)
   end

   -- ---------------------------------------------------------------------------------
   -- Cast spell

   -- Does the actual casting of spell, after choose() was called

   function cast()
      if (simpl_party_in_combat()) then
         attack_monsters()
      else
    	 simpl_printcon(string.format("%s casts a magic arrow spell, but nothing happens.", caster))
      end
   end

   -- Choose target for attack or party member, e.g., for healing. Is empty for, say, a light spell.
   -- PRECONDITION: combat_ptr must be assigned here already, as choose member uses a function from Combat!
   
   function choose()
      simpl_set_combat_ptr(combat_ptr)  -- This is crucial, if we want to know any attributes of our attackers!
      
      targets = simpl_choose_monster()
      
      if (targets < 0) then
	 simpl_printcon("Changed your mind then?")
      end
   end

   -- Helper function, do not call from C directly!  Only used within the script.
   -- PRECONDITION: combat_ptr must be assigned here already, as choose member uses a function from Combat!
   
   function attack_monsters()
      if (targets < 0) then
	 simpl_printcon(string.format("%s attempted a spell, but is instead passing this battle round.", caster), true)
      else
	 name    = simpl_get_single_monster_name(targets)
	 damage  = simpl_rand(1, 5)
	 
	 -- simpl_printcon(string.format("%s casts a magic arrow spell, causing the %s %d points of damage.", caster, name, damage), true)
         
         do_attack{
	    targets = 1,                -- how many opponents can be attacked in one round by the spell
	    attack_whole_group = false, -- if true, then the whole group is attacked, not just one individual in the group
	    resistance = -10,           -- if negative, then the opponent has less resistance against spell
	    range = 30,                 -- max range of the spell
	    dmg = damage,               -- damage
	    spell_lasts = 1,            -- spell could last multiple rounds and do damage again and again, e.g., poison, charming, sleep, etc.
	    playername = caster         -- Spell caster name
	 }
      end
   end

   -- Spells that have a duration for multiple rounds need to define the following 'constructor' and 'destructor' functions. Other spells leave this blank!

   function init()
   end
   
   function finish(player)
   end

   -- ---------------------------------------------------------------------------------
      
end
