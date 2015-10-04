-- -----------------------------------------------
-- Services definition file for simplicissimus
-- Copyright (c) Andreas Bauer <baueran@gmail.com>
-- -----------------------------------------------

Services["resurrect"]  = { 
   name                   = "Resurrect", 

   heal                   = 50000,   -- Also, heal completely! No one has more than 50.000 HP!
   heal_poison            = true,
   resurrect              = true,

   print_after            = "The resurrection was successful!",

   gold                   = 300
}
