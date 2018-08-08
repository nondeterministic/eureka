require "math"

function without_weapon(monster, attacked_player_name, poison_attack)
   poison_attack = poison_attack or false -- default argument for poison attack, cf. https://stackoverflow.com/questions/6022519/define-default-values-for-function-arguments
   
   if (monster.distance <= 10) then
      r = simpl_rand(1, 20) - simpl_bonus(monster.luck) - simpl_bonus(monster.dxt)
      attack_successful = r < simpl_get_ac(attacked_player_name)

      if (attack_successful) then
         simpl_notify_party_hit()
         damage = math.max(1, simpl_rand(1,6) + simpl_bonus(monster.strength))
         simpl_printcon(string.format("A %s hits %s for %d points of damage.",
                                      monster.name, attacked_player_name, damage),
                        true)

         -- Check if unpoisoned player received a poison attack...
         if (poison_attack == true and simpl_rand(1,6) <= 3 and not(string.find(simpl_get_player_condition(attacked_player_name), "POISON"))) then
            simpl_printcon(string.format("%s is starting to feel seriously ill...", attacked_player_name), true)
            simpl_set_player_condition(attacked_player_name, "POISONED")
         end

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
