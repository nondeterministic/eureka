-- -----------------------------------------------
-- Conversation definition file for eureka
-- Copyright (c) Andreas Bauer <a@pspace.org>
-- -----------------------------------------------

do
   -- --------------------------------------------------------
   -- Define character values for either party-join or a fight
   -- --------------------------------------------------------

   c_values = { 
      name = "Rigge", race = "HUMAN", ep = 0, hp = 10, hpm = 10, sp = 0, spm = 0, str = 7, luck = 15, dxt = 15, wis = 4, charr = 14, 
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
   items[0] = Edibles["beer"]
   items[1] = Edibles["wine"]
   items[2] = Edibles["standard ration"]

   function description()
      simpl_printcon("A tall man with a bushy beard and some kind of a working robe tied around his waist.")
   end

   function name()
      simpl_printcon("I am " .. c_values["name"] .. ", and I own this inn. " ..
                        "Would you like to know what's on the menu? (y/n)")
      
      answer = simpl_getkey("yn")
      simpl_printcon(string.format("%s ", answer))
      if (answer == "y") then
         simpl_ztatssave()
         buy()
         simpl_ztatsrestore()
      else
         simpl_printcon("That's quite in order. Take your time and let me know when you have made up your mind.")
      end
   end

   function buy()
      -- Pass items for displaying and choice in ztats-window
      selected_item = simpl_ztatsshopinteraction(items)
      
      -- Try to add item to player inventory, if something was selected
      if (string.len(selected_item) > 0) then
         buyResult = simpl_buyitem(selected_item)
         if (buyResult == -1) then
            simpl_printcon("It seems, you already carry too much.")
         elseif (buyResult == -2) then
            simpl_printcon("You don't have enough gold.")
         elseif (buyResult == 0) then
            simpl_printcon("To good health! (He raises his own drinking glass towards you.)")
         end
      else
         simpl_printcon("So, you changed your mind then?")
      end
      
      simpl_printcon("Care for another one? (y/n)")
      job2()
   end

   function job()
      simpl_printcon("I run the inn here. Care for a menu? (y/n)")
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
      simpl_printcon("I have done enough fighting for a lifetime. I will stay with my inn and help rebuild Lendom.")
      return false
   end
   
   function bye()
      simpl_printcon("Farewell, and visit us soon again!")
   end
   
   function otherwise(item)
      if (item == "thirsty" or item == "hungry" or item == "bartender" or item == "business" or item == "food" or item == "buy" or item == "menu" or item == "order" or item == "sell" or item == "pub" or item == "drink" or item == "drinks") then
         job()
      elseif (string.find(item, "hotel") or string.find(item, "sleep") or string.find(item, "door") or string.find(item, "room") or string.find(item, "stay") or string.find(item, "bed") or string.find(item, "guest") or string.find(item, "rest")) then
         simpl_printcon("Yes, I used to offer lodging here as well, but no one has been staying here for ages. If you like, I can unlock the room for you, and you can stay there for nothing. " ..
                           "It is all the same to me. Are you interested? (y/n)")
         answer = simpl_getkey("yn")
         simpl_printcon(string.format("%s ", answer))
         
         if (answer == "y") then
            simpl_printcon("Alright then, you will find the door to the room opened. You can enter and leave as you please. You may safely ignore the weird noises, you hear at night. I think, it's just the wind.")
            simpl_remove_from_current_map("hotelroomdoor")
         else
            simpl_printcon("Perhaps another time then. What else do you want to know? ")
         end
      elseif (string.find(item, "noise") or string(item, "night") or string(item, "tunnel")) then
         simpl_printcon("The last guests who stayed here packed up their beds in fright the next morning. " ..
                        "They claimed that there are loud footsteps and some kind of grunting coming from the ground. I do not believe in such spooks though... There must be a natural explanation. ")
      else
         simpl_printcon("I am sorry, I cannot help you with that.")
      end
   end
end
