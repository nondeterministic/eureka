-- -----------------------------------------------
-- Conversation definition file for eureka
-- Copyright (c) Andreas Bauer <baueran@gmail.com>
-- -----------------------------------------------

do
	-- --------------------------------------------------------
	-- Define character values for either party-join or a fight
	-- --------------------------------------------------------

	c_values = { 
		name = "Fips", race = "HUMAN", ep = 0, hp = 10, hpm = 10, sp = 0, spm = 0, str = 7, luck = 15, dxt = 15, wis = 4, charr = 14, 
		iq = 15, endd = 7, sex = "MALE", profession = "SHEPHERD", weapon = Weapons["axe"], shield = Shields["small shield"] 
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

	function description()
		simpl_printcon("You see a young boy with a friendly smile.")
	end

	function name()
		simpl_printcon("I'm Fips.")
	end
	
	function job()
		simpl_printcon("I'm only a child, I don't have a job nor money. Dost though give me some?")
		answer = simpl_getcon()
		simpl_printcon(string.format("%s ", answer))

		if (answer == "y" or answer == "yes") then
		   simpl_printcon("That's great! How many coins will you give me?")
		   coins = simpl_getcon()
		   simpl_printcon(string.format("%s ", coins))

		   if (tonumber(coins) <= 0) then
		     simpl_printcon("Nearly fooled me, dork!")
		   else
		     if (simpl_modify_gold(-(tonumber(coins)))) then
			    simpl_printcon("Thank you! May God be with you, stranger.")
			 else
		     	simpl_printcon("You don't have enough gold. Perhaps another time?")
		     end
		   end
		else
		   simpl_printcon("A shame. I suppose you don't have much yourself...")		                   
		end
	end

	function join()
		if (simpl_partysize() < 6) then
			simpl_printcon("I'm honoured and will join thee.")
			join_successful = simpl_join(c_values)
			simpl_remove_from_current_map("boy")
		    return true
        else
		 	simpl_printcon("I'm honoured and will join thee, but you seem to have no space in your party.")
		 	return false
		end
	end
	
	function bye()
		simpl_printcon("I better get going, too. Can't stand around chatting with you all day.")
	end
	
	function otherwise(item)
		if (item == "wendy") then
			simpl_printcon("She's from Canberra, isn't she?")
		else	
			simpl_printcon(string.format("%s? I don't know about such things. I'm just a boy after all...", item))
		end
	end
end
