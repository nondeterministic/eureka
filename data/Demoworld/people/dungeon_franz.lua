-- -----------------------------------------------
-- Conversation definition file for eureka
-- Copyright (c) Andreas Bauer <a@pspace.org>
-- -----------------------------------------------

do
   -- --------------------------------------------------------
   -- Define character values for either party-join or a fight
   -- --------------------------------------------------------

   c_values = { 
      name = "Franz", race = "HUMAN", ep = 0, hp = 10, hpm = 10, sp = 0, spm = 0, str = 7, luck = 15, dxt = 15, wis = 4, charr = 14, 
      iq = 15, endd = 7, sex = "MALE", profession = "SHEPHERD"
   } 

   conv_over = false
   
   -- -----------------------------------------------
   -- Standard functions
   -- -----------------------------------------------

   function get_weapon()
      return ""
   end

   function get_shield()
      return ""
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
      simpl_printcon("You see a haggard, broken man.")
   end
 
   function name()
      simpl_printcon("My name is " .. c_values["name"] .. " but I have not heard anyone call me that for a long time now...")
   end
   
   function job()
      simpl_printcon("I used to be a shepherd.")
   end
   
   function join()
      simpl_printcon("I have been locked up for too long. I would be a burden to you. Take your wife to the surface and get us help instead!")
      return false
   end
  
   function bye()
      simpl_printcon("Good bye for now, but please do not forget about us down here.")
   end
   
   function otherwise(item)
      simpl_printcon("Sorry, I cannot help you with that.")
   end
end
