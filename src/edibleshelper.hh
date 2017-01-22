/*
 * shieldhelper.hh
 *
 *  Created on: Jun 22, 2014
 *      Author: baueran
 */

#ifndef EDIBLESHELPER_HH_
#define EDIBLESHELPER_HH_

#include "edible.hh"

#include <string>

#include <lua.h>
#include <lualib.h>

class EdiblesHelper
{
public:
	EdiblesHelper();
	virtual ~EdiblesHelper();
	static Edible* createFromLua(std::string, lua_State*);
	static bool existsInLua(std::string, lua_State*);
};

#endif
