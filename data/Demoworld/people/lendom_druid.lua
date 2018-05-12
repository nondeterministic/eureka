-- -----------------------------------------------
-- Conversation definition file for eureka
-- Copyright (c) Andreas Bauer <a@pspace.org>
-- -----------------------------------------------

do
   -- --------------------------------------------------------
   -- Define character values for either party-join or a fight
   -- --------------------------------------------------------

   c_values = { 
      name = "Olav", race = "HUMAN", ep = 0, hp = 10, hpm = 10, sp = 0, spm = 0, str = 7, luck = 15, dxt = 15, wis = 4, charr = 14, 
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

   items = {}
   items[0] = Services["heal light wounds"]
   items[1] = Services["heal"]
   items[2] = Services["heal poisonous state"]

   function description()
      simpl_printcon("You see serious but friendly looking priest.")
   end

   function name()
      simpl_printcon("My name is " .. c_values["name"] .. ", I am a healer. " ..
                        "Are you in need of healing? (y/n)")
      
      answer = simpl_getkey("yn")
      simpl_printcon(string.format("%s ", answer))
      if (answer == "y") then
         simpl_ztatssave()
         buy()
         simpl_ztatsrestore()
      else
         simpl_printcon("Perhaps another time then. What else do you want to know?")
      end
   end
   
   function buy()
      -- First choose player
      selected_player = simpl_choose_player()
      if (selected_player < 0) then
         simpl_printcon("So, you changed your mind then?")
         return
      end
      simpl_printcon(simpl_get_player_name(selected_player))
      
      -- Pass items for displaying and choice in ztats-window
      selected_item = simpl_ztatsshopinteraction(items)
      
      -- Try to add item to player inventory, if something was selected
      if (string.len(selected_item) > 0) then
         buyResult = simpl_buyservice(selected_player, selected_item)
         if (buyResult == -1) then
            simpl_printcon("You don't have enough gold.")
         end
      else
         simpl_printcon("So, you changed your mind then?")
      end
      
      simpl_printcon("Dost thou seek to buy more of my time? (y/n)")
      job2()
   end

   function job()
      simpl_printcon("In order to live, I sell my gift of healing to the people in need in this town. Are you in need of healing? (y/n)?")
      job2()
   end
   
   function job2()
      answer = simpl_getkey("yn")
      simpl_printcon(string.format("%s ", answer))
      
      if (answer == "y") then
         simpl_ztatssave()
         buy()
         simpl_ztatsrestore()
      else
         simpl_printcon("Perhaps another time then. What else do you want to know?")
      end
   end
   
   function join()
      simpl_printcon("I cannot join thee, my responsibilities are with this town.")
      return false
   end
   
   function bye()
      simpl_printcon("Good bye, traveller!")
   end
   
   function otherwise(item)
      if (item == "buy") then
         job()
      else	
         simpl_printcon("I am sorry, I cannot help you with that.")
      end
   end
end
