-- -----------------------------------------------
-- Conversation definition file for simplicissimus
-- Copyright (c) Andreas Bauer <baueran@gmail.com>
-- -----------------------------------------------

do
   -- --------------------------------------------------------
   -- Define character values for either party-join or a fight
   -- --------------------------------------------------------

   c_values = { 
      name = "Wilfried", race = "HUMAN", ep = 0, hp = 10, hpm = 10, sp = 0, spm = 0, str = 7, luck = 15, dxt = 15, wis = 4, charr = 14, 
      iq = 15, endd = 7, sex = "MALE", profession = "TINKER", weapon = Weapons["axe"], shield = Shields["small shield"] 
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
      simpl_printcon("You see a friendly looking bard with a beard.")
   end

   function name()
      simpl_printcon("I'm Wilfried, and what is your name?")
      name_answer = simpl_getcon()
      simpl_printcon("Nice to meet you, " .. name_answer .. ".")
   end
   
   function job()
      simpl_printcon("My job is to entertain people with songs. I'm a bard after all.")
   end

   function join()
      if (pc_party) then
	 simpl_printcon("I am tempted and flattered, but I think it safer to stay within those city walls.")
      else
	 simpl_printcon("Sorry, as much as I enjoyed talking to you. It seems that you and I are not, how shall I say, compatible. You do not wish us to collectively advance, but are more concerned for yourself. I do not want to join you in that quest. I hope you understand.")
      end
      return false
   end
  
   function bye()
      if (pc_party) then
	 if (string.len(name_answer) > 0) then
	    simpl_printcon("Farewell " .. ", it was a pleasure meeting you.")
	 else
	    simpl_printcon("Farewell stranger, it was a pleasure meeting you.")
	 end
      else
	 msg = "Farewell, it was interesting talking to you, though I wish you'd think your antisocial positions over. "
	 .. "We are a community that only works, if we obey to certain rules."
	 simpl_printcon(msg)
      end
   end
   
   function otherwise(item)
      if (item == "bard" or item == "entertain" or string.find(item, "song")) then
	 msg = "Bards offer first and foremost two services to those who will listen: we entertain with song and dance, but we also travel and speak the truth."
	 simpl_printcon(msg)
      elseif (item == "liberty") then
	 simpl_printcon("Liberty indeed, but of course not at the expense of others. First and foremost, we need greater social justice in this kingdom.")
      elseif (item == "lies") then
	 simpl_printcon("It seems you too have fallen for the false propaganda of some who do not wish a free and equal society in the future. This society, however, has no room for people like you. Your intolerant ideas of how people should live are not ours.")
	 pc_party = false
      elseif (item == "truth") then
	 msg = "Yes, every song also has an element of truth in it, don't you know? Even if it will take the listener some time to figure that one out. Dost thou wish to hear a song of mine? (y/n)"
	 simpl_printcon(msg)
	 song_answer = simpl_getkey("yn")
	 simpl_printcon(string.format("%s ", song_answer))
	 if (song_answer == "y") then
	    simpl_printcon("To and fro the kingdom, we shall overcome the dark past of our fathers.")
	    simpl_printcon("Who gave us war and conflict. But now we shall rise like a Phoenix from the ashes.")
	    simpl_printcon("So rejoice people of this kingdom, for the new dawn is here which will bring liberty, equality and fraternity for us all.")
	    simpl_printcon("La la la...")
	    -- TODO: Refine song / poem

	    simpl_printcon("Is this not an uplifting message? (y/n)")
	    uplift = simpl_getkey("yn")
	    if (uplift == "y") then
	       simpl_printcon("Indeed, I knew you were a good person, when I saw you. We share the same ideals, it seems.")
	    else
	       simpl_printcon("Curious. How can anyone not support the cause of liberty, equality and fraternity? I guess, you believe in all those lies that some people tell...")
	    end
	 else
	    simpl_printcon("Not in the mood then, I take it...")
	 end
      elseif (string.find(item, "ideal") or string.find(item, "social") or string.find(item, "justice")) then
	 msg = ""
	 simpl_printcon(msg)
      else
	 simpl_printcon(string.format("I cannot help you with %s.", item))
      end
   end
end
