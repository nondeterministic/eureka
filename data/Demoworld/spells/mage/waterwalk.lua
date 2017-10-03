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

   local name        = "walk over water"
   local level       = 3
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
	 return false
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
      simpl_printcon(caster .. " casts a walk over water spell, and walking suddenly becomes light as a feather...")
      simpl_play_sound(get_sound_path(), 1)
      simpl_make_icon_walkable(2)  -- 2 is the icon of shallow water, see Demoworld.xml	 
   end

   -- Choose target for attack or party member, e.g., for healing. Is empty for, say, a light spell.

   -- function choose()
   -- end

   -- Spells that have a duration for multiple rounds need to define the following 'constructor' and 'destructor' functions. Other spells leave this blank!

   function init()
      simpl_set_spell_duration_party(caster, name, 10)
   end
   
   function finish(player)
      simpl_remove_icon_walkable(2)  -- 2 is the icon of shallow water, see Demoworld.xml	 
      simpl_printcon("Walking suddenly feels heavy again...")
   end

   -- ---------------------------------------------------------------------------------

end
