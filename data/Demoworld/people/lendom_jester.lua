-- -----------------------------------------------
-- Conversation definition file for eureka
-- Copyright (c) Andreas Bauer <baueran@gmail.com>
-- -----------------------------------------------

do
   -- --------------------------------------------------------
   -- Define character values for either party-join or a fight
   -- --------------------------------------------------------

   c_values = { 
      name = "Brae", race = "HUMAN", ep = 0, hp = 10, hpm = 10, sp = 0, spm = 0, str = 7, luck = 15, dxt = 15, wis = 4, charr = 14, 
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
      simpl_printcon("You see an old looking, bent man with a young but serious face.")
   end

   function name()
      simpl_printcon("I am " .. c_values["name"] .. ".")
   end

   function job()
      simpl_printcon("I was am, and always will be a bard. I travel the lands, and sing songs about the world. After the enemy came here though, I do not feel like singing. Can you relate to that? (y/n)")
      answer = simpl_getkey("yn")
      if (answer == "y") then
	 simpl_printcon("Who couldn't? The losses were terrible...")
      else
	 simpl_printcon("I guess, you have to have been there to understand...")
      end
   end
   
   function join()
      simpl_printcon("No, my friend, I have seen enough battle and bloodshed. I will be back to being a bard now. I won't touch a sword anymore - unless I have to in order to defend myself.")
      return false
   end
  
   function bye()
      simpl_printcon("They shall sing songs about you and your quest, I hope.")
   end
   
   function otherwise(item)
      if (item == "bard") then
         simpl_printcon("There used to be many of us. But many were not prepared for such a fierce battle. We travel the lands and bring song about what is going on...")
      elseif (item == "battle" or item == "news") then
         simpl_printcon("The people around here seem so confident that the enemy is defeated. But I tell you, he merely sleeps. He is still there. " ..
                           "For instance, they have dug tunnels to get through the once existing city walls. No one knows what became of them. Some people swear, they can still hear noises " ..
                           "down below. I think, these people are right. The enemy is still closer than we think.")
      elseif (item == "enemy" or item == "noises") then
         simpl_printcon("There is talk, the tunnels under the town still exist. The enemy is still in there, regaining strength...")
      elseif (item == "tunnel" or item == "tunnels") then
         simpl_printcon("It was a tactic of the enemy: to dig a tunnel under our city walls and then attack us from the inside and outside.")
      else
         simpl_printcon("I am sorry, I cannot help you with that.")
      end
   end
end
