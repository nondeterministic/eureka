-- -----------------------------------------------
-- Conversation definition file for eureka
-- Copyright (c) Andreas Bauer <baueran@gmail.com>
-- -----------------------------------------------

do
   -- --------------------------------------------------------
   -- Define character values for either party-join or a fight
   -- --------------------------------------------------------

   c_values = { 
      name = "Woman", race = "HUMAN", ep = 0, hp = 20, hpm = 20, sp = 0, spm = 0, 
      str = 16, luck = 15, dxt = 12, wis = 4, charr = 10, 
      iq = 10, endd = 14, sex = "FEMALE", profession = "SHEPHERD", weapon = Weapons["axe"], 
      shield = Shields["small shield"] 
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

   function description()
      simpl_printcon("A young, but scholarly looking woman throws a serious look at you...") 
   end

   function name()
      simpl_printcon("My name is Tanya.")
   end
   
   function job()
      simpl_printcon("I am the new librarian.")
   end

   function join()
      simpl_printcon("No, my place is here amongst the books.")
      return false
   end
   
   function bye()
      simpl_printcon("Fare well.")
   end
   
   function otherwise(item)
      if (item == "new" or item == "librarian" or item == "library") then
	 simpl_printcon("Unfortunately, the previous librarian needed replacing since he would try to hide books from their removal by the governour's servants.")
      elseif (item == "removal" or item == "book removal" or string.find(item, "servant") or string.find(item, "governo")) then
	 msg = "There are books in existence whose content is detrimental to a peaceful coexistence within society. "
	 .. "Clearly, it was a wise decision of the governour to protect the public from such content, dost thou not agree? (y/n)"
	 simpl_printcon(msg)
	 books_answer = simpl_getkey("yn")
	 simpl_printcon(string.format("%s ", books_answer))
	 if (books_answer == "y") then
	    simpl_printcon("Indeed, I have always made sure that my boy, John, grows up learning about the right set of human values.")
	 else
	    simpl_printcon("Oh, so I take it, you have not become a public servant then yourself. (Smiles ironically.)")
	 end
      elseif (string.find(item, "servant") or string.find(item, "public")) then
	 msg = "Many people have closed their businesses in order to become public servants. "
	    .. "Apparently they are necessary to organise a kingdom the size of ours..."
	 simpl_printcon(msg)
      elseif (item == "boy" or item == "John") then
	 simpl_printcon("I am his mother, but I do believe that he is a very intelligent little boy. He comes with me to the library a lot, because he likes to read books that even I find difficult to read. But I let him. It may do him good.")
      else	
	 simpl_printcon("I cannot help you with that.")
      end
   end
end
