-- -----------------------------------------------
-- Conversation definition file for eureka
-- Copyright (c) Andreas Bauer <baueran@gmail.com>
-- -----------------------------------------------

do
   -- --------------------------------------------------------
   -- Define character values for either party-join or a fight
   -- --------------------------------------------------------

   c_values = { 
      name = "Warwick", race = "HUMAN", ep = 0, hp = 20, hpm = 20, sp = 0, spm = 0, 
      str = 16, luck = 15, dxt = 12, wis = 4, charr = 10, 
      iq = 10, endd = 14, sex = "MALE", profession = "SHEPHERD", weapon = Weapons["axe"], 
      shield = Shields["small shield"] 
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
      simpl_printcon("In front of you is the chief governour.")
   end

   function name()
      simpl_printcon("I am " .. c_values[name] .. ", elected chief governour of this kingdom. What is your name fellow citizen?")
      pname = simpl_getcon()
      simpl_printcon(pname ... ", you must be very excited meeting your chief governour. What else would you like to ask me?")
   end
   
   function job()
      msg = "My duty is to serve the people who elected me chief governour of this, may I say, former kingdom. We now "
      .. "have the beginning of a democratic society, in which everyone gets to have their say. And I represent those "
      .. "people."
      simpl_printcon(msg)
   end

   function join()
      simpl_printcon("You have a sense of humour, I must give you that.")
      return false
   end
   
   function bye()
      if (string.length(pname) > 0) then
	 simpl_printcon("It was a pleasure to talk to a citizen like yourself, " .. pname .. ". I too must get back to my duties; that is, to serve the people.")
      else
	 simpl_printcon("It was a pleasure to talk to a citizen like yourself. Perhaps next time we will introduce each other first.")
      end
   end
   
   function otherwise(item)
      simpl_printcon("I can't help you with that.")
   end
end
