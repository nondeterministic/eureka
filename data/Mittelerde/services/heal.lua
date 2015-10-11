-- -----------------------------------------------
-- Services definition file for simplicissimus
-- Copyright (c) Andreas Bauer <baueran@gmail.com>
-- -----------------------------------------------

Services["heal"]  = { 
   name                   = "heal", 

   heal                   = 50000,   -- Heal completely! No one has more than 50.000 HP!
   heal_poison            = true,
   resurrect              = false,

   print_after            = "Healing was successful!",

   gold                   = 25
}
