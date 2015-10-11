/*
 * shieldhelper.cc
 *
 *  Created on: Jun 22, 2014
 *      Author: baueran
 */

#include "serviceshelper.hh"
#include "service.hh"
#include "luawrapper.hh"
#include "luaapi.hh"        // to define _lua_state

#include <string>

#include <boost/algorithm/string.hpp>

ServicesHelper::ServicesHelper()
{
/*	globArrayName = "Shields";*/
}

ServicesHelper::~ServicesHelper()
{
	// TODO Auto-generated destructor stub
}

// See ShieldHelper.

Service* ServicesHelper::createFromLua(std::string array_name)
{
	std::string globArrayName = "Services";
	LuaWrapper lua(_lua_state);
	Service *w = new Service();

	w->name(lua.get_item_prop<std::string>(globArrayName, array_name, "name"));
	w->heal = (int)(lua.get_item_prop<double>(globArrayName, array_name, "heal"));
	w->heal_poison = lua.get_item_prop<bool>(globArrayName, array_name, "heal_poison");
	w->resurrect = lua.get_item_prop<bool>(globArrayName, array_name, "resurrect");
	w->print_after = lua.get_item_prop<std::string>(globArrayName, array_name, "print_after");
	w->gold((int)(lua.get_item_prop<double>(globArrayName, array_name, "gold")));

	return w;
}

// Returns true if the Lua array has an entry named item_name, false otherwise.

bool ServicesHelper::exists(std::string item_name)
{
	LuaWrapper lua(_lua_state);
	return lua.hasEntry("Services", item_name);
}
