// This source file is part of eureka
//
// Copyright (c) 2007-2018 Andreas Bauer <a@pspace.org>
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

#include "armourhelper.hh"
#include "armour.hh"
#include "luawrapper.hh"

#include <lua.h>
#include <lualib.h>

ArmourHelper::ArmourHelper()
{
}

ArmourHelper::~ArmourHelper()
{
	// TODO Auto-generated destructor stub
}


Armour* ArmourHelper::createFromLua(std::string array_name, lua_State* lua_state)
{
	std::string glob_array_name = "Armour";
	LuaWrapper lua(lua_state);
	Armour *w = new Armour();

	w->name(lua.get_item_prop<std::string>(glob_array_name, array_name, "name"));
	w->plural_name(lua.get_item_prop<std::string>(glob_array_name, array_name, "plural_name"));
	w->protection((int)(lua.get_item_prop<double>(glob_array_name, array_name, "protection")));
	w->weight((int)(lua.get_item_prop<double>(glob_array_name, array_name, "weight")));
	w->icon = (int)(lua.get_item_prop<double>(glob_array_name, array_name, "icon"));
	w->gold((int)(lua.get_item_prop<double>(glob_array_name, array_name, "gold")));

	if (lua.get_item_prop<bool>(glob_array_name, array_name, "is_helmet"))
		w->set_is_helmet();

	if (lua.get_item_prop<bool>(glob_array_name, array_name, "is_shoes"))
		w->set_is_shoes();

	if (lua.get_item_prop<bool>(glob_array_name, array_name, "is_gloves"))
		w->set_is_gloves();

	return w;
}

/// Returns true if the Lua array has an entry named item_name, false otherwise.

bool ArmourHelper::existsInLua(std::string item_name, lua_State* lua_state)
{
	LuaWrapper lua(lua_state);
	return lua.hasEntry("Armour", item_name);
}
