-- -----------------------------------------------
-- Conversation definition file for eureka
-- Copyright (c) Andreas Bauer <a@pspace.org>
-- -----------------------------------------------

do
   -- --------------------------------------------------------
   -- Define character values for either party-join or a fight
   -- --------------------------------------------------------

   c_values = { 
      name = "Menthos", race = "HUMAN", ep = 0, hp = 10, hpm = 10, sp = 0, spm = 0, str = 7, luck = 15, dxt = 15, wis = 4, charr = 14, 
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
      simpl_printcon("You see a mage with a gown that certainly would have been impressive once...")
   end

   function name()
      simpl_printcon("My name is " .. c_values["name"] .. " the mage.")
   end
   
   function job()
      simpl_printcon("My duties are to protect the castle of Velnebras.")
   end
   
   function join()
      simpl_printcon("No, I am needed here in order to rebuild Velnebras.")
      return false
   end
  
   function bye()
      simpl_printcon("Fare well, may better days be upon us soon!")
   end
   
   function otherwise(item)
      if (item == "protect" or item == "castle" or item == "velnebras" or item == "attack") then
	 simpl_printcon("Although we fought with all our might, we have failed to protect the castle Velnebras " ..
			   "from the enemy hordes. They outnumbered us and came from all sides.")
      elseif (string.find(item, "enem")) then
		 simpl_printcon("You best try and ask the great Melnior about the enemy. But ever since he failed to protect us " ..
				"from him, he has become somewhat recluse and hides for most of the day in his chambers " ..
				"in the south eastern tower of the castle. For me, these days are too terrible to recall...")
      elseif (item == "melnior") then
	 simpl_printcon("He used to be a great wizard in these lands, but ever since the attack, he is only a shadow of his former self, " ..
			   "hides behind an energy field that no one can pass unharmed other than him. He is basically a recluse, and we " ..
			   "worry about him.")
      else	
	 simpl_printcon("I cannot help you with that, perhaps ask our elder, Melnior.")
      end
   end
end
