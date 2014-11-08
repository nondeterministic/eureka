/*
 * shieldhelper.hh
 *
 *  Created on: Jun 22, 2014
 *      Author: baueran
 */

#ifndef SHIELDHELPER_HH_
#define SHIELDHELPER_HH_

#include <string>
#include "shield.hh"

class ShieldHelper
{
public:
	ShieldHelper();
	virtual ~ShieldHelper();
	static Shield* createFromLua(std::string);
	static bool exists(std::string);
};

#endif /* SHIELDHELPER_HH_ */
