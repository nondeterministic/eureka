-- -----------------------------------------------
-- Conversation definition file for eureka
-- Copyright (c) Andreas Bauer <baueran@gmail.com>
-- -----------------------------------------------

do
   -- --------------------------------------------------------
   -- Define character values for either party-join or a fight
   -- --------------------------------------------------------
   
   c_values = { 
      name = "Monkey Doodle", race = "HUMAN", ep = 0, hp = 10, hpm = 10, sp = 0, spm = 0, str = 7, luck = 15, dxt = 15, wis = 4, charr = 14, 
      iq = 15, endd = 7, sex = "FEMALE", profession = "SHEPHERD", weapon = Weapons["axe"], shield = Shields["small shield"] 
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
   
   -- Services on offer
   items = {}
   items[0] = Services["level_up"]
   
   function description()
      simpl_printcon("You are looking into a serious but somehow light and content female face.")
   end
   
   function name()
      simpl_printcon("My name is " .. c_values["name"] .. ", and what is yours?")
      tmp_name = simpl_getcon()
      simpl_printcon(tmp_name .. " sounds weird to me.")
   end
   
   function job()
   	  simpl_printcon("I can level you all up when you have sufficient experience points collected. Dost thou like me to check and level you guys up? (y/n)")
      answer = simpl_getkey("yn")
      simpl_printcon(string.format("%s ", answer))
      
      if (answer == "y") then
	    if (simpl_level_up() == 0) then
	      simpl_printcon("Seems you can't level up right now.")
	    else
	      simpl_printcon("That's that then. You are leveled up now!")
	    end
      else
	     simpl_printcon("Perhaps another time...")
      end
   end
     
   function join()
      simpl_printcon("My place is here to level shit up.")
      return false
   end
   
   function bye()
      simpl_printcon("Let me know if you need leveling up again, buddy.")
   end
   
   function otherwise(item)
   		if (item == "level" or item == "experience" or item == "help" or item == "buy" or item == "shop" or item == "sell") then
			job()
    	else	
			simpl_printcon("I cannot help you with that.")
    	end
    end
end
