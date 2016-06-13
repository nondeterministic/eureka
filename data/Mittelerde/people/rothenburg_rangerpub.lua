-- -----------------------------------------------
-- Conversation definition file for eureka
-- Copyright (c) Andreas Bauer <baueran@gmail.com>
-- -----------------------------------------------

do
   -- --------------------------------------------------------
   -- Define character values for either party-join or a fight
   -- --------------------------------------------------------

   c_values = { 
      name = "Sascha", race = "HUMAN", ep = 0, hp = 10, hpm = 10, sp = 0, spm = 0, str = 7, luck = 15, dxt = 15, wis = 4, charr = 14, 
      iq = 15, endd = 7, sex = "MALE", profession = "RANGER", weapon = Weapons["axe"], shield = Shields["small shield"] 
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

   pc_party = true -- Set to false when bard realises, party is not PC
   name_answer = ""

   function description()
      simpl_printcon("You see a serious looking man with a bow tied around his body.")
   end

   function name()
      simpl_printcon("My name is Sascha.")
   end
   
   function job()
      simpl_printcon("By profession I am a ranger, but these days I mostly try to protect our plight for more freedom and democracy.")
   end

   function join()
      simpl_printcon("I cannot join you on your quest. I am needed here to protect our civil liberties.")
   end
  
   function bye()
      simpl_printcon("Good bye my friend, and please let me know if you hear or see anything suspicious. Our enemies can be anywhere. Take care.")
   end
   
   function otherwise(item)
      if (string.find(item, "democra") or item == "freedom") then
	 msg = "Democracy is the epitome of leadership and far superior to being ruled by a single king who has first and foremost his own interests in mind. "
	    .. "I am glad, the king is gone, don't you agree? (y/n)"
	 simpl_printcon(msg)
	 king_answ = simpl_getkey("yn")
	 if (king_answ == "y") then
	    simpl_printcon("I let you in on a secret: though the king is exiled, he still has followers in this kingdom, who are eager to reinstate him back to office.")
	 else
	    simpl_printcon("You can't be serious.")
	 end
      elseif (string.find(item, "exile")) then
	 msg = "Ever since the king was forced from these lands, his tracks were somewhat lost. "
	    .. "It is rumoured he is building an army outside this kingdom to regain power. But who knows?"
	 simpl_printcon(msg)
      elseif (item == "army" or item == "outside" or item == "kingdom") then
	 simpl_printcon("As I said to you before, I am here to prevent that and I am ready to fight the enemies of our free and democratic society, with all means if necessary.")
      elseif (item == "enemies") then
	 msg = "In times of old, we had to fear mainly orcs in the forests and the king's guards in the cities. "
	 .. "But now that we govern ourselves by free and fair elections, we no longer face such perils. "
	 .. "Instead, we now have to be on the lookout from royalists, traitors and other conservatives who oppose our new civil liberties. "
	 .. "And this is why I am here: to protect these liberties."
	 simpl_printcon(msg)
      elseif (string.find(item, "libert")) then
	 msg = "Our liberties are many. For example, we all have the equal right to vote in free elections, send our children to school, "
	 .. "employ many of the services, our elected rulers provide us with. The price to pay is that everyone has to pay their taxes, of course."
	 simpl_printcon(msg)
      elseif (string.find(item, "tax")) then
	 simpl_printcon("Taxes are necessity, but you may want to ask our local tax officer as I am no expert on that topic.")
      else
	 simpl_printcon(string.format("Not sure what you mean by %s.", item))
      end
   end
end
