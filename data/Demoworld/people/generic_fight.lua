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

   function attack()
      if (c_values["hp"] < c_values["hpm"] / 100 * 20) then
	 	flee()
	 	return false
      end

      player_name = simpl_rand_player(1) -- Get exactly 1 random player to attack

      r = simpl_rand(1, 20) - simpl_bonus(c_values["luck"]) - simpl_bonus(c_values["dxt"])
      attack_successful = r < simpl_get_ac(player_name)
      
      if (attack_successful == false) then
      	 simpl_printcon(string.format("%s swings the %s at %s but misses.", 
      				      c_values["name"], get_weapon(), player_name), true)
      end
		     
      return attack_successful
   end

   function fight()
      wep = c_values["weapon"]

      if (distance > 10) then
      	 simpl_printcon(string.format("%s swings the %s at %s but cannot reach.",
      				      c_values["name"], wep.name, player_name), true)
      	 return
      end

      damage = simpl_rand(wep.damage_min, wep.damage_max)
      simpl_printcon(string.format("%s swings the %s and hits %s for %d points of damage.",
      				   c_values["name"], wep.name, player_name, damage), true)

      simpl_player_change_hp(player_name, -damage)
      simpl_notify_party_hit()
   end
	
end
