-- -----------------------------------------------
-- Troll definition file for eureka
-- Copyright (c) Andreas Bauer <a@pspace.org>
-- -----------------------------------------------

require "string"
require "math"

do
   local hp          = 0
   local hp_max      = 0
   local weapon      = nil
   local strength    = 25
   local luck        = 18
   local dxt         = 15
   local distance    = 0
   local gold        = 10
   local ep          = 18
   local sp          = 40
   
   local name        = "Beholder"
   local plural_name = "Beholders"
   local distance    = 0
   
   local combat      = ""
   local nth_foe     = 0

   local attacked_players = {} -- Names of party members who are attacked

   function create_instance()
      hp_max   = simpl_rand(30, 40) + 5
      hp       = hp_max
      weapon   = nil
      strength = simpl_rand(5, 15) + 5
      luck     = simpl_rand(5, 12)
      gold     = simpl_rand(50, 100)
   end

   function set_combat_ptr(ptr, number)
      combat  = ptr
      nth_foe = number
      simpl_set_combat_ptr(combat)
   end

   function get_name() 
      return name
   end

   function get_plural_name()      
      return plural_name
   end

   function get_ep()
      return ep
   end

   function set_gold(new_gold)
      gold = new_gold
   end

   function get_gold()
      return gold
   end

   function get_distance()
      return distance
   end

   function set_distance(new_dist)
      distance = new_dist
   end

   function img_path()
      return simpl_datapath() .. "/bestiary/beholder.png"
   end
   
   function get_hp()
      return hp
   end

   function set_hp(new_hp)
      hp = new_hp
   end

   function get_hp_max()
      return hp_max
   end

   function set_hp_max(new_hp_max)
      hp_max = new_hp_max
   end

   function get_strength()
      return strength
   end

   function get_luck()
      return luck
   end

   function set_luck(new_luck)
      luck = new_luck
   end

   function get_dxt()
      return dxt
   end

   function set_dxt(newdxt)
      dxt = newdxt
   end

   function get_weapon()
      return ""
   end

   function set_weapon(name)
      weapon = Weapons[name]
   end

   -- Return true if, when given a chance to, the foe rather advances
   -- than fights.  If false is returned, it indicates that the foe
   -- can actually fight from the distance, say, with a long range
   -- weapon or with a magic spell, etc.

   function advance()
      return false
   end

   function flee()
      simpl_flee(nth_foe)
   end

   function attack()
      if (get_hp() < get_hp_max() / 100 * 20) then
	 flee()
	 return false
      end

      attacked_players = {}
      number_of_attacked_players = 0
      
      -- Determine who gets potentially attacked by, say, an odem spell.
      for i=1,(simpl_get_partysize() - 1) do
	 if (simpl_rand(1,10) < 6 and simpl_get_player_is_alive(simpl_get_player_name(i))) then
	    -- -10 is a magic bonus by the skeleton lord to make the spell more effective
	    random = simpl_rand(1,20) + simpl_bonus(get_luck()) - 10
	    if (random < simpl_get_ac(simpl_get_player_name(i))) then
	       attacked_players[simpl_get_player_name(i)] = true
	       number_of_attacked_players = number_of_attacked_players + 1
	    end
	 end
      end

      if (number_of_attacked_players == 0) then
	 simpl_printcon(string.format("A %s puts a deadly stare at you, but misses.", get_name()), true)
      end
      
      return (number_of_attacked_players > 0)
   end

   function fight()
      simpl_printcon(string.format("A %s puts a deadly stare at you...", get_name()), true)
      sp = sp - 5
      for k, v in pairs(attacked_players) do
	 r = simpl_rand(1, 20) - 10
	 attack_successful = r < simpl_get_ac(k)
	 
	 if (attack_successful == true) then
	    damage = simpl_rand(3, 18)
	    simpl_printcon(string.format("...and %s takes %d points of damage.", k, damage), true)
	    simpl_player_change_hp(k, -damage)
	    simpl_notify_party_hit()
	 else
	    simpl_printcon(string.format("...and %s, who was directly targetted, managed to evade the attack.", k), true)
	 end
      end
   end
   
end
