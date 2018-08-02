-- -----------------------------------------------
-- Conversation definition file for eureka
-- Copyright (c) Andreas Bauer <a@pspace.org>
-- -----------------------------------------------

do
   -- --------------------------------------------------------
   -- Define character values for either party-join or a fight
   -- --------------------------------------------------------

   c_values = { 
      name = "Ignaz", race = "HUMAN", ep = 0, hp = 10, hpm = 10, sp = 0, spm = 0, str = 7, luck = 15, dxt = 15, wis = 4, charr = 14, 
      iq = 15, endd = 7, sex = "MALE", profession = "MAGE", weapon = Weapons["spear"], armour = Armour["leather armour"] 
   } 

   conv_over = false
   
   -- -----------------------------------------------
   -- Standard functions
   -- -----------------------------------------------

   function get_weapon()
      if (c_values["weapon"] ~= nil) then
         return c_values["weapon"].name
      else
         return ""
      end
   end

   function get_shield()
      if (c_values["shield"] ~= nil) then
         return c_values["shield"].name
      else
         return ""
      end
   end

   function get_armour()
      if (c_values["armour"] ~= nil) then
         return c_values["armour"].name
      else
         return ""
      end
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

   pname = ""
   
   function description()
      simpl_printcon("In front of you, a tall albeit worn out fighter.")
   end

   function name()
      simpl_printcon("They call me " .. c_values["name"] .. ". And who art though?")
      pname = simpl_getcon()
      simpl_printcon(pname .. "? That sounds familiar...")
   end
   
   function job()
      simpl_printcon("In my younger days, I was what you would call a mercenary, but I now live on this farm with my daughter Inge " ..
			"and seek to fight only when fate leaves me no choice.")
   end
   
   function join()
      if (simpl_partysize() < 6) then
         simpl_printcon("I will join thee. I know that my daughter Inge will cope until we return. She has grown up a lot " ..
                           "and the enemy hordes shall not return so soon again. We did not beat them, but gave them a huge blow, " ..
                           "from which they will need to recover. But let us go now and not loose any further time!")
         join_successful = simpl_join()
         simpl_remove_from_current_map("ignaz")
         conv_over = true
         return true
      end
      simpl_printcon("You seem to have a whole party already.")
      return false
   end
  
   function bye()
      simpl_printcon("May God be with you!")
   end
   
   function otherwise(item)
      if (item == "daughter" or item == "inge") then
         simpl_printcon("She must be around here somewhere, possibly in the stables, milking the cows...")
      elseif (item == "melnior" or string.find(item, "sent")) then
         simpl_printcon("Melnior and I go a long way back. I tried to help him defend Velnebras, but the enemies were too many. They left behind " ..
                           "a great devastation and many deaths. Now I also remember, why you seem familiar to me: I fought side by side with your " ..
                        "brave wife Evelyn until...")
      elseif (string.find(item, "until") or item == "evelyn" or item == "wife") then
         simpl_printcon("...until Edvard got killed. She then returned to her home, and I have not seen her since. Have you seen her there? (y/n)")
         answer = simpl_getkey("yn")
         simpl_printcon(string.format("%s ", answer))
         
         if (answer == "y") then
            simpl_printcon("I am glad to know she is alive.")
         else
            simpl_printcon("That is what I feared: she may be out to avenge Edvard on her own. Together we may be able to find her, before it is too late. Dost thou accept my services to your quest? (y/n)")
            answer = simpl_getkey("yn")
            simpl_printcon(string.format("%s ", answer))
            
            if (answer == "y") then
               join()
            else
               simpl_printcon("As you wish, but do come by my house again at any time, should you ever change your mind. Attempting this huge task alone is suicide.")
            end
         end
      elseif (item == "enemy") then
         simpl_printcon("The enemy must not be underestimated!")
      elseif (item == "velnebras") then
         simpl_printcon("It is... was a once proud city west of here.")
      elseif (item == "fight" or item == "choice") then
         simpl_printcon("Velnebras did not leave me a choice - I had to fight.")
      elseif (item == "dog") then
         simpl_printcon("The dog's name is Emma. She is the best and very protective of Inge.")
      else	
         simpl_printcon("Not sure, what this means.")
      end
   end
end
