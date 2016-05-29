-- -----------------------------------------------
-- Conversation definition file for eureka
-- Copyright (c) Andreas Bauer <baueran@gmail.com>
-- -----------------------------------------------

do
   -- --------------------------------------------------------
   -- Define character values for either party-join or a fight
   -- --------------------------------------------------------

   c_values = { 
      name = "Frederik", race = "HUMAN", ep = 0, hp = 20, hpm = 20, sp = 0, spm = 0, 
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
      simpl_printcon("Before you, you see a very young boy with a friendly but at the same time serious face.")
   end

   function name()
      simpl_printcon("I'm Frederik, but everyone calls me Fredi.")
   end
   
   function job()
      simpl_printcon("I help father in his shop. And one day I will also be a blacksmith and perhaps continue his shop, God willing.")
   end

   function join()
      simpl_printcon("You can't be serious. I'm only a boy, not an adventurer. Besides, my father needs me here.")
      return false
   end
   
   function bye()
      simpl_printcon("I too must get back to my duties. Farewell!")
   end
   
   function otherwise(item)
      if (item == "Fredi" or string.find(item, "nick")) then
	 simpl_printcon("Fredi is my nickname.")
      elseif (item == "mother") then
	 simpl_printcon("Her death seems so far away, I can barely remember her. But father told me that we was ambushed and killed in the forest, collecting brushwood. The orcs who are responsible were never found or, worse, even looked for.")
      elseif (string.find(item, "orc")) then
	 msg = "I have never seen one myself, but everyone knows they are lurking outside these city walls, in the lands of this kingdom, in the forests, waiting for "
	 .. "passers by... Sorry, it is difficult for me to speak about this. I just wish someone would help us. We're only safe in the city, where our rulers protect us, but "
	 .. "it really feels like being a prisoner."
	 simpl_printcon(msg)
      elseif (string.find(item, "prison") or item == "city" or item == "safe") then
	 msg = "It seems everyone is finding it normal that one cannot go outside the city anymore, but I remember that mother and I went playing in the forests, at the rivers, in the fields. "
	    .. "These are wonderful memories. But somehow noone else seems to want to go outside anymore. People say they are happy. I cannot believe that. How foolish is that?"
	 simpl_printcon(msg)
      elseif (string.find(item, "ruler")) then
	 msg = "I don't know much about the rulers. People love our elected rulers. Father never talks about this much though..."
	 simpl_printcon(msg)
      else	
	 simpl_printcon("I cannot help you with that.")
      end
   end
end
