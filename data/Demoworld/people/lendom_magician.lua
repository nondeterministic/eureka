-- -----------------------------------------------
-- Conversation definition file for eureka
-- Copyright (c) Andreas Bauer <a@pspace.org>
-- -----------------------------------------------

do
   -- --------------------------------------------------------
   -- Define character values for either party-join or a fight
   -- --------------------------------------------------------

   c_values = { 
      name = "Skara", race = "HUMAN", ep = 0, hp = 10, hpm = 10, sp = 0, spm = 0, str = 7, luck = 15, dxt = 15, wis = 4, charr = 14, 
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
      simpl_printcon("You see a middle aged magic user, wearing a blue robe.")
   end

   function name()
      simpl_printcon(c_values["name"] .. " is my name. How may I help you, stranger?")
   end

   function job()
      simpl_printcon("I am a user of magic, you could say.")
   end
   
   function join()
      simpl_printcon("I cannot join thee. This city is in need of my help.")
      return false
   end
  
   function bye()
      simpl_printcon("Farewell, stranger!")
   end
   
   function otherwise(item)
      if (item == "magic") then
         simpl_printcon("Magic was one of the prime defences, we had against the enemy.")
      elseif (item == "enemy" or string.find(item, "defenc")) then
         simpl_printcon("The enemies were many, but they did not expect such strong magical abilities of a seemingly insignificant town as ours. I have cast many a spells to destroy the enemy.")
      elseif (string.find(item, "spell")) then
         simpl_printcon("I have cast both attack and healing spells as far as my abilities allowed. In the end, the enemy has retreated, but some people say, he merely sleeps...")
      elseif (string.find(item, "sleep")) then
         simpl_printcon("Some people hear noises under the soil at night, and believe the enemy is still at large.")
      elseif (string.find(item, "noise") or string.find(item, "noise")) then
         simpl_printcon("You're best to ask around town for this phenomenon. I have never heard it, but people who slept in the room in the pub swear that they have...  I am not sure, what to make of it.")
      else
         simpl_printcon("I am sorry, I cannot help you with that.")
      end
   end
end
