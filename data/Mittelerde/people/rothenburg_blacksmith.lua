-- -----------------------------------------------
-- Conversation definition file for simplicissimus
-- Copyright (c) Andreas Bauer <baueran@gmail.com>
-- -----------------------------------------------

do
   -- --------------------------------------------------------
   -- Define character values for either party-join or a fight
   -- --------------------------------------------------------

   c_values = { 
      name = "Knut", race = "HUMAN", ep = 0, hp = 10, hpm = 10, sp = 0, spm = 0, str = 7, luck = 15, dxt = 15, wis = 4, charr = 14, 
      iq = 15, endd = 7, sex = "MALE", profession = "TINKER", weapon = Weapons["axe"], shield = Shields["small shield"] 
   } 

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
   items[0] = Weapons["axe"]
   items[1] = Weapons["sword"]
   items[2] = Shields["small shield"]

   function description()
      simpl_printcon("You see a blacksmith at work.")
   end

   function name()
      simpl_printcon("I'm Knut the blacksmith.")
   end
   
   function job()
      simpl_printcon("I trade in armour and weapons. Dost thou seek to do some trade? (y/n)")
      job2()
   end
   
   function job2()
      answer = simpl_getkey("yn")
      simpl_printcon(string.format("%s ", answer))

      if (answer == "y") then
	 simpl_printcon("Dost thou wish to (b)uy or (s)ell?")
	 buySell = simpl_getkey("bs")
	 simpl_printcon(string.format("%s ", buySell))

	 simpl_ztatssave()
	 if (buySell == "b") then
	    buy()
	 else
	    simpl_printcon("Very well. Remember though, I am only interested in armour and weapons.")
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
	    simpl_printcon("It seems, you already carry too much.")
	 elseif (buyResult == -2) then
	    simpl_printcon("You don't have enough gold.")
	 elseif (buyResult == 0) then
	    simpl_printcon("I am sure it will guide you well.")
	 else
	    simpl_printcon("Hmmm... This transaction failed.")
	 end
      else
	 simpl_printcon("Changed your mind then?")
      end

      simpl_printcon("Dost thou seek to undertake further business? (y/n)")
      job2()
   end

   function join()
      simpl_printcon("Even if I wanted to, I have responsibilities, you see.")
      return false
   end

   function sell()
      -- When selling you don't get the full price. Here, -25%.
      price_reduction = 25
      simpl_ztatssellarmsfrominventory(price_reduction)
      
      simpl_printcon("Dost thou seek to undertake further business? (y/n)")
      job2()
   end
   
   function bye()
      simpl_printcon("I better get going, too. Can't stand around chatting with you all day.")
   end
   
   function otherwise(item)
      if (item == "work" or item == "trade" or item == "blacksmith" or item == "buy" or item == "sell" or item == "shop") then
	 job()
      elseif (string.find(item, "respons") or item == "responsibilities") then
	 simpl_printcon("Apart from the shop, I have to look after my boy.")
      elseif (item == "boy" or item == "son") then
	 simpl_printcon("Frederik is his name. He is only young, but already tries to help me here wherever he can. I am all that is left to him, since his mother died...")
      elseif (item == "mother" or item == string.find(item, "die")) then
	 simpl_printcon("It happened five years ago, but I still don't know the exact circumstances surrounding her death. I only know that Orcs have killed her in the woods, where they now roam more or less safely as no one disturbs them.")
      elseif (string.find(item, "orc") or string.find(item, "safe")) then
	 msg = "Orcs had almost disappeared from these lands, but for a few years now they have returned with more fury than before. "
	 .. "Worse yet, it seems that no one is doing anything about it, so they keep on killing people who enter the forests after dark or loose their way..."
	 simpl_printcon(msg)
      elseif (string.find(item, "disappear") or string.find(item, "fury") or string.find(item, "return")) then
	 msg = "In old times, men would ride out and keep these lands safe. They did this upon their own free will. But these days, everyone expects the king to do this "
	 .. "whereas he says the orcs have gotten too many. So we all stay within the city, do our work, pay our taxes and try to lead an as normal life as possible. "
	 .. "I just hope my boy will someday be able to live a life that we used to have..."
	 simpl_printcon(msg)
      else
	 simpl_printcon(string.format("%s, you say? That is curious, indeed.", item))
      end
   end
end
