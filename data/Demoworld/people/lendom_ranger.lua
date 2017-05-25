-- -----------------------------------------------
-- Conversation definition file for eureka
-- Copyright (c) Andreas Bauer <baueran@gmail.com>
-- -----------------------------------------------

do
   -- --------------------------------------------------------
   -- Define character values for either party-join or a fight
   -- --------------------------------------------------------

   c_values = { 
      name = "Alvar", race = "HUMAN", ep = 0, hp = 10, hpm = 10, sp = 0, spm = 0, str = 7, luck = 15, dxt = 15, wis = 4, charr = 14, 
      iq = 15, endd = 7, sex = "MALE", profession = "MAGE", weapon = Weapons["axe"], shield = Shields["small shield"] 
   } 

   conv_over = false
   
   -- -----------------------------------------------
   -- Standard functions
   -- -----------------------------------------------

   function get_weapon()
      return c_values["weapon"].name
   end

   function get_shield()
      return c_values["shield"].name
   end

   function load_generic_fight_file(name)
      dofile(name) -- To enable combat, must be inserted AFTER c_values is defined!
   end

   function conversation_over()
      return conv_over
   end

   -- -----------------------------------------------
   -- Standard terms
   -- -----------------------------------------------

   function description()
      simpl_printcon("You see a ranger carrying a bow and a dusty green robe.")
   end

   function name()
      simpl_printcon("They call me " .. c_values["name"] .. ".")
   end

   function job()
      simpl_printcon("Once I had a small farm not far from here, but the enemy has destroyed it and killed my family. I now help rebuild Lendom after having helped to defend it in the great war.")
   end
   
   function join()
      highest_level = 0
      
      for i=0,simpl_get_partysize() do
	 if (simpl_get_level(i) > highest_level) then
	    highest_level = simpl_get_level(i)
	 end
      end

      if (highest_level >= 3) then
	 simpl_printcon("You appear to be serious in your undertaking. I shall be honoured to join thee!")
	 return true
      else
	 simpl_printcon("A lot of people pass through and ask for help, but they do not know what they are getting themselves into. " ..
			"Become more experienced, and ask me in the future. I may or not help thee in thy quest then. Until then, be careful " ..
			"as the enemy has not been defeated. He merely sleeps...")
	 return false
      end
   end
  
   function bye()
      simpl_printcon("Take care, my friend.")
   end
   
   function otherwise(item)
      if (item == "enemy" or item == "war" or item == "family") then
	 simpl_printcon("It was a terrible battle and we all had lost a lot. But now we have to look forward and rebuild the town. We must think of those still alive, our children.")
      elseif (item == "children" or item == "alive" or item == "town") then
	 simpl_printcon("Tragedies have happened, but much has been rebuilt. A reminder of what happened you can find in the north of town. That used to be old Tork's farm house. " ..
			"All have been killed, except for his dog. It still roams up there somewhere, looking for its owners. It refuses to come to town and live with anyone else. " ..
			   "It is a good, loyal dog, who fought fiercly side by side with us.")
      elseif (item == "dog") then
	 simpl_printcon("Look for Tork's old dog up in the ruins north of town. But be a bit careful with his dog. He does not trust anyone after what happened... Dogs know and cannot forget.")
      else
	 simpl_printcon("I am sorry, I cannot help you with that.")
      end
   end
end
