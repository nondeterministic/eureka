-- -----------------------------------------------
-- Conversation definition file for eureka
-- Copyright (c) Andreas Bauer <baueran@gmail.com>
-- -----------------------------------------------

do
   -- --------------------------------------------------------
   -- Define character values for either party-join or a fight
   -- --------------------------------------------------------

   c_values = { 
      name = "Evelyn", race = "HUMAN", ep = 0, hp = 10, hpm = 10, sp = 0, spm = 0, str = 7, luck = 15, dxt = 15, wis = 4, charr = 14, 
      iq = 15, endd = 7, sex = "FEMALE", profession = "MAGE", weapon = Weapons["axe"], shield = Shields["small shield"] 
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

   pname = ""
   
   function description()
      simpl_printcon("You see your beloved wife with a dirty but overjoyed face.")
   end
 
   function name()
      simpl_printcon("I may look different in this prison, but I am still Evelyn your wife.")
   end
   
   function job()
      simpl_printcon("There was nothing to do inside these dark prison walls other than to pray for your arrival. " ..
		     "I do not know how you achieved what you have, but we must not loose any more time! We must find our way back to the surface.")
   end
   
   function join()
      simpl_printcon("Of course I shall join you, and although I am weak, I will need a weapon to defend myself and arms to protect myself from these monsters down here." ..
			"And before we save ourselves, we must make sure to free the others who were locked up with me down here. Do you promise not to leave without them (y/n)?")
      answer = simpl_getkey("yn")
      simpl_printcon(string.format("%s ", answer))
	 
      if (answer == "y") then
	 simpl_printcon("I knew you had a good heart and would not let the others down. I shall join thee in that fight and way back to the surface.")
	 join_successful = simpl_join(c_values)
	 simpl_remove_from_current_map("wife")
	 conv_over = true
	 return true
      else
	 simpl_printcon("Then, I am afraid, I rather die down here alone than cowardly make away without those that are in desperate need of my help. Your quest was in vain. Farewell, dear husband.")
	 conv_over = true
	 return false
      end
   end
  
   function bye()
      simpl_printcon("You cannot just leave me here! I am your wife!")
   end
   
   function otherwise(item)
      if (string.find(item, "son") and not(string.find(item, "prison")) or string.find(item, "edvard")) then
	 simpl_printcon("There was nothing more I could do for him... If prison would not have broken me already, his fate would have.")
      elseif (item == "surface" or item == "prison") then
	 simpl_printcon("I have been locked up here for too long. I do not even remember what the outside world looks like anymore... We must haste and find our way out of here!")
      else
	 simpl_printcon("Let us concern ourselves with how to get out of here instead!")
      end
   end
end
