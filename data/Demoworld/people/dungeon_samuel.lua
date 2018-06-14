-- -----------------------------------------------
-- Conversation definition file for eureka
-- Copyright (c) Andreas Bauer <a@pspace.org>
-- -----------------------------------------------

do
   -- --------------------------------------------------------
   -- Define character values for either party-join or a fight
   -- --------------------------------------------------------

   c_values = { 
      name = "Samuel", race = "HUMAN", ep = 0, hp = 10, hpm = 10, sp = 0, spm = 0, str = 7, luck = 15, dxt = 15, wis = 4, charr = 14, 
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

   pname = ""
   
   function description()
      simpl_printcon("You see a young boy who eyes you with disbelief.")
   end
 
   function name()
      simpl_printcon("On the outside, I was called " .. c_values["name"] .. " but in here names do not matter. " ..
			"Have you come to save us (y/n)?")
      answer = simpl_getkey("yn")
      simpl_printcon(string.format("%s ", answer))
      if (answer == "y") then
	 simpl_printcon("God bless!")
      else
	 simpl_printcon("Then we all shall die in here.")
      end
   end
   
   function job()
      simpl_printcon("I am too young to have a job.")
   end
   
   function join()
      simpl_printcon("The journey back outwards is too perilous for me. Please, come back and send help!")
      return false
   end
  
   function bye()
      simpl_printcon("Good bye for now, but please do not forget about us down here.")
   end
   
   function otherwise(item)
      simpl_printcon("Send help, stranger!")
   end
end
