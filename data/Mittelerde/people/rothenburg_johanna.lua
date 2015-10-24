-- -----------------------------------------------
-- Conversation definition file for simplicissimus
-- Copyright (c) Andreas Bauer <baueran@gmail.com>
-- -----------------------------------------------

do
   -- --------------------------------------------------------
   -- Define character values for either party-join or a fight
   -- --------------------------------------------------------

   c_values = { 
      name = "Johanna", race = "HUMAN", ep = 0, hp = 20, hpm = 20, sp = 0, spm = 0, 
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
      simpl_printcon("You see a poor albeit proud looking woman, who stares into the campfire and does not seem to take much notice of your presence.")
   end

   function name()
      simpl_printcon("I am Johanna.")
   end
   
   function job()
      simpl_printcon("Before the king ruled these lands, I was a shepherd once.")
   end

   function join()
      simpl_printcon("Under different circumstances, perhaps. For now, my duty is to sit here and wait.")
      return false
   end
   
   function bye()
      simpl_printcon("Let us hope that we meet under better circumstances next time...")
   end
   
   function otherwise(item)
      if (item == "king") then
	 simpl_printcon("I do not know much about him, only that he seems to be very popular with the people. Dost thou love the king as well? (y/n)")
	 love_answer = simpl_getkey("yn")
	 simpl_printcon(string.format("%s ", love_answer))
	 if (love_answer == "y") then
	    simpl_printcon("Interesting. (Attends back to the fire.)")
	 else
	    simpl_printcon("Oh, so I take it, you have not become a public servant then yourself. (Smiles ironically.)")
	 end
      elseif (item == "shepherd" or item == "herd") then
	 simpl_printcon("My husband and I had a herd once of our own, but we could not afford to let them grass on these lands any longer.") 
      elseif (item == "husband") then
	 msg = "He has been sent to prison about a year ago for letting our herd roam in the king's forests and without obtaining "
	 .. "a royal permission certificate. He was aware of the risks involved, but decided it was worth it. "
	 .. "Now he has to pay the price, I guess. I just hope to be able to see him again some day..."
	 simpl_printcon(msg)
      elseif (string.find(item, "servant") or string.find(item, "public")) then
	 msg = "Many people have closed their businesses in order to become public servants. "
	    .. "Apparently they are needed to organise a kingdom the size of our king's..."
	 simpl_printcon(msg)
      elseif (item == "lies") then
	 msg = "Shhhh, not so loud, stranger! You must not publicly question the collective opinion, it might cost you dearly."
	 simpl_printcon(msg)
      elseif (string.find(item, "collective") or string.find(item, "opinion") or item == "cost") then
	 msg = "The king's informers are everywhere, and they are all too happy to denounce even their closest neighbours and friends. "
	    .. "But there exists a small network of critical minds, who do not buy into the altruistic image of the powers that be, and "
	    .. "whom you can count on in times of need."
	 simpl_printcon(msg)
      elseif (string.find(item, "network") or string.find(item, "critical") or string.find(item, "minds")) then
	 msg = "For the sake of everyone's safety, no one knows exactly who is in this network and who is not. If you really are what "
	    .. "you pretend to be, go and find TODO in TODO, and ask him about the lies as well. He can introduce you to some potentially "
	    .. "likeminded fellows."
	 -- TODO: Add name and town of underground contact!
	 simpl_printcon(msg)
      else	
	 simpl_printcon("I cannot help you with that.")
      end
   end
end
