-- -----------------------------------------------
-- Troll definition file for eureka
-- Copyright (c) Andreas Bauer <a@pspace.org>
-- -----------------------------------------------

require "string"
require "math"

do
   local monster = {}

   monster.name        = "Dragon"
   monster.plural_name = "Dragons"
   monster.img_path    = ""

   monster.hp          = 0
   monster.hp_max      = 0
   monster.weapon      = nil
   monster.strength    = 16
   monster.luck        = 12
   monster.dxt         = 12
   monster.distance    = 0
   monster.gold        = 5
   monster.ep          = 10
   monster.sp          = 60
   monster.sp_max      = 60
   
   monster.distance    = 0
   monster.combat      = ""
   monster.nth_foe     = 0

   -- ---------------------------------------------------------------------
   -- true if, when given a chance to, the foe rather advances
   -- than fights.  If false, it indicates that the foe
   -- can actually fight from the distance, say, with a long range
   -- weapon or with a magic spell, etc.
   monster.advance     = true

   -- Every monster, right now, needs a carbon copy of this function.
   function get_name()
      return monster.name
   end
   -- ---------------------------------------------------------------------
   
   monster.create_instance = function()
      monster.hp_max   = simpl_rand(3, 18) + 15
      monster.hp       = monster.hp_max
      monster.strength = simpl_rand(3, 18) + 15
      monster.luck     = simpl_rand(3, 18) + 5
      monster.dxt      = simpl_rand(3, 18) + 5
      monster.img_path = simpl_datapath() .. "/bestiary/dragon.png"
      monster.gold     = simpl_rand(500, 1000) + 200
   end

   monster.set_combat_ptr = function(ptr, number)
      monster.combat  = ptr
      monster.nth_foe = number
      simpl_set_combat_ptr(monster.combat)
   end

   monster.attack = function() 
      if (simpl_rand(1, 6) < 4) then
         dofile(simpl_datapath() .. "/bestiary/attacks/odem.lua")
         odem(monster)
      else
         dofile(simpl_datapath() .. "/bestiary/attacks/without_weapon.lua")
         attacked_player_name = simpl_rand_player()
         without_weapon(monster, attacked_player_name)
      end
   end
end
