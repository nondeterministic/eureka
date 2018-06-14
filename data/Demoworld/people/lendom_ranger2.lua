-- -----------------------------------------------
-- Conversation definition file for eureka
-- Copyright (c) Andreas Bauer <a@pspace.org>
-- -----------------------------------------------

do
   -- --------------------------------------------------------
   -- Define character values for either party-join or a fight
   -- --------------------------------------------------------

   c_values = { 
      name = "Orje", race = "HUMAN", ep = 0, hp = 10, hpm = 10, sp = 0, spm = 0, str = 7, luck = 15, dxt = 15, wis = 4, charr = 14, 
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

   function get_armour()
      return "" -- we ignore armour
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
      simpl_printcon("You see a ranger wearing a robe that's seen better days.")
   end

   function name()
      simpl_printcon("My name is " .. c_values["name"] .. ". And who art thou?")
      answer = simpl_getcon()
      simpl_printcon("Your name sounds familiar...")
   end

   function job()
      simpl_printcon("I am and always have been a ranger around these lands.")
   end
   
   function join()
      simpl_printcon("Sorry, I cannot join thee.")
      return false
   end
  
   function bye()
      simpl_printcon("Take care. The enemy is not defeated. He merely sleeps.")
   end
   
   function otherwise(item)
      if (item == "enemy" or item == "enemies" or item == "attack") then
	 simpl_printcon("It was quick and fierce, but only because the enemy had planned this from a long hand, and dug tunnels under the town, from which he attacked us in the night.")
      elseif (item == "ranger") then
	 simpl_printcon("We roam the forests, keep an eye out for enemies, and were much needed during the attack.")
      elseif (item == "tunnel" or item == "tunnels") then
	 simpl_printcon("I have personally helped to close them, but one can never be sure if all have been found...")
      else
	 simpl_printcon("I am sorry, I cannot help you with that.")
      end
   end
end
