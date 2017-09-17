/*
 * itemfactory.cpp
 *
 *  Created on: Jun 20, 2014
 *      Author: baueran
 */

#include <iostream>

#include "gamerules.hh"
#include "armour.hh"
#include "shield.hh"

int GameRules::bonus(int value)
{
  if (value >= 19)
    return 4;
  else if (value >= 17)
    return 3;
  else if (value >= 15)
    return 2;
  else if (value >= 12)
    return 1;
  else if (value <= 3)
    return -3;
  else if (value <= 5)
    return -2;
  else if (value <= 8)
    return -1;
  else
    return 0;
}

// Determine armour class of creature, monster or player alike

int GameRules::armour_class(int base_ac, int dext, Shield* shield, Armour* armour) // Helmet, Magic
{
	return base_ac - bonus(dext) - (shield? shield->protection() : 0) - (armour? armour->protection() : 0);
}

