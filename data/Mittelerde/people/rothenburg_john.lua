-- -----------------------------------------------
-- Conversation definition file for eureka
-- Copyright (c) Andreas Bauer <baueran@gmail.com>
-- -----------------------------------------------

do
   -- --------------------------------------------------------
   -- Define character values for either party-join or a fight
   -- --------------------------------------------------------

   c_values = { 
      name = "John", race = "HUMAN", ep = 0, hp = 20, hpm = 20, sp = 0, spm = 0, 
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
      simpl_printcon("Before you, a young child with a serious face.")
   end

   function name()
      simpl_printcon("My name is John.")
   end
   
   function job()
      simpl_printcon("I am only a child, and don't have a job. But I want to become a philosopher.")
   end

   function join()
      simpl_printcon("For now my place is with my mother and my books.")
      return false
   end
   
   function bye()
      simpl_printcon("You should look around, perhaps you find something that interests you.")
   end
   
   function otherwise(item)
      if (item == "read" or string.find(item, "librar")) then
	 simpl_printcon("At first I was given only childrens' books, but I quickly got bored of them. These days I read books on history and philosophy as these are more exciting to me, even if it sometimes takes me a while, before I understand certain things.")
      elseif (item == "history" or item == "philosophy" or item == "books") then
	 simpl_printcon("What I find mainly fascinating is how different the books are from what adults teach me. It seems that either the books are all wrong, or the adults have never read or understood them. I hope that one day I will make sense of all this.")
      elseif (item == "philosopher") then
	 simpl_printcon("Indeed, the concept of reaching certain conclusions based on premises and deduction is fascinating to me. It makes you challenge the common consensus.")
      elseif (item == "consensus") then
	 msg = "For example, it seems, there is a consensus amongst all adults in this country that the expansion of public law on the expense of private liberties is something "
	 .. "desirable, even though it seems logical to me that a small group of people cannot know what is desirable for each and every individual. Dost thou not agree? (y/n) "
	 simpl_printcon(msg)
	 books_answer = simpl_getkey("yn")
	 simpl_printcon(string.format("%s ", books_answer))
	 if (books_answer == "y") then
	    simpl_printcon("Indeed, not many would agree to that, though it seems obvious.")
	 else
	    simpl_printcon("Indeed, most people would not agree and it is beyond me as to why. Perhaps one day I will find out, when I am older. I might be too young to understand right now.")
	 end
      elseif (string.find(item, "prison") or item == "city" or item == "safe") then
	 msg = "It seems everyone is finding it normal that one cannot go outside the city anymore, but I remember that mother and I went playing in the forests, at the rivers, in the fields. "
	    .. "These are wonderful memories. But somehow noone else seems to want to go outside anymore. People say they are happy. I cannot believe that. How foolish is that?"
	 simpl_printcon(msg)
      else	
	 simpl_printcon("I can't help you with that.")
      end
   end
end
