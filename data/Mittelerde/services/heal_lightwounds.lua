-- -----------------------------------------------
-- Services definition file for simplicissimus
-- Copyright (c) Andreas Bauer <baueran@gmail.com>
-- -----------------------------------------------

Services["heal_light"]  = { 
   name                   = "Heal light wounds", 
   
   heal                   = math.random(5,15),
   heal_poison            = true,
   resurrect              = false,

   print_after            = "Healing of light wounds was successful!",

   gold                   = 5
}
