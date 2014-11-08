-- -----------------------------------------------
-- Conversation definition file for simplicissimus
-- Copyright (c) Andreas Bauer <baueran@gmail.com>
-- -----------------------------------------------

do
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
	
	function bye()
		simpl_printcon("I better get going, too. Can't stand around chatting with you all day.")
	end
	
	-- -----------------------------------------------
	
	function otherwise(item)
		if (item == "wendy") then
			simpl_printcon("She's from Canberra, isn't she?")
		else	
			simpl_printcon(string.format("%s? I don't know about such things. I'm just a boy after all...", item))
		end
	end
end
