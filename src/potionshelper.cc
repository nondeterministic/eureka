/*
 * shieldhelper.cc
 *
 *  Created on: Jun 22, 2014
 *      Author: baueran
 */

#include "potionshelper.hh"
#include "potion.hh"
#include "luawrapper.hh"
#include "party.hh"
#include "gamecontrol.hh"
#include "ztatswin.hh"
#include "edible.hh"
#include "edibleshelper.hh"

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

void PotionsHelper::drink(Potion* potion, lua_State* lua_state)
{
	Party& party     = Party::Instance();
	GameControl& gc  = GameControl::Instance();
	ZtatsWin& zwin   = ZtatsWin::Instance();

	// First compute the effects the potion has in terms of it being also a normal edible item...
	EdiblesHelper edibles_helper;
	edibles_helper.eat((Edible*)potion);

	// Push function name on stack
	lua_getglobal(lua_state, "Potions");          // Push "Potions" on stack
    lua_pushstring(lua_state, "healing potion");  // Push item_id on stack, e.g., "healing potion"
    lua_gettable(lua_state, -2);                  // Remove item_id from stack and put Potions[item_id] in its place

	lua_getfield(lua_state, -1, "effect");
	if (lua_pcall(lua_state, 0, 0, 0) != 0) {
		std::cerr << "ERROR: potionshelper.cc: Lua: '" << lua_tostring(lua_state, -1) << "'." << std::endl;
	}

	std::cout << "DONE DRINKING.\n";
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
