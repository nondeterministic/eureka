/*
 * weaponhelper.cc
 *
 *  Created on: Jun 22, 2014
 *      Author: baueran
 */

#include <memory>
#include <iostream>

#include <lua.h>
#include <lualib.h>

#include "weaponhelper.hh"
#include "weapon.hh"
#include "luawrapper.hh"

WeaponHelper::WeaponHelper()
{
	// TODO Auto-generated constructor stub
}

WeaponHelper::~WeaponHelper()
{
	// TODO Auto-generated destructor stub
}

/// Create a weapon from the name inside Lua's Weapons array.
/// For example, if Weapons has an entry "axe", then a new
/// axe weapon will be created and a pointer to it returned.

Weapon* WeaponHelper::createFromLua(std::string array_name, lua_State* lua_state)
{
	LuaWrapper lua(lua_state);
	Weapon *w = new Weapon();

	try {
		w->name(lua.get_item_prop<std::string>("Weapons", array_name, "name"));
		w->plural_name(lua.get_item_prop<std::string>("Weapons", array_name, "plural_name"));
		w->hands((int)(lua.get_item_prop<double>("Weapons", array_name, "hands")));
		w->range((int)(lua.get_item_prop<double>("Weapons", array_name, "range")));
		w->dmg_min((int)(lua.get_item_prop<double>("Weapons", array_name, "damage_min")));
		w->dmg_max((int)(lua.get_item_prop<double>("Weapons", array_name, "damage_max")));
		w->dmg_bonus((int)(lua.get_item_prop<double>("Weapons", array_name, "damage_bonus")));
		w->icon = (int)(lua.get_item_prop<double>("Weapons", array_name, "icon"));
		w->weight((int)(lua.get_item_prop<double>("Weapons", array_name, "weight")));
		w->gold((int)(lua.get_item_prop<double>("Weapons", array_name, "gold")));
		w->light_radius((int)(lua.get_item_prop<double>("Weapons", array_name, "light_radius")));
		w->destroy_after((int)(lua.get_item_prop<double>("Weapons", array_name, "destroy_after")));
	} catch (...) {
		throw;
	}

	return w;
}

/// Returns true if the Lua array has an entry named item_name, false otherwise.

bool WeaponHelper::existsInLua(std::string item_name, lua_State* lua_state)
{
	LuaWrapper lua(lua_state);
	return lua.hasEntry("Weapons", item_name);
}
