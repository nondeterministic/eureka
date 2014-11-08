-- -----------------------------------------------
-- Conversation definition file for simplicissimus
-- Copyright (c) Andreas Bauer <baueran@gmail.com>
-- -----------------------------------------------

do
	-- -----------------------------------------------
	-- Standard terms
	-- -----------------------------------------------

	function description()
		simpl_printcon("You see a stocky guard, not looking too friendly at you...")
	end

	function name()
		simpl_printcon("I'm Skippy the bush kangaroo. No seriously, who are you to walk around asking such questions?")
		answer = simpl_getcon()
		simpl_printcon(string.format("%s ", answer))
		simpl_printcon(string.format("%s? That's one hell of a weird name, mate.", answer))
	end
	
	function job()
		simpl_printcon("What do *you* think a guard does all day?")
		answer = simpl_getcon()
		simpl_printcon(string.format("%s ", answer))
		simpl_printcon(string.format("%s? You have no idea...", answer))
	end
	
	function bye()
		simpl_printcon("I better get going, too. Can't stand around chatting with you all day.")
	end
	
	-- -----------------------------------------------
	
	function otherwise(item)
		simpl_printcon(string.format("%s? I don't know about such things. I'm just a guard after all...", item))
	end
end
