/*
 * shieldhelper.hh
 *
 *  Created on: Jun 22, 2014
 *      Author: baueran
 */

#ifndef POTIONSHELPER_HH_
#define POTIONSHELPER_HH_

#include "potion.hh"

#include <string>

#include <lua.h>
#include <lualib.h>

class PotionsHelper
{
public:
	PotionsHelper();
	virtual ~PotionsHelper();
	static Potion* createFromLua(std::string, lua_State*);
	static bool existsInLua(std::string, lua_State*);
};

#endif
