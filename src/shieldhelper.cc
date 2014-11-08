/*
 * shieldhelper.cc
 *
 *  Created on: Jun 22, 2014
 *      Author: baueran
 */

#include "shieldhelper.hh"
#include "shield.hh"
#include "luawrapper.hh"
#include "luaapi.hh"        // to define _lua_state

ShieldHelper::ShieldHelper()
{
/*	globArrayName = "Shields";*/
}

ShieldHelper::~ShieldHelper()
{
	// TODO Auto-generated destructor stub
}

// See ShieldHelper.

Shield* ShieldHelper::createFromLua(std::string array_name)
{
	std::string globArrayName = "Shields";
	LuaWrapper lua(_lua_state);
	Shield *w = new Shield();

	w->name(lua.get_item_prop<std::string>(globArrayName, array_name, "name"));
	w->plural_name(lua.get_item_prop<std::string>(globArrayName, array_name, "plural_name"));
	w->protection((int)(lua.get_item_prop<double>(globArrayName, array_name, "protection")));
	w->weight((int)(lua.get_item_prop<double>(globArrayName, array_name, "weight")));
	w->icon = (int)(lua.get_item_prop<double>(globArrayName, array_name, "icon"));
	w->gold((int)(lua.get_item_prop<double>(globArrayName, array_name, "gold")));
	return w;
}

// Returns true if the Lua array has an entry named item_name,
// false otherwise.

bool ShieldHelper::exists(std::string item_name)
{
	LuaWrapper lua(_lua_state);
	return lua.hasEntry("Shields", item_name);
}
