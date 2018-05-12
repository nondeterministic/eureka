-- -----------------------------------------------
-- Services definition file for eureka
-- Copyright (c) Andreas Bauer <a@pspace.org>
-- -----------------------------------------------

Services["heal poisonous state"]  = { 
   name                   = "heal poisonous state", 
   
   heal                   = 0,
   heal_poison            = true,
   resurrect              = false,
   level_up               = false,

   print_after            = "Healing of poisonous state was successful!",

   gold                   = 10
}
