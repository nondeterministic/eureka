-- -----------------------------------------------
-- Conversation definition file for eureka
-- Copyright (c) Andreas Bauer <a@pspace.org>
-- -----------------------------------------------

do
   -- --------------------------------------------------------
   -- Define character values for either party-join or a fight
   -- --------------------------------------------------------

   c_values = { 
      name = "Grutle", race = "HUMAN", ep = 0, hp = 10, hpm = 10, sp = 0, spm = 0, str = 7, luck = 15, dxt = 15, wis = 4, charr = 14, 
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

   -- Convenience function to sort items alphabetically
   function sort_items(item1, item2)
      return item1.name < item2.name
   end
   
   -- -----------------------------------------------
   -- Standard terms
   -- -----------------------------------------------

   items = {}
   items[0] = Shields["small shield"]
   items[1] = Weapons["axe"]
   items[2] = Weapons["sword"]
   items[3] = Weapons["bow"]
   items[4] = Weapons["dagger"]
   items[5] = Weapons["crossbow"]
   items[6] = Weapons["mace"]
   items[7] = Weapons["helbeard"]
   items[8] = Armour["leather helmet"]
   items[9] = Armour["leather armour"]
   items[10] = Armour["metal helmet"]
   items[11] = Armour["platemail"]
   items[12] = MiscItems["arrow"]
   items[13] = MiscItems["bolt"]
   table.sort(items, sort_items)
   
   function description()
      simpl_printcon("You see big, sooty blacksmith with sweat running down his forehead.")
   end

   function name()
      simpl_printcon("I am " .. c_values["name"] .. ". Are you here to buy or sell some arms? (y/n)")
      askforbusiness()
   end

   function askforbusiness()
      answer = simpl_getkey("yn")
      simpl_printcon(string.format("%s ", answer))
      if (answer == "y") then
         simpl_printcon("Would you like to buy or sell? (b/s)")
         buyorsell = simpl_getkey("bs")
         
         simpl_ztatssave()
         if (buyorsell == "b") then
            buy()
         else
            sell()
         end
         
         simpl_ztatsrestore()
      else
         simpl_printcon("Perhaps another time then. What else do you want to know?")
      end
   end
   
   function buy()
      -- Pass items for displaying and choice in ztats-window
      selected_item = simpl_ztatsshopinteraction(items)

      -- Try to add item to player inventory, if something was selected
      if (string.len(selected_item) > 0) then
         buyResult = simpl_buyitem(selected_item)
         
         if (buyResult == -1) then
            simpl_printcon("You can't carry any more.")
         elseif (buyResult == -2) then
            simpl_printcon("You don't have enough gold.")
         end
      else
         simpl_printcon("So, you changed your mind then?")
      end
      
      simpl_printcon("Dost thou seek to buy more of my time? (y/n)")
      askforbusiness()
   end
   
   function sell()
      price_reduction = 25
      simpl_ztatssellarmsfrominventory(price_reduction)
      
      simpl_printcon("Dost thou seek to undertake further business? (y/n)")
      askforbusiness()
   end
   
   function job()
      simpl_printcon("I am the blacksmith in town. Are you interested in doing some business? (y/n)")
      askforbusiness()
   end

   function join()
      simpl_printcon("I cannot join thee, who would run this shop then?")
      return false
   end
  
   function bye()
      simpl_printcon("Good bye, and visit us soon again!")
   end
   
   function otherwise(item)
      if (item == "buy" or item == "sell") then
	 job()
      else	
	 simpl_printcon("I am sorry, I cannot help you with that.")
      end
   end
end
