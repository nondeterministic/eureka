/*
 * shieldhelper.cc
 *
 *  Created on: Jun 22, 2014
 *      Author: baueran
 */

#include "mischelper.hh"
#include "miscitem.hh"
#include "luawrapper.hh"
#include "luaapi.hh"        // to define _lua_state
#include "party.hh"
#include "gamecontrol.hh"

#include <string>
#include <iostream>

#include <boost/algorithm/string.hpp>

MiscHelper::MiscHelper()
{
/*	globArrayName = "Shields";*/
}

MiscHelper::~MiscHelper()
{
	// TODO Auto-generated destructor stub
}

// See ShieldHelper.

MiscItem* MiscHelper::createFromLua(std::string array_name)
{
	std::string globArrayName = "MiscItems";
	LuaWrapper lua(_lua_state);
	MiscItem *w = new MiscItem();

	w->name(lua.get_item_prop<std::string>(globArrayName, array_name, "name"));
	w->plural_name(lua.get_item_prop<std::string>(globArrayName, array_name, "plural_name"));

	return w;
}

// Returns true if the Lua array has an entry named item_name, false otherwise.

bool MiscHelper::exists(std::string item_name)
{
	LuaWrapper lua(_lua_state);
	return lua.hasEntry("Misc", item_name);
}
