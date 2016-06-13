-- -----------------------------------------------
-- Conversation definition file for eureka
-- Copyright (c) Andreas Bauer <baueran@gmail.com>
-- -----------------------------------------------

do
   -- --------------------------------------------------------
   -- Define character values for either party-join or a fight
   -- --------------------------------------------------------

   c_values = { 
      name = "John", race = "HUMAN", ep = 0, hp = 20, hpm = 20, sp = 0, spm = 0, 
      str = 16, luck = 15, dxt = 12, wis = 4, charr = 10, 
      iq = 10, endd = 14, sex = "FEMALE", profession = "SHEPHERD", weapon = Weapons["axe"], 
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
      simpl_printcon("You see a woman with a somewhat determined face.")
   end

   function name()
      simpl_printcon("My name is Lea.")
   end
   
   function job()
      simpl_printcon("I am an apprentice fighter and wish to serve in the government's army.")
   end

   function join()
      simpl_printcon("No, I wish to join the army and fight the enemies of freedom and democracy.")
      return false
   end
   
   function bye()
      simpl_printcon("It was good meeting a likeminded soul. Fare well!")
   end
   
   function otherwise(item)
      if (item == "army" or string.find(item, "govern")) then
	 simpl_printcon("The new governour is building up a new kind of army, which will consist only of the best and bravest fighters. For the first time in history, however, also females may join.")
      elseif (string.find(item, "female")) then
	 simpl_printcon("That is what I call true progress, which would not have been possible under the former king. I have made it my call therefore to fight for the freedoms we are now given with free elections and newly found rights.")
      elseif (string.find(item, "rights") or string.find(item, "election") or string.find(item, "freedom")) then
	 simpl_printcon("Yes, a lot of things have changed for the better since the king has been abandoned.")
      elseif (item == "king") then
	 msg = "I admit, the king seemed a noble man. We all know now, however, how much more progressive the new government is. I therefore wish to join its ranks."
	 simpl_printcon(msg)
      else	
	 simpl_printcon("I can't help you with that.")
      end
   end
end
