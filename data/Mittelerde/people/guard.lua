-- -----------------------------------------------
-- Conversation definition file for simplicissimus
-- Copyright (c) Andreas Bauer <baueran@gmail.com>
-- -----------------------------------------------

do
	-- --------------------------------------------------------
	-- Define character values for either party-join or a fight
	-- --------------------------------------------------------

	c_values = { 
		name = "Guard", race = "HUMAN", ep = 0, hp = 20, hpm = 20, sp = 0, spm = 0, str = 16, luck = 15, dxt = 12, wis = 4, charr = 10, 
		iq = 10, endd = 14, sex = "MALE", profession = "FIGHTER", weapon = Weapons["axe"], shield = Shields["small shield"] 
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

	function join()
	        simpl_printcon("In your dreams matey.")
		return false
	end
	
	function bye()
		simpl_printcon("I better get going, too. Can't stand around chatting with you all day.")
	end
	
	function otherwise(item)
		simpl_printcon(string.format("%s? I don't know about such things. I'm just a guard after all...", item))
	end
end
