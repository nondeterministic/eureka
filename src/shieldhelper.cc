// This source file is part of eureka
//
// Copyright (c) 2007-2018 Andreas Bauer <baueran@gmail.com>
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

#include "shieldhelper.hh"
#include "shield.hh"
#include "luawrapper.hh"

#include <lua.h>
#include <lualib.h>

ShieldHelper::ShieldHelper()
{
/*	globArrayName = "Shields";*/
}

ShieldHelper::~ShieldHelper()
{
	// TODO Auto-generated destructor stub
}

// See ShieldHelper.

Shield* ShieldHelper::createFromLua(std::string array_name, lua_State* lua_state)
{
	std::string globArrayName = "Shields";
	LuaWrapper lua(lua_state);
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

bool ShieldHelper::existsInLua(std::string item_name, lua_State* lua_state)
{
	LuaWrapper lua(lua_state);
	return lua.hasEntry("Shields", item_name);
}
