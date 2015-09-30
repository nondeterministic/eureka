-- -----------------------------------------------
-- Conversation definition file for simplicissimus
-- Copyright (c) Andreas Bauer <baueran@gmail.com>
-- -----------------------------------------------

do
   -- --------------------------------------------------------
   -- Define character values for either party-join or a fight
   -- --------------------------------------------------------

   c_values = { 
      name = "Tax officer", race = "HUMAN", ep = 0, hp = 20, hpm = 20, sp = 0, spm = 0, 
      str = 16, luck = 15, dxt = 12, wis = 4, charr = 10, 
      iq = 10, endd = 14, sex = "MALE", profession = "THIEF", weapon = Weapons["axe"], 
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

   function description()
      simpl_printcon("You see a formidably dressed middle aged man, holding a yellow staff.")
   end

   function name()
      simpl_printcon("My name should not concern you, but tell, who is it that I speak to?")
      answer = simpl_getcon()
      simpl_printcon(string.format("%s ", answer))
      simpl_printcon(string.format("%s you say? That is peculiar. I seem to be unable to find your name in my books...", answer))
   end
   
   function job()
      simpl_printcon("I am Rothenburg's chief tax officer. Whoever pays their taxes, will be written down in my books.")
   end

   function join()
      simpl_printcon("No, my duty is here to serve the people of this kingdom, including yourself.")
      return false
   end
   
   function bye()
      simpl_printcon("Always at your service.")
   end
   
   function otherwise(item)
      if (item == "tax" or item == "taxes") then
	 simpl_printcon("Without taxes there would be no protection of our citizens, no schools, no regular markets or any of the other luxuries the people of our kingdom get to enjoy everyday.")
	 simpl_printcon("Dost though not agree, that one cannot run a kingdom without taxes? (y/n)")
	 answer = simpl_getkey("yn")
	 simpl_printcon(string.format("%s ", answer))
	 if (answer == "y") then
	    simpl_printcon("Indeed, it seems obvious to everyone with just half a brain.")
	 else
	    simpl_printcon("Oh, that seems a rather selfish attitude to me. By the way, passing through our city gates will cost you 2 gold coins in tax. ")
	    simpl_printcon("Would you be so kind to pay that now? (y/n)")
	    pay_answer = simpl_getkey("yn")
	    simpl_printcon(string.format("%s ", pay_answer))
	    if (pay_answer == "y") then
	       if (simpl_modify_gold(-2)) then
		  simpl_printcon("Thank you, your gold will help everyone in the kingdom.")
	       else
		  simpl_printcon("You should not try and fool the king into believing you are poor. GUARDS!! GUARDS!!")
		  conv_over = true
		  -- TODO: End conversation, make guards hostile
	       end
	    else
	       simpl_printcon("Your continued antisocial behaviour leaves me no choice but to call for the guards. GUARDS!!")
	       conv_over = true
	       -- TODO: End conversation, make guards hostile
	    end
	 end
      elseif (item == "servant" or item == "public" or item == "public servant" or item == "skills") then
	 job()
      elseif (item == "book" or item == "books") then
	 simpl_printcon("Excellent book keeping skills are essential for a public servant.")
      else	
	 simpl_printcon("I do not know about that. Also duty calls, could you please hurry up a little?")
      end
   end
end
