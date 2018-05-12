-- -----------------------------------------------
-- Services definition file for eureka
-- Copyright (c) Andreas Bauer <a@pspace.org>
-- -----------------------------------------------

Services["resurrection"]  = { 
   name                   = "resurrection", 

   heal                   = 50000,   -- Also, heal completely! No one has more than 50.000 HP!
   heal_poison            = true,
   resurrect              = true,
   level_up               = false,

   print_after            = "The resurrection was successful!",

   gold                   = 300
}
