/*
 * gamerules.hh
 *
 *  Created on: Jun 20, 2014
 *      Author: baueran
 */

#ifndef GAMERULES_HH_
#define GAMERULES_HH_

#include "shield.hh"
#include "armour.hh"

namespace GameRules
{
	int armour_class(int base_ac, int dext, Shield* shield, Armour* armour);
	int bonus(int);
}

#endif /* GAMERULES_HH_ */
