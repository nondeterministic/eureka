function plain_encounter()
   -- 5% chance for an encounter in the plain terrain
   
   day_bonus = 10
   if (not(simpl_is_day())) then
      day_bonus = 0 -- it is more likely to get attacked at night
   end
   
   return simpl_rand(1,100) <= (15 - day_bonus)
end

function forest_encounter()
   -- 20% chance for an encounter in the forest terrain

   day_bonus = 10
   if (not(simpl_is_day())) then
      day_bonus = 0 -- it is more likely to get attacked at night
   end
   
   return simpl_rand(1,100) <= (30 - day_bonus)
end

function water_encounter()
   -- 5% chance for an encounter in the water terrain
   return simpl_rand(1,100) <= 5
end

function dungeon_encounter()
   -- 20% chance for an encounter in the forest terrain
   return simpl_rand(1,100) <= 20
end

function deep_water_encounter()
   return water_encounter()
end

function get_default_icon(monster_name)
   icons = {}
   icons["orc"] = 300 
   icons["troll"] = 333 
   icons["spider"] = 233
   icons["skeleton"] = 304
   icons["skeleton_lord"] = 357
   
   if (icons[monster_name]) then
      return icons[monster_name]
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
      return { 
	 { __name = "orc", 
	   __distance = simpl_rand(1, 4) * 10, 
	   __number = simpl_rand(1, 5) },
	 --    { __name = "Orc", 
	 --      __distance = math.random(1, 4) * 10, 
	 --      __number = math.random(1, 5) },
	 { __name = "troll", 
	   __distance = -1, 
	   __number = math.random(0, 2) }
      }
   elseif ((terrain == "forest" or terrain == "rocks") and forest_encounter()) then
      return { 
	 { __name = "orc", 
	   __distance = simpl_rand(1, 4) * 10, 
	   __number = simpl_rand(1, 5) },
	 { __name = "troll", 
	   __distance = -1, 
	   __number = math.random(0, 2) }
      }
   elseif (terrain == "dungeon" and dungeon_encounter()) then
      if (random <= 30) then
	 return { 
	    { __name = "orc", 
	      __distance = simpl_rand(1, 4) * 10, 
	      __number = simpl_rand(1, 5) },
	    { __name = "troll", 
	      __distance = -1, 
	      __number = math.random(0, 2) }
	 }
      elseif (random <= 50) then
	 return { 
	    { __name = "skeleton", 
	      __distance = simpl_rand(1, 4) * 10, 
	      __number = simpl_rand(1, 3) },
	    { __name = "skeleton_lord", 
	      __distance = -1, 
	      __number = math.random(0, 2) * math.random(0, 1) }
	 }
      else -- if (random <= 80) then
	 return { 
	    { __name = "spider", 
	      __distance = simpl_rand(1, 4) * 10, 
	      __number = simpl_rand(1, 2) }
	 }	       
      end
   else
      return {} -- { { "a", 1 }, { "b", 2 }, { "c", 3 } }
   end
end
