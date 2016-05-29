-- -----------------------------------------------
-- Services definition file for eureka
-- Copyright (c) Andreas Bauer <baueran@gmail.com>
-- -----------------------------------------------

Services["heal light wounds"]  = { 
   name                   = "heal light wounds", 
   
   heal                   = math.random(5,15),
   heal_poison            = true,
   resurrect              = false,
   level_up               = false,

   print_after            = "Healing of light wounds was successful!",

   gold                   = 5
}
