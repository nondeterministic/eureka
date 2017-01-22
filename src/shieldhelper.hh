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

#include <lua.h>
#include <lualib.h>

class ShieldHelper
{
public:
	ShieldHelper();
	virtual ~ShieldHelper();
	static Shield* createFromLua(std::string, lua_State*);
	static bool existsInLua(std::string, lua_State*);
};

#endif /* SHIELDHELPER_HH_ */
