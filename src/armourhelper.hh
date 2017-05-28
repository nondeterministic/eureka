#ifndef ARMOURHELPER_HH_
#define ARMOURHELPER_HH_

#include <string>

#include "armour.hh"

#include <lua.h>
#include <lualib.h>

class ArmourHelper
{
public:
	ArmourHelper();
	virtual ~ArmourHelper();
	static Armour* createFromLua(std::string, lua_State*);
	static bool existsInLua(std::string, lua_State*);
};

#endif
