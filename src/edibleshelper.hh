/*
 * shieldhelper.hh
 *
 *  Created on: Jun 22, 2014
 *      Author: baueran
 */

#ifndef EDIBLESHELPER_HH_
#define EDIBLESELPER_HH_

#include <string>
#include "edible.hh"

class EdiblesHelper
{
public:
	EdiblesHelper();
	virtual ~EdiblesHelper();
	static Edible* createFromLua(std::string);
	static bool exists(std::string);
};

#endif
