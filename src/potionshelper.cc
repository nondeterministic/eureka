/*
 * shieldhelper.cc
 *
 *  Created on: Jun 22, 2014
 *      Author: baueran
 */

#include "potionshelper.hh"
#include "potion.hh"
#include "luawrapper.hh"

#include <string>
#include <iostream>

#include <lua.h>
#include <lualib.h>

#include <boost/algorithm/string.hpp>

PotionsHelper::PotionsHelper()
{
}

PotionsHelper::~PotionsHelper()
{
	// TODO Auto-generated destructor stub
}

/// See ShieldHelper.

Potion* PotionsHelper::createFromLua(std::string array_name, lua_State* lua_state)
{
	std::string globArrayName = "Potions";
	LuaWrapper lua(lua_state);
	Potion *potion = new Potion();

	potion->name(lua.get_item_prop<std::string>(globArrayName, array_name, "name"));
	potion->plural_name(lua.get_item_prop<std::string>(globArrayName, array_name, "plural_name"));

	potion->food_up = (int)(lua.get_item_prop<double>(globArrayName, array_name, "food_up"));
	potion->healing_power = stringToEmphasis.at(boost::to_upper_copy<std::string>(lua.get_item_prop<std::string>(globArrayName, array_name, "healing_power")));
	potion->poison_healing_power = stringToEmphasis.at(boost::to_upper_copy<std::string>(lua.get_item_prop<std::string>(globArrayName, array_name, "poison_healing_power")));
	potion->poisonous = stringToEmphasis.at(boost::to_upper_copy<std::string>(lua.get_item_prop<std::string>(globArrayName, array_name, "poisonous")));
	potion->is_magic_herb = lua.get_item_prop<bool>(globArrayName, array_name, "magic_herb");
	potion->intoxicating = stringToEmphasis.at(boost::to_upper_copy<std::string>(lua.get_item_prop<std::string>(globArrayName, array_name, "intoxicating_effect")));

	potion->weight((int)(lua.get_item_prop<double>(globArrayName, array_name, "weight")));
	potion->icon = (int)(lua.get_item_prop<double>(globArrayName, array_name, "icon"));
	potion->gold((int)(lua.get_item_prop<double>(globArrayName, array_name, "gold")));

	potion->set_ingredient_names(lua.get_strings_from_subtable(globArrayName, array_name, "ingredients"));

	return potion;
}

/// Returns true if the Lua array has an entry named item_name, false otherwise.

bool PotionsHelper::existsInLua(std::string item_name, lua_State* lua_state)
{
	LuaWrapper lua(lua_state);
	return lua.hasEntry("Potions", item_name);
}
