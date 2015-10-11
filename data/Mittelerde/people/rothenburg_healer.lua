-- -----------------------------------------------
-- Conversation definition file for simplicissimus
-- Copyright (c) Andreas Bauer <baueran@gmail.com>
-- -----------------------------------------------

do
   -- --------------------------------------------------------
   -- Define character values for either party-join or a fight
   -- --------------------------------------------------------
   
   c_values = { 
      name = "Healer", race = "HUMAN", ep = 0, hp = 10, hpm = 10, sp = 0, spm = 0, str = 7, luck = 15, dxt = 15, wis = 4, charr = 14, 
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
   
   -- Services on offer
   items = {}
   items[0] = Services["heal"]
   items[1] = Services["heal light wounds"]
   items[2] = Services["resurrection"]
   
   function description()
      simpl_printcon("You are looking into a serious but somehow light and content female face.")
   end
   
   function name()
      simpl_printcon("My name is Pia. How can I help you?")
   end
   
   function job()
      simpl_printcon("Since my early childhood I have had this gift to help people. So I decided to become a healer. Tell me, are you in need of healing? (y/n)")
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
      selected_item   = simpl_ztatsshopinteraction(items)

      -- Try to add item to player inventory, if something was selected
      if (string.len(selected_item) > 0) then
	 simpl_printcon("Which party member is in need of healing?")
	 selected_player = simpl_choose_player()

	 simpl_printcon(simpl_get_player_name(selected_player))

	 buyResult = simpl_buyservice(selected_player, selected_item)
	 if (buyResult == -1) then
	    simpl_printcon("You don't have enough gold.")
	 elseif (buyResult == 0) then -- success
	    -- Do nothing.
	 else
	    simpl_printcon("Hmmm... This transaction failed. buyResult = " .. buyResult)
	 end
      else
	 simpl_printcon("Perhaps another time...")
      end
      
      simpl_printcon("Can I be of assistance with anything else? (y/n)")
      job2()
   end
   
   function join()
      simpl_printcon("My place is here to heal people...")
      return false
   end
   
   function bye()
      simpl_printcon("Best of luck in your quest!")
   end
   
   function otherwise(item)
      if (item == "heal" or item == "healing" or item == "health" or item == "help" or item == "buy") then
	 job()
      else	
	 simpl_printcon("I cannot help you with that.")
      end
   end
end
