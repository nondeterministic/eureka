-- -----------------------------------------------
-- Conversation definition file for simplicissimus
-- Copyright (c) Andreas Bauer <baueran@gmail.com>
-- -----------------------------------------------

do
   -- --------------------------------------------------------
   -- Define character values for either party-join or a fight
   -- --------------------------------------------------------

   c_values = { 
      name = "Off-duty guard", race = "HUMAN", ep = 0, hp = 20, hpm = 20, sp = 0, spm = 0, 
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
      simpl_printcon("You're faced with a stocky-built city guard having a meal.")
   end

   function name()
      simpl_printcon("My name is Gunter. Mpfgrmph...")
   end
   
   function job()
      simpl_printcon("Mpfgrmph... I'm a guard here in town, but now I am off duty and having a meal.")
   end

   function join()
      simpl_printcon("My duty is to protect and to serve here in town.")
      return false
   end
   
   function bye()
      simpl_printcon("Mpfgrmph... Same to you.")
   end
   
   function otherwise(item)
      if (item == "guard" or item == "duty" or item == "off duty") then
	 simpl_printcon("Guard duty involves first and foremost the protection of this city.")
      elseif (item == "protection" or item == "city") then
	 simpl_printcon("The king in his endless wisdom has increased the number of guards, making Rothenburg one of the safest cities within the kingdom. Alas! our jails are still full of offenders.")
      elseif (item == "jail" or item == "jails" or item == "offenders") then
	 simpl_printcon("Can you believe, there are people living amongst us who want to benefit from the king's generosity, but when it is their turn to give back to society, they try and avoid paying their taxes, hide their gold or flee the city. But don't worry, the arm of justice reaches far throughout the kingdom. Antisocial behaviour like this will not stand.")
      elseif (item == "people" or item == "antisocial") then
	 simpl_printcon("Between you and me, rumour has it that there is a whole dwelling of this antisocial pack somewhere within the realms of our kingdom. I haven't met or seen anyone with me own eyes though.")
      else	
	 simpl_printcon("Mpfgrmph... Sorry, can't help ya with that.")
      end
   end
end
