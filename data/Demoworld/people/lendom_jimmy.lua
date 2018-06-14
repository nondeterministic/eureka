-- -----------------------------------------------
-- Conversation definition file for eureka
-- Copyright (c) Andreas Bauer <a@pspace.org>
-- -----------------------------------------------

do
   -- --------------------------------------------------------
   -- Define character values for either party-join or a fight
   -- --------------------------------------------------------

   c_values = { 
      name = "Jimmy", race = "HUMAN", ep = 0, hp = 10, hpm = 10, sp = 0, spm = 0, str = 7, luck = 15, dxt = 15, wis = 4, charr = 14, 
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
      simpl_printcon("You see a young boy with a dirty face and clothes.")
   end

   function name()
      simpl_printcon(c_values["name"] .. ". What is yours?")
      answer = simpl_getcon()
      simpl_printcon(answer .. " ")
      simpl_printcon("That's a funny name.")
   end

   function job()
      simpl_printcon("I don't have a job. In fact, I have lost my parents in the recent attacks.")
   end
   
   function join()
      simpl_printcon("No, thanks.")
      return false
   end
  
   function bye()
      simpl_printcon("Good-bye.")
   end
   
   function otherwise(item)
      if (item == "parents" or item == "attacks") then
         simpl_printcon("When the dust has settled, they simply never came back. Have you lost someone in the attacks, too? (y/n)")
         answer = simpl_getkey("yn")
         if (answer == "y") then
            simpl_printcon("Then you know, how it feels, I guess.")
         else
            simpl_printcon("Lucky, you.")
         end
      else
         simpl_printcon("I am sorry, I cannot help you with that.")
      end
   end
end
