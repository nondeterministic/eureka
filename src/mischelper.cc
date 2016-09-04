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

MiscItem* MiscHelper::createFromLua(std::string array_name, MapObj* mo)
{
	std::cout << "MISCHELPER CREATING: " << array_name << ", " <<  mo->description() << ".\n";
	std::string globArrayName = "MiscItems";
	LuaWrapper lua(_lua_state);
	MiscItem *w = new MiscItem();

	w->name(lua.get_item_prop<std::string>(globArrayName, array_name, "name"));
	w->plural_name(lua.get_item_prop<std::string>(globArrayName, array_name, "plural_name"));
	w->icon = (int)(lua.get_item_prop<double>(globArrayName, array_name, "icon"));

	// If an object was passed, use its description.
	if (mo != NULL)
		w->description(mo->description());
	else
		w->description(lua.get_item_prop<std::string>(globArrayName, array_name, "description"));

	// Sometimes we want to create an item FROM a MapObj and need the MapObj's properties, so that when the player drops the item,
	// it can be restored with all the MapObj properties.
	if (mo != NULL)
		w->set_obj(*mo);

	return w;
}

// Returns true if the Lua array has an entry named item_name, false otherwise.

bool MiscHelper::exists(std::string item_name)
{
	LuaWrapper lua(_lua_state);
	return lua.hasEntry("Misc", item_name);
}
