function plain_encounter()
   -- 5% chance for an encounter in the plain terrain
   return simpl_rand(1,100) <= 5
end

function forest_encounter()
   -- 20% chance for an encounter in the forest terrain
   return simpl_rand(1,100) <= 20
end

function water_encounter()
   -- 5% chance for an encounter in the water terrain
   return simpl_rand(1,100) <= 5
end

function deep_water_encounter()
   return water_encounter()
end

-- ---------------------
-- Random monsters table
-- ---------------------

-- TODO: Put the information below in a separate table, and then randomly return an entry from the table instead

function rand_encounter(terrain)
   random = simpl_rand(1, 100)

   if (terrain == "plain" and plain_encounter()) then
      if (random <= 100) then
	 return { 
	    { __name = "Orc", 
	      __distance = simpl_rand(1, 4) * 10, 
	      __number = simpl_rand(1, 5) },
	 --    { __name = "Orc", 
	 --      __distance = math.random(1, 4) * 10, 
	 --      __number = math.random(1, 5) },
	    { __name = "Troll", 
	      __distance = -1, 
	      __number = math.random(0, 2) }
	 }
      end
   else
      return {} -- { { "a", 1 }, { "b", 2 }, { "c", 3 } }
   end
end
