-- -----------------------------------------------
-- Conversation definition file for eureka
-- Copyright (c) Andreas Bauer <a@pspace.org>
-- -----------------------------------------------

do
   -- --------------------------------------------------------
   -- Define character values for either party-join or a fight
   -- --------------------------------------------------------

   c_values = { 
      name = "Inge", race = "HUMAN", ep = 0, hp = 10, hpm = 10, sp = 0, spm = 0, str = 7, luck = 15, dxt = 15, wis = 4, charr = 14, 
      iq = 15, endd = 7, sex = "FEMALE", profession = "MAGE", weapon = Weapons["axe"], shield = Shields["small shield"] 
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

   pname = ""
   
   function description()
      simpl_printcon("You see a young albeit wise and somewhat proud looking woman.")
   end

   function name()
      simpl_printcon("I am " .. c_values["name"] .. ".")
   end
   
   function job()
      simpl_printcon("I look after the stables and minded the property when father was away. These were difficult times...")
   end
   
   function join()
      simpl_printcon("Impossible, the animals need me or they will die.")
      return false
   end
  
   function bye()
      simpl_printcon("Fare well and may God protect your quest!")
   end
   
   function otherwise(item)
      if (item == "father" or item == "property" or item == "farm") then
         simpl_printcon("Father's property is what feeds us and when he was away, I had to make sure it would last.")
      elseif (item == "away") then
         simpl_printcon("Father fought in Velnebras against the enemy hordes. It is best you ask him about it. I dare not think let alone speak of it.")
      elseif (string.find(item, "enemy")) then
         simpl_printcon("They were many and invaded at night. I am glad father is still alive. But ever since he returned, he is not the same. Something is on his mind...")
      elseif (item == "mind") then
         simpl_printcon("It is best, you talk to him yourself. He is in, preparing dinner, I would think.")
      else	
         simpl_printcon("I am sorry, I cannot help you with that.")
      end
   end
end
