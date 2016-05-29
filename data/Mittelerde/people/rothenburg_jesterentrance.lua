-- -----------------------------------------------
-- Conversation definition file for eureka
-- Copyright (c) Andreas Bauer <baueran@gmail.com>
-- -----------------------------------------------

do
   -- --------------------------------------------------------
   -- Define character values for either party-join or a fight
   -- --------------------------------------------------------

   c_values = { 
      name = "Jester", race = "HUMAN", ep = 0, hp = 20, hpm = 20, sp = 0, spm = 0, 
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
      simpl_printcon("A somewhat funny looking person with a tiny guitar is eyeing you.")
   end

   function name()
      simpl_printcon("They call me Skippy. He ho ho he he...")
   end
   
   function job()
      simpl_printcon("I used to be a Jester, but since the rulers in their endless wisdom have decided to reinvest the citizen's money into the citizens themselves, I am now a public servant who is here to entertain you and everyone else, and not just the royals anymore. He ho ha ha...")
   end

   function join()
      simpl_printcon("No, my duty is to entertain. Ho ho.")
      return false
   end
   
   function bye()
      simpl_printcon("The pleasure was all mine. Ho ho.")
   end
   
   function otherwise(item)
      if (string.find(item, "ruler")) then
	 simpl_printcon("Ever since the rulers came into power, the quality of living has dramatically increased for everyone around here. Democracy is a wonderful thing, ain't it?")
      elseif (item == "servant" or item == "public" or item == "public servant") then
	 simpl_printcon("It is very simple. A public servant's duty is to serve the public. In return, the public pays the public servant indirectly with their tax contribution.")
      elseif (item == "tax" or item == "contribution" or item == "tax contribution" or item == "pays" or item == "pay" or item == "money") then
	 simpl_printcon("Taxation pays for the services, the government provides us all with. And most importantly, those who are poorer, have to pay less tax than those who are better off. It is a fair system devised by wise people such as our rulers.")
      elseif (item == "duty") then
	 job()
      else	
	 simpl_printcon("I do not know about such things. He he ho ho.")
      end
   end
end
