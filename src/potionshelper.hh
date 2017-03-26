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
private:
	static std::string _glob_array_name;

public:
	PotionsHelper();
	virtual ~PotionsHelper();
	static Potion* createFromLua(std::string, lua_State*);
	static bool existsInLua(std::string, lua_State*);
	static std::vector<Potion*> get_loaded_lua_potions(lua_State*);
	void drink(Potion*, lua_State*);
};

#endif
