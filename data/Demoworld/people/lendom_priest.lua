-- -----------------------------------------------
-- Conversation definition file for eureka
-- Copyright (c) Andreas Bauer <a@pspace.org>
-- -----------------------------------------------

do
   -- --------------------------------------------------------
   -- Define character values for either party-join or a fight
   -- --------------------------------------------------------

   c_values = { 
      name = "Martin", race = "HUMAN", ep = 0, hp = 10, hpm = 10, sp = 0, spm = 0, str = 7, luck = 15, dxt = 15, wis = 4, charr = 14, 
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
      simpl_printcon("You see monk with a partly shaven head and wearing a brown robe.")
   end
   
   function name()
      simpl_printcon("My name is " .. c_values["name"] .. ". And you must be " .. simpl_get_player_name(0) .. ". Am I right? (y/n)")
      answer = simpl_getkey("yn")
      if (answer == "y") then
         simpl_printcon("I knew you would come here sooner or later.")
      else
         simpl_printcon("I see that you do not trust me, yet. But you will have to, if you really want to save your wife...")
      end
   end
   
   function job()
      simpl_printcon("I am the priest of this town. But I also meditate with people and help them gain higher consciousness and levels of experience. Dost thou want me to try it with you? (y/n)")
      answer = simpl_getkey("yn")
      if (answer == "y") then
         level_up()
      else
         simpl_printcon("Make no mistake: you will need to see me sooner or later, or you will not be able to help rescue your wife.")
      end      
   end
   
   function level_up()
      simpl_printcon("Let me concentrate and put my hands onto you...")
      levelled_up = simpl_level_up()
      if (levelled_up == 0) then
         simpl_printcon("You are not experienced enough yet. Come back, once you have gained more experience, and I shall help you...")
      end
   end
   
   function join()
      simpl_printcon("Impossible. The town needs me as much as you will. Come back any time to visit me though.")
      return false
   end
  
   function bye()
      simpl_printcon("Bye for now. Come back, when you have gained more experience and we shall medidate together.")
   end
   
   function otherwise(item)
      if (item == "wife") then
         simpl_printcon("I have seen Evelyn being dragged away by the retreating hordes as I have returned to Lendom. I am almost certain, she has been brought to a dungeon in the mountains somewhere south east of here...")
      elseif (item == "dungeon" or item == "mountains") then
         simpl_printcon("No one has ever seen the enemy's hideout, but it must be there. It is unwieldy territory. So be careful, if you go look for her. And do not forget to return to me for meditation! It will make you stronger and prepare you for your quest.")
      elseif (string.find(item, "medita")) then
         level_up()
      else
         simpl_printcon("I am sorry, I cannot help you with that.")
      end
   end
end
