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

   local name        = "light"
   local level       = 1
   local sp          = 2
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

   function get_targets()
      return targets
   end
   
   function get_sound_path()
      return simpl_datapath() .. "sound/spell_normal.wav"
   end

   -- ---------------------------------------------------------------------------------
   -- Cast spell

   -- Does the actual casting of spell, after choose() was called
   
   function cast()
      simpl_set_magic_light_radius(4)
   end

   -- Choose target for attack or party member, e.g., for healing. Is empty for, say, a light spell.

   function choose()
   end

   -- Spells that have a duration for multiple rounds need to define the following 'constructor' and 'destructor' functions. Other spells leave this blank!

   function init()
      simpl_set_spell_duration_party(caster, name, 10)
   end
   
   function finish(player)
   	  simpl_printcon("Mooboo")
      simpl_set_magic_light_radius(0)
   end
   
   -- ---------------------------------------------------------------------------------

end
