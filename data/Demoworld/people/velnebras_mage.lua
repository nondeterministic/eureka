-- -----------------------------------------------
-- Conversation definition file for eureka
-- Copyright (c) Andreas Bauer <a@pspace.org>
-- -----------------------------------------------

do
   -- --------------------------------------------------------
   -- Define character values for either party-join or a fight
   -- --------------------------------------------------------

   c_values = { 
      name = "Melnior", race = "HUMAN", ep = 0, hp = 10, hpm = 10, sp = 0, spm = 0, str = 7, luck = 15, dxt = 15, wis = 4, charr = 14, 
      iq = 15, endd = 7, sex = "MALE", profession = "MAGE", weapon = Weapons["axe"], shield = Shields["small shield"] 
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

   function get_armour()
      return "" -- we ignore armour
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
      simpl_printcon("You see an old, bent down man.")
   end

   function name()
      simpl_printcon("I am " .. c_values["name"] .. " the wizard of Velnebras. And who is it that I am speaking to?")
      pname = simpl_getcon()
      simpl_printcon(pname .. " you say? I have known your wife, Evelyn, and your son Edvard. "
		     .. "Both came here to seek my help, when they attacked us.")
   end
   
   function job()
      simpl_printcon("My duties are to rule this town, Alas! it is a depressive task since the devastation.")
   end
   
   function join()
      simpl_printcon("Sorry, Velnebras needs me now to rebuild.")
      return false
   end
  
   function bye()
      simpl_printcon("May God be with you!")
   end
   
   function otherwise(item)
      if (item == "wife" or item == "son" or item == "evelyn" or item == "edvard") then
         simpl_printcon("It all happened so fast, there was no hope for Edvard. Your wife only got injured, but I do not know " ..
                           "what became of her as the battle dust had settled. She fought bravely and we believe, she took " ..
                           "Edvard back to your home. Have you spoken to her ever since? (y/n)")
         answer = simpl_getkey("yn")
         simpl_printcon(string.format("%s ", answer))
         
         if (answer == "y") then
            simpl_printcon("I am relieved to hear she is alive!")
         else
            simpl_printcon("This is curious! I hope she did not foolishly try to avenge Edvard's death...")
         end
      elseif (item == "avenge" or item == "death") then
         simpl_printcon("She blamed herself for not being able to defend him. And the enemy showed no mercy. " ..
                           "You must try and find her, before she finds the enemy - or rather: before the enemy finds her. " ..
                        "I will arrange for the guards to open the gate to what is left of our armoury. In it, you will find " ..
                           "some magical items, which shall help you in your quest. But you must not loose any more time! " ..
                           "You must find her, and return her to safety! It may be that Ignaz of Sarsen may be able to assist you. " ..
                           "Sarsen lies east of here beyond the great river and in the mountains near the coast. " ..
                           "Tell him that I sent you, and he might join you. He fought hard in the battle.")
         simpl_play_sound("55820__sergenious__bridge.wav")
         simpl_change_icon(12, 19, 71, 72)
      elseif (item == "enemy") then
         simpl_printcon("He is still out there. Be careful!")
      elseif (item == "devastation") then
         simpl_printcon("The enemy spared almost none, and I blame myself.")
      elseif (item == "blame") then
         simpl_printcon("I should have been able to protect this city better...")
      else	
         simpl_printcon("I am sorry, I cannot help you with that.")
      end
   end
end
