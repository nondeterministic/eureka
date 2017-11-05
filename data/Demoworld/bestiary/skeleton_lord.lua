-- -----------------------------------------------
-- Troll definition file for eureka
-- Copyright (c) Andreas Bauer <baueran@gmail.com>
-- -----------------------------------------------

require "string"
require "math"

do
   local hp          = 0
   local hp_max      = 0
   local weapon      = nil
   local strength    = 0
   local luck        = 0
   local dxt         = 0
   local distance    = 0
   local gold        = 10
   local ep          = 18
   local sp          = 20
   
   local name        = "Skeleton Lord"
   local plural_name = "Skeleton Lords"
   local distance    = 0
   
   local combat      = ""
   local nth_foe     = 0

   local attacked_players = {} -- Names of party members who are attacked

   function create_instance()
      hp_max   = 1
      -- hp_max   = simpl_rand(5, 14) + 4
      hp       = hp_max
      weapon   = Weapons.axe
      strength = simpl_rand(5, 14) + 4
      luck     = simpl_rand(5, 12)
      gold     = simpl_rand(1, 2)  -- An Orc may carry up to 2 gold coins
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
      return simpl_datapath() .. "/bestiary/skeleton.png"
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
      return weapon.name
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
	    random = simpl_rand(1,20) - simpl_bonus(get_luck()) - simpl_bonus(get_dxt())
	    if (random < simpl_get_ac(simpl_get_player_name(i))) then -- TODO: add magic-protection bonus for player!
	       attacked_players[simpl_get_player_name(i)] = true
	       number_of_attacked_players = number_of_attacked_players + 1
	    end
	 end
      end

      if (number_of_attacked_players == 0) then
	 simpl_printcon(string.format("A %s casts a spell, but nothing happens.", get_name()), true)
      end
      
      return (number_of_attacked_players > 0)
   end

   function fight()
      if (sp < 5) then -- Fight with weapon, if spell points are low
	 wep = Weapons[get_weapon()]

	 if (distance > 10) then
	    simpl_printcon(string.format("A %s tries to attack with an %s, but cannot reach.",
					 get_name(), wep.name), true)
	    return
	 else
	    player_name = simpl_rand_player(1)
	    damage = simpl_rand(wep.damage_min, wep.damage_max)
	    simpl_printcon(string.format("A %s swings his %s and hits %s for %d points of damage.",
					 get_name(), wep.name, player_name, damage), true)
	    simpl_player_change_hp(player_name, -damage)
	    simpl_notify_party_hit()
	 end
      else -- Cast spell
	 simpl_printcon(string.format("A %s casts a magic arrows spell...", get_name()), true)
	 sp = sp - 5
	 for k, v in pairs(attacked_players) do
	    damage = simpl_rand(1, 4)
	    simpl_printcon(string.format("%s takes %d points of damage.", k, damage), true)
	    simpl_player_change_hp(k, -damage)
	    simpl_notify_party_hit()
	 end
      end
   end
   
end
