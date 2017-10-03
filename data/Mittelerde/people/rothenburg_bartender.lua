-- -----------------------------------------------
-- Conversation definition file for eureka
-- Copyright (c) Andreas Bauer <baueran@gmail.com>
-- -----------------------------------------------

do
   -- --------------------------------------------------------
   -- Define character values for either party-join or a fight
   -- --------------------------------------------------------
   
   c_values = { 
      name = "Bartender", race = "HUMAN", ep = 0, hp = 10, hpm = 10, sp = 0, spm = 0, str = 7, luck = 15, dxt = 15, wis = 4, charr = 14, 
      iq = 15, endd = 7, sex = "FEMALE", profession = "SHEPHERD", weapon = Weapons["axe"], shield = Shields["small shield"] 
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
   
   -- Items for sale
   items = {}
   items[0] = Edibles["beer"]
   items[1] = Edibles["wine"]
   items[2] = Edibles["standard ration"]
   
   function description()
      simpl_printcon("You see a friendly looking bartender.")
   end
   
   function name()
      simpl_printcon("I'm Ludwig, owner of this fine pub.")
   end
   
   function job()
      simpl_printcon("Food and drinks are sold here. Dost thou seek to buy some? (y/n)")
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
	 simpl_printcon("Perhaps another time then. Feel free to look around and have a friendly chat with folks around here. And if you get hungry or thirsty, let me know.")
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
	    if (string.find(selected_item, "wine")) then
	       simpl_printcon("I can sell you a glass of the house wine, but the good ones are in the cellar. Would you care to have a look yourself? (y/n)")
	       look_answer = simpl_getkey("yn")
	       simpl_printcon(string.format("%s ", look_answer))
	       if (look_answer == "y") then
		  simpl_printcon("Alright then, I shall open the gate for you. Feel free to take a look downstairs...")
		  conv_over = true
		  -- Open the gate and play a creeky sound!
		  simpl_play_sound("55820__sergenious__bridge.wav", 1)
		  simpl_change_icon(39, 36, 71, 72)
	       end
	    end
	    simpl_printcon("To good health! (He raises his own drinking glass towards you.)")
	 else
	    simpl_printcon("Hmmm... This transaction failed.")
	 end
      else
	 simpl_printcon("Changed your mind then, eh?")
      end
      
      simpl_printcon("Anything else, matey? (y/n)")
      job2()
   end
   
   function join()
      simpl_printcon("In your dreams matey.")
      return false
   end
   
   function bye()
      simpl_printcon("Hope to welcome you again soon. Have a good day!")
   end
   
   function otherwise(item)
      if (item == "thirsty" or item == "hungry" or item == "bartender" or item == "business" or item == "food" or item == "buy" or item == "menu" or item == "order" or item == "sell" or item == "pub" or item == "drink" or item == "drinks") then
	 job()
      else	
	 simpl_printcon("I have guests to attend to. Let me know, if you want to order something from the menu.")
      end
   end
end
