-- -----------------------------------------------
-- Conversation definition file for eureka
-- Copyright (c) Andreas Bauer <baueran@gmail.com>
-- -----------------------------------------------

do
   -- --------------------------------------------------------
   -- Define character values for either party-join or a fight
   -- --------------------------------------------------------

   c_values = { 
      name = "Mithar", race = "HUMAN", ep = 0, hp = 10, hpm = 10, sp = 0, spm = 0, str = 7, luck = 15, dxt = 15, wis = 4, charr = 14, 
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
      simpl_printcon("You see a tall man wearing a robe with various golden symbols on it. An air of power surrounds this person.")
   end

   function name()
      simpl_printcon("My name is " .. c_values["name"] .. ". And who art thou?")
      answer = simpl_getcon()
      simpl_printcon(answer .. " ")
      simpl_printcon("I came across your name before...")
   end

   function job()
      simpl_printcon("I am both mayor of this town and leader of the guild of wizards.")
   end
   
   function join()
      simpl_printcon("As a mayor, my duties are with Lendom. I am sure, you understand.")
      return false
   end
  
   function bye()
      simpl_printcon("Farewell!")
   end
   
   function otherwise(item)
      if (item == "mayor" or item == "lendom" or item == "town") then
	 simpl_printcon("Now that we are rebuilding, it is my task to reinstate order and structures of public life.")
      elseif (string.find(item, "wizard") or string.find(item, "guild")) then
	 simpl_printcon("The guild used to have many members, most of which Alas! have died in the recent attacks.")
      elseif (string.find(item, "attack") or string.find(item, "died")) then
	 simpl_printcon("It was dark times, but now we are building for a brighter future. Take a look around, and see for yourself. Some of our stores have even reopened.")
      else
	 simpl_printcon("I am sorry, I cannot help you with that.")
      end
   end
end
