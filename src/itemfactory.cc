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

#include "itemfactory.hh"
#include "item.hh"
#include "potionshelper.hh"
#include "edibleshelper.hh"
#include "weaponhelper.hh"
#include "shieldhelper.hh"
#include "serviceshelper.hh"
#include "armourhelper.hh"
#include "mischelper.hh"
#include "util.hh"
#include "world.hh"

#include <lua.h>
#include <lualib.h>

#include <string>
#include <vector>
#include <iostream>

#include <boost/algorithm/string/predicate.hpp>

ItemFactory::ItemFactory()
{
	// TODO Auto-generated constructor stub
}

ItemFactory::~ItemFactory()
{
	// TODO Auto-generated destructor stub
}

/**
 * Where lua_name is prefixed by weapon::lua_name, etc.
 * obj is the MapObj, if the item is created from a MapObj, e.g., from pick up command.
 * Most helpers ignore it, but it can be used by some MiscItems.
 */

Item* ItemFactory::create(std::string lua_name, MapObj* obj)
{
	if (lua_name.length() == 0)
		throw std::runtime_error("create(): Cannot create Lua item from empty lua_name.");

	std::vector<std::string> lname_split = Util::splitString(lua_name, "::");

	lua_State* lua_state = luaL_newstate();
	luaL_openlibs(lua_state);
	World::Instance().init_lua_arrays(lua_state);

	if (lname_split.size() > 0) {
		if (lname_split[0] == "weapons")
			return WeaponHelper::createFromLua(lname_split[1], lua_state);
		else if (lname_split[0] == "shields")
			return ShieldHelper::createFromLua(lname_split[1], lua_state);
		else if (lname_split[0] == "armour")
			return ArmourHelper::createFromLua(lname_split[1], lua_state);
		else if (lname_split[0] == "edibles")
			return EdiblesHelper::createFromLua(lname_split[1], lua_state);
		else if (lname_split[0] == "potions")
			return PotionsHelper::createFromLua(lname_split[1], lua_state);
		else if (lname_split[0] == "services")
			return ServicesHelper::createFromLua(lname_split[1], lua_state);
		else if (lname_split[0] == "miscitems")
			return MiscHelper::createFromLua(lname_split[1], lua_state, obj);
		else
			throw std::runtime_error("create(): Don't know how to create item with lua_name '" + lua_name + "'.");
	}
	else {
		lua_close(lua_state);
		throw std::runtime_error("create(): Cannot extract Lua name from '" + lua_name + "'.");
	}

	lua_close(lua_state);
}

/**
 * Like create(), except that it doesn't need a prefix and returns NULL if item cannot be created.
 */

Item* ItemFactory::create_plain_name(std::string item_name)
{
	lua_State* lua_state = luaL_newstate();
	luaL_openlibs(lua_state);
	World::Instance().init_lua_arrays(lua_state);

	if (WeaponHelper::existsInLua(item_name, lua_state)) {
		auto ret = WeaponHelper::createFromLua(item_name, lua_state);
		lua_close(lua_state);
		return ret;
	}
	else if (ShieldHelper::existsInLua(item_name, lua_state)) {
		auto ret = ShieldHelper::createFromLua(item_name, lua_state);
		lua_close(lua_state);
		return ret;
	}
	else if (ArmourHelper::existsInLua(item_name, lua_state)) {
		auto ret = ArmourHelper::createFromLua(item_name, lua_state);
		lua_close(lua_state);
		return ret;
	}
	else if (EdiblesHelper::existsInLua(item_name, lua_state)) {
		auto ret = EdiblesHelper::createFromLua(item_name, lua_state);
		lua_close(lua_state);
		return ret;
	}
	else if (ServicesHelper::existsInLua(item_name, lua_state)) {
		auto ret = ServicesHelper::createFromLua(item_name, lua_state);
		lua_close(lua_state);
		return ret;
	}
	else if (PotionsHelper::existsInLua(item_name, lua_state)) {
		auto ret = PotionsHelper::createFromLua(item_name, lua_state);
		lua_close(lua_state);
		return ret;
	}
	else if (MiscHelper::existsInLua(item_name, lua_state)) {
		auto ret = MiscHelper::createFromLua(item_name, lua_state);
		lua_close(lua_state);
		return ret;
	}
	else {
		// We didn't know which item to create. Should not happen.
		// TODO: Maybe throw exception?
		std::cerr << "ERROR: itemfactory.cc: Don't know what to create in create_plain_name(): " << item_name << "." << std::endl;
	}

	lua_close(lua_state);
	return NULL;
}
