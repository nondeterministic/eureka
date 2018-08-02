function with_weapon(monster, attacked_player_name)
   if (monster.distance <= monster.weapon.range) then
      r = simpl_rand(1, 20) - simpl_bonus(monster.luck) - simpl_bonus(monster.dxt)
      attack_successful = r < simpl_get_ac(attacked_player_name)
      
      if (attack_successful) then
         simpl_notify_party_hit()
         damage = simpl_rand(monster.weapon.damage_min, monster.weapon.damage_max) +
            simpl_bonus(monster.strength) +
            simpl_bonus(monster.weapon.damage_bonus)
         simpl_printcon(string.format("A %s hits %s with a %s for %d points of damage.",
                                      monster.name, attacked_player_name, monster.weapon.name, damage),
                        true)
         simpl_player_change_hp(attacked_player_name, -damage)
      else
         simpl_printcon(string.format("A %s tries to hit %s but misses.",
                                      monster.name, attacked_player_name),
                        true)
      end
   else
      simpl_printcon(string.format("A %s aims at %s but cannot reach.",
                                   monster.name, attacked_player_name), true)

   end
end
