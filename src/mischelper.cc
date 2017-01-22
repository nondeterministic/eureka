// This source file is part of eureka
//
// Copyright (c) 2007-2016  Andreas Bauer <baueran@gmail.com>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
// USA.

#include "mischelper.hh"
#include "miscitem.hh"
#include "luawrapper.hh"
#include "party.hh"
#include "gamecontrol.hh"

#include <string>
#include <iostream>

#include <lua.h>
#include <lualib.h>

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

MiscItem* MiscHelper::createFromLua(std::string array_name, lua_State* lua_state, MapObj* mo)
{
	std::string globArrayName = "MiscItems";
	LuaWrapper lua(lua_state);
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

bool MiscHelper::existsInLua(std::string item_name, lua_State* lua_state)
{
	LuaWrapper lua(lua_state);
	return lua.hasEntry("Misc", item_name);
}
