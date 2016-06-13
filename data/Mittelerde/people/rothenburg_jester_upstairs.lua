-- -----------------------------------------------
-- Conversation definition file for eureka
-- Copyright (c) Andreas Bauer <baueran@gmail.com>
-- -----------------------------------------------

do
   -- --------------------------------------------------------
   -- Define character values for either party-join or a fight
   -- --------------------------------------------------------

   c_values = { 
      name = "Ralf", race = "HUMAN", ep = 0, hp = 20, hpm = 20, sp = 0, spm = 0, 
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

   function description()
      simpl_printcon("You see a jester with a somewhat forced, uneasy smile.")
   end

   function name()
      simpl_printcon("Hey, ho, my name is " .. c_values[name] .. ".")
   end
   
   function job()
      simpl_printcon("I am the commander in chief of the army. No, seriously, what do you think it is I do?")
   end

   function join()
      simpl_printcon("I will stay right here, where I belong.")
      return false
   end
   
   function bye()
      simpl_printcon("Fare well and stay true to yourself. Ho, ho, ho!")
   end
   
   function otherwise(item)
      if (item == "chief" or item == "commander" or item == "army" or item == "jester") then
	 simpl_printcon("Sometimes I regret my career choices and wish I would really become something more important. But instead, I sit here and have to entertain everyone, irrespective of whether I am in the mood for it or not.")
      elseif (item == "career" or item == "entertain" or item == "mood") then
	 simpl_printcon("You see, I do this, because it pays, but I have a nagging feeling that the things I tell people and the things I see aren't right, and it eats me up from the inside.")
      elseif (item == "feeling" or item == "things" or item == "right") then
	 simpl_printcon("[He looks over his shoulder to see whether someone can hear him.] Even though we have democracy now and freedoms for everyone, it seems the jails are fuller than ever, and full with people I once knew and respected, not criminals.")
      elseif (string.find(item, "jail") or string.find(item, "democracy") or string.finde(item, "criminal")) then
	 simpl_printcon("Draw your own conclusions from that, or end up like me...")
      else	
	 simpl_printcon("I cannot help you with that.")
      end
   end
end
