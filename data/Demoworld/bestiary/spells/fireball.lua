function cast_fireball(monster)
   attacked_players = {}
   number_of_attacked_players = 0

   monster.sp = monster.sp - 5
   
   -- Determine who gets potentially attacked by, say, an odem spell.
   for i=1,(simpl_get_partysize() - 1) do
      if (simpl_rand(1,10) < 6 and simpl_get_player_is_alive(simpl_get_player_name(i))) then
         r = simpl_rand(1, 20) - simpl_bonus(monster.luck)
         if (r < simpl_get_ac(simpl_get_player_name(i))) then -- TODO: add magic-protection bonus for player!
            attacked_players[simpl_get_player_name(i)] = true
            number_of_attacked_players = number_of_attacked_players + 1
         end
      end
   end
      
   if (number_of_attacked_players == 0) then
      simpl_printcon(string.format("A %s casts a spell, but nothing happens.", monster.name), true)
      return
   end

   simpl_printcon(string.format("A %s casts a fireball spell...", monster.name), true)
   for k, v in pairs(attacked_players) do
      r = simpl_rand(1, 20) - 10
      attack_successful = r < simpl_get_ac(k)
      
      if (attack_successful == true) then
         damage = simpl_rand(1, 8)
         simpl_printcon(string.format("%s takes %d points of damage.", k, damage), true)
         simpl_player_change_hp(k, -damage)
         simpl_notify_party_hit()
      else
         simpl_printcon(string.format("%s, who was directly targetted, managed to evade the attack.", k), true)
      end
   end
end
