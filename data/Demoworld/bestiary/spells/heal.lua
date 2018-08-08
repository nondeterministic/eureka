require "math"

function cast_heal(monster)
   simpl_printcon(string.format("A %s casts a healing spell and recovers.", monster.name), true)
   monster.hp = math.min(monst.hp_max, monster.hp + simpl_rand(1,6))
   monster.sp = monster.sp - 5
end
