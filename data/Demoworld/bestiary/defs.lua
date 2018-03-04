function plain_encounter()
   -- 3 - 10% chance for an encounter in the plain terrain
   
   day_bonus = 7
   if (not(simpl_is_day())) then
      day_bonus = 0 -- it is more likely to get attacked at night
   end
   
   return simpl_rand(1,100) <= (10 - day_bonus)
end

function forest_encounter()
   -- 5 - 15% chance for an encounter in the forest terrain

   day_bonus = 10
   if (not(simpl_is_day())) then
      day_bonus = 0 -- it is more likely to get attacked at night
   end
   
   return simpl_rand(1,100) <= (15 - day_bonus)
end

function water_encounter()
   -- 5% chance for an encounter in the water terrain
   return simpl_rand(1,100) <= 5
end

function dungeon_encounter()
   -- 15% chance for an encounter in the dungeon
   return simpl_rand(1,100) <= 15
end

function deep_water_encounter()
   return water_encounter()
end

function get_default_icon(monster_name)
   icons = {}
   icons["ORC"] = 300 
   icons["TROLL"] = 333 
   icons["SPIDER"] = 233
   icons["SKELETON"] = 304
   icons["SKELETON LORD"] = 357
   icons["ETTIN"] = 325
   icons["WOLF"] = 366
   
   if (icons[string.upper(monster_name)]) then
      return icons[string.upper(monster_name)]
   else
      print("bestiary/defs.lua problem! No default icon for" .. monster_name)
      return 0 -- This should never happen!
   end
end

-- ---------------------
-- Random monsters table
-- ---------------------

-- TODO: Put the information below in a separate table, and then randomly return an entry from the table instead

function rand_encounter(terrain)
   random = simpl_rand(1, 100)

   if (terrain == "plain" and plain_encounter()) then
      if (math.random(1,2) == 1) then
	 return {
	    { __name = "Orc",
	      __distance = simpl_rand(1, 4) * 10,
	      __number = simpl_rand(1, 3) },
	    { __name = "Dark Knight", 
	      __distance = math.random(1, 4) * 10, 
	      __number = math.random(0, 2) },
	    { __name = "Troll",
	      __distance = -1,
	      __number = math.random(0, 2) }
	 }
      else
	 return {
	    { __name = "Wolf",
	      __distance = simpl_rand(1, 4) * 10,
	      __number = simpl_rand(2, 5) }
	 }
      end
   elseif ((terrain == "forest" or terrain == "rocks") and forest_encounter()) then
      if (math.random(1,2) == 1) then
	 return { 
	    { __name = "Orc", 
	      __distance = simpl_rand(1, 4) * 10, 
	      __number = simpl_rand(1, 5) },
	    { __name = "Troll", 
	      __distance = -1, 
	      __number = math.random(0, 2) }
	 }
      else
	 return {
	    { __name = "Wolf",
	      __distance = simpl_rand(1, 4) * 10,
	      __number = simpl_rand(2, 5) }
	 }
      end
   elseif (terrain == "dungeon" and dungeon_encounter()) then
      if (random <= 30) then
         return {
            { __name = "Orc",
              __distance = simpl_rand(1, 4) * 10,
              __number = simpl_rand(1, 2) },
            { __name = "Troll",
              __distance = -1,
              __number = math.random(0, 2) }
         }
      elseif (random <= 50) then
         return {
            { __name = "Skeleton",
              __distance = simpl_rand(1, 4) * 10,
              __number = simpl_rand(1, 3) },
            { __name = "Evil Wizard",
              __distance = simpl_rand(1, 4) * 10,
              __number = simpl_rand(0, 3) },
            { __name = "Skeleton Lord",
              __distance = -1,
              __number = math.random(0, 2) * math.random(0, 1) }
         }
      elseif (random <= 80) then
         return {
            { __name = "Spider",
              __distance = simpl_rand(1, 4) * 10,
              __number = simpl_rand(1, 2) }
         }
      else
         return {
            { __name = "Ettin",
              __distance = simpl_rand(1, 4) * 10,
              __number = simpl_rand(1, 3) },
            { __name = "Beholder",
              __distance = simpl_rand(1, 4) * 10,
              __number = simpl_rand(0, 3) },
            { __name = "Skeleton Lord",
              __distance = -1,
              __number = math.random(0, 2) * math.random(0, 1) }
         }	 
      end
   else
      return {} -- { { "a", 1 }, { "b", 2 }, { "c", 3 } }
   end
end
