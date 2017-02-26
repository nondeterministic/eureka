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

std::string PotionsHelper::_glob_array_name = "Potions";

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
	LuaWrapper lua(lua_state);
	Potion *potion = new Potion();

	potion->name(lua.get_item_prop<std::string>(_glob_array_name, array_name, "name"));
	potion->plural_name(lua.get_item_prop<std::string>(_glob_array_name, array_name, "plural_name"));

	potion->food_up = (int)(lua.get_item_prop<double>(_glob_array_name, array_name, "food_up"));
	potion->healing_power = stringToEmphasis.at(boost::to_upper_copy<std::string>(lua.get_item_prop<std::string>(_glob_array_name, array_name, "healing_power")));
	potion->poison_healing_power = stringToEmphasis.at(boost::to_upper_copy<std::string>(lua.get_item_prop<std::string>(_glob_array_name, array_name, "poison_healing_power")));
	potion->poisonous = stringToEmphasis.at(boost::to_upper_copy<std::string>(lua.get_item_prop<std::string>(_glob_array_name, array_name, "poisonous")));
	potion->is_magic_herb = lua.get_item_prop<bool>(_glob_array_name, array_name, "magic_herb");
	potion->intoxicating = stringToEmphasis.at(boost::to_upper_copy<std::string>(lua.get_item_prop<std::string>(_glob_array_name, array_name, "intoxicating_effect")));

	potion->weight((int)(lua.get_item_prop<double>(_glob_array_name, array_name, "weight")));
	potion->icon = (int)(lua.get_item_prop<double>(_glob_array_name, array_name, "icon"));
	potion->gold((int)(lua.get_item_prop<double>(_glob_array_name, array_name, "gold")));

	potion->set_ingredient_names(lua.get_strings_from_subtable(_glob_array_name, array_name, "ingredients"));

	return potion;
}

/// Returns true if the Lua array has an entry named item_name, false otherwise.

bool PotionsHelper::existsInLua(std::string item_name, lua_State* lua_state)
{
	LuaWrapper lua(lua_state);
	return lua.hasEntry(_glob_array_name, item_name);
}

/**
 * Assumes that init_lua_arrays was called prior! Then, it will return an array of all Potions, that have been initialised in Lua-land.
 * Do not forget to free memory for the returned vector, if you no longer need it!  Those potions are on the heap!
 */

std::vector<Potion*> PotionsHelper::get_loaded_lua_potions(lua_State* L)
{
	std::vector<Potion*> potions;

	LuaWrapper lua(L);
	std::vector<std::string> potion_names = lua.get_itemids_from_itemarray(_glob_array_name);

	for (std::string potion_name: potion_names) {
		Potion* potion = PotionsHelper::createFromLua(potion_name, L);
		potions.push_back(potion);
	}

	return potions;
}
