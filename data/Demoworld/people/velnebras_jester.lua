-- -----------------------------------------------
-- Conversation definition file for eureka
-- Copyright (c) Andreas Bauer <a@pspace.org>
-- -----------------------------------------------

do
   -- --------------------------------------------------------
   -- Define character values for either party-join or a fight
   -- --------------------------------------------------------

   c_values = { 
      name = "Jiggles", race = "HUMAN", ep = 0, hp = 10, hpm = 10, sp = 0, spm = 0, str = 7, luck = 15, dxt = 15, wis = 4, charr = 14, 
      iq = 15, endd = 7, sex = "MALE", profession = "BARD", weapon = Weapons["axe"], shield = Shields["small shield"] 
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
      simpl_printcon("You see a jester with a tear, trying to put on a happy face.")
   end

   function name()
      simpl_printcon("They call me " .. c_values["name"] .. ", ho ho ho.")
   end
   
   function job()
      simpl_printcon("I used to be the jester in town, but ever since the invasion on that fateful day, it seems that people no longer are so interested in my tricks and jokes...")
   end
   
   function join()
      simpl_printcon("No, thanks. I am tired, I just want to rest.")
      return false
   end
  
   function bye()
      simpl_printcon("Talk to you again, soon, ho ho ho!")
   end
   
   function otherwise(item)
      if (item == "invasion" or item == "day" or item == "fateful") then
	 simpl_printcon("It was terrible. Although we all knew it was coming, we did not want to face the truth until it was too late. We did not take serious the voices of the sceptics and warners.")
      elseif (item == "sceptics" or item == "warners" or item == "voices") then
	 simpl_printcon("Yes, there were some people who warned us of the intentions of the enemy, but we did not want to hear or believe them. We just wanted to keep living our comfortable lives, and look where this has lead us to!")
      elseif (item == "enemy") then
	 simpl_printcon("The enemy grew stronger by the day, and kept reassuring us that we had nothing to fear from him. A fatal mistake, as it turned out.")
      else	
	 simpl_printcon("This does not ring a bell to me, ho ho ho.")
      end
   end
end
