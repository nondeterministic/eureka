-- -----------------------------------------------
-- Conversation definition file for simplicissimus
-- Copyright (c) Andreas Bauer <baueran@gmail.com>
-- -----------------------------------------------

do
	-- --------------------------------------------------------
	-- Define character values for either party-join or a fight
	-- --------------------------------------------------------

	c_values = { 
		name = "Lilia", race = "HUMAN", ep = 0, hp = 10, hpm = 10, sp = 0, spm = 0, str = 7, luck = 15, dxt = 15, wis = 4, charr = 14, 
		iq = 15, endd = 7, sex = "FEMALE", profession = "SHEPHERD", weapon = Weapons["axe"], shield = Shields["small shield"] 
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
	
	-- -----------------------------------------------
	-- Standard terms
	-- -----------------------------------------------

    -- Items for sale
	items = {}
	items[0] = Edibles["iron ration"]
	items[1] = Edibles["standard ration"]
	items[2] = Edibles["magic mushroom"]

	function description()
		simpl_printcon("You see a food merchant.")
	end

	function name()
		simpl_printcon("I'm Lilia the food merchant in town. I'm vegan, lesbian and gluten-intolerant.")
	end
	
	function job()
		simpl_printcon("I sell fair trade vegen food. Dost thou seek to buy some (y/n)?")
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
		    simpl_printcon("Changed your mind then, eh?")
		end

 	    simpl_printcon("Dost thou seek to undertake further business (y/n)?")
	    job2()
	end

	function join()
	        simpl_printcon("In your dreams matey.")
		return false
	end

	function sell()
		-- When selling you don't get the full price. Here, -25%.
        price_reduction = 25
	    simpl_ztatssellarmsfrominventory(price_reduction)
		
	    simpl_printcon("Dost thou seek to undertake further business (y/n)?")
	    job2()
	end
	
	function bye()
		simpl_printcon("I better get going, too. Can't stand around chatting with you all day.")
	end
	
	function otherwise(item)
		if (item == "vegan" or item == "food" or item == "buy") then
			job()
		else	
			simpl_printcon(string.format("%s? I don't know about such things. I'm just a simple mind after all...", item))
		end
	end
end
