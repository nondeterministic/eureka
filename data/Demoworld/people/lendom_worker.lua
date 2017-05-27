-- -----------------------------------------------
-- Conversation definition file for eureka
-- Copyright (c) Andreas Bauer <baueran@gmail.com>
-- -----------------------------------------------

do
   -- --------------------------------------------------------
   -- Define character values for either party-join or a fight
   -- --------------------------------------------------------

   c_values = { 
      name = "Ulthar", race = "HUMAN", ep = 0, hp = 10, hpm = 10, sp = 0, spm = 0, str = 7, luck = 15, dxt = 15, wis = 4, charr = 14, 
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
      simpl_printcon("You see a man carrying some tools.")
   end

   function name()
      simpl_printcon("I am " .. c_values["name"] .. ". And who am I talking to?")
      answer = simpl_getcon()
      simpl_printcon("Your name sounds familiar...")
   end

   function job()
      simpl_printcon("I used to be a blacksmith, but my store was destroyed. So now I help to rebuild Lendom.")
   end
   
   function join()
      simpl_printcon("Sorry, I cannot join thee.")
      return false
   end
  
   function bye()
      simpl_printcon("Good luck in your quest.")
   end
   
   function otherwise(item)
      if (item == "store" or item == "destroyed" or item == "lendom") then
	 simpl_printcon("It was a fierce battle, but by sticking together, we have defeated the enemy. Many were lost, but we have to rebuild Lendom anyway.")
      elseif (item == "lost" or item == "battle") then
	 simpl_printcon("I rather not recall events. Some say that some people survived and have been brought by the enemy into a dark dungeon, where they now live to serve them. People have tried to find them, but never returned back home. So who knows?")
      elseif (item == "dungeon" or item == "survived") then
	 simpl_printcon("I cannot tell you where this dungeon might be. But I know for a fact it must be outside town, because they have dug long tunnels from afar into Lendom. Where these tunnels start, we do not know...")
      else
	 simpl_printcon("I am sorry, I cannot help you with that.")
      end
   end
end
