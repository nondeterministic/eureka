/*
 * shieldhelper.hh
 *
 *  Created on: Jun 22, 2014
 *      Author: baueran
 */

#ifndef EDIBLESHELPER_HH_
#define EDIBLESHELPER_HH_

#include "edible.hh"
#include "playercharacter.hh"

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
	void eat_player(Edible*, PlayerCharacter*);
	void eat_party(Edible*);
};

#endif
