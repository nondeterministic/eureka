function fleeing(monster)
   if (monster.hp < (monster.hp_max / 100 * 20)) then
      simpl_flee(monster.number)
      return true
   else
      return false
   end
end
