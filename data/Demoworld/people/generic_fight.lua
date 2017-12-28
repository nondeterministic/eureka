-- -------------------------------------------------
-- Generic combat definition file for eureka
-- Copyright (c) Andreas Bauer <baueran@gmail.com>
-- -------------------------------------------------

do
   local player_name = "" -- Name of party member who is attacked
   local distance    = 0
   
   function flee()
      simpl_flee(nth_foe)
   end

   function img_path()
      return simpl_datapath() .. "/bestiary/generic_fight.png"
   end

   function set_distance(new_dist)
      distance = new_dist
   end

   function get_distance()
      return distance
   end

   function get_name() 
      return c_values["name"]
   end

   function get_plural_name()      
      return c_values["name"] .. "s"
   end

   function attack()
      if (c_values["hp"] < c_values["hpm"] / 100 * 20) then
	 flee()
	 return false
      end

      player_name = simpl_rand_player(1) -- Get exactly 1 random player to attack

      r = simpl_rand(1, 20) - simpl_bonus(c_values["luck"]) - simpl_bonus(c_values["dxt"])
      attack_successful = r < simpl_get_ac(player_name)
      
      if (attack_successful == false) then
	 simpl_printcon(string.format("%s tries to attack %s but misses.", 
				      c_values["name"], player_name), true)
      end
		     
      return attack_successful
   end

   function fight()
      if (distance > 10) then
      	 simpl_printcon(string.format("%s tries to attack %s but cannot reach.",
      				      c_values["name"], player_name), true)
      	 return
      end
      
      wep = c_values["weapon"]
      
      if (not(wep == nil)) then
	 damage = simpl_rand(wep.damage_min, wep.damage_max)
	 simpl_printcon(string.format("%s swings the %s and hits %s for %d points of damage.",
				      c_values["name"], wep.name, player_name, damage), true)

	 simpl_player_change_hp(player_name, -damage)
      else
	 damage = simpl_rand(1, 3) -- default for bare hands. TODO: Add c_values player properties!
	 simpl_printcon(string.format("Using bare hands, %s attacks %s for %d points of damage.",
				      c_values["name"], player_name, damage), true)

	 simpl_player_change_hp(player_name, -damage)	 
      end
      
      simpl_notify_party_hit()
   end
	
end
j
