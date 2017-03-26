/*
 * shieldhelper.cc
 *
 *  Created on: Jun 22, 2014
 *      Author: baueran
 */

#include "edibleshelper.hh"
#include "edible.hh"
#include "luawrapper.hh"
#include "gamecontrol.hh"
#include "ztatswin.hh"
#include "party.hh"

#include <string>
#include <iostream>

#include <lua.h>
#include <lualib.h>

#include <boost/algorithm/string.hpp>

EdiblesHelper::EdiblesHelper()
{
/*	globArrayName = "Shields";*/
}

EdiblesHelper::~EdiblesHelper()
{
	// TODO Auto-generated destructor stub
}

void EdiblesHelper::eat(Edible* item)
{
	Party& party     = Party::Instance();
	GameControl& gc  = GameControl::Instance();
	ZtatsWin& zwin   = ZtatsWin::Instance();
	EventManager& em = EventManager::Instance();

	// Food up
	if (item->food_up > 0) {
		Party::Instance().set_food(Party::Instance().food() + item->food_up);
		gc.draw_status(); gc.printcon("That was delicious. (PRESS SPACE BAR)"); em.get_key(" ");
	}

	// Intoxication
	int intoxicating_rounds = 0;
	switch (item->intoxicating) {
	case VERY_LITTLE:
		intoxicating_rounds = gc.random(0, 10);
		break;
	case SOME:
		intoxicating_rounds = gc.random(10, 20);
		break;
	case STRONG:
		intoxicating_rounds = gc.random(20, 30);
		break;
	case VERY_STRONG:
		intoxicating_rounds = gc.random(30, 40);
		break;
	default:
		break;
	}
	Party::Instance().rounds_intoxicated = Party::Instance().rounds_intoxicated + intoxicating_rounds;
	if (intoxicating_rounds > 0) {
		gc.draw_status();
		gc.printcon("It seems that " + item->name() + " has an intoxicating effect... (PRESS SPACE BAR)");
		em.get_key(" ");
	}

	// Getting poisoned
	for (int i = 0; i < Party::Instance().party_size(); i++) {
		PlayerCharacter* pl = Party::Instance().get_player(i);
		bool poisoned = false;

		if (pl->condition() != DEAD) {
			switch (item->poisonous) {
			case VERY_LITTLE:
				poisoned = gc.random(0, 10) >= 9;
				break;
			case SOME:
				poisoned = gc.random(0, 10) >= 7;
				break;
			case STRONG:
				poisoned = gc.random(0, 10) >= 5;
				break;
			case VERY_STRONG:
				poisoned = gc.random(0, 10) >= 3;
				break;
			default:
				break;
			}
			if (poisoned) {
				pl->set_condition(POISONED);
				gc.draw_status();
				gc.printcon(pl->name() + " is starting to feel quite sick... (PRESS SPACE BAR)");
				em.get_key(" ");
			}
		}
	}

	// Poison healing
	for (int i = 0; i < Party::Instance().party_size(); i++) {
		PlayerCharacter* pl = Party::Instance().get_player(i);
		bool phealed = false;

		if (pl->condition() == POISONED) {
			switch (item->poison_healing_power) {
			case VERY_LITTLE:
				phealed = gc.random(0, 10) >= 9;
				break;
			case SOME:
				phealed = gc.random(0, 10) >= 7;
				break;
			case STRONG:
				phealed = gc.random(0, 10) >= 5;
				break;
			case VERY_STRONG:
				phealed = gc.random(0, 10) >= 3;
				break;
			default:
				break;
			}

			if (phealed) {
				pl->set_condition(GOOD);
				gc.draw_status();
				gc.printcon(pl->name() + " feels less sick suddenly... (PRESS SPACE BAR)");
				em.get_key(" ");
			}
		}
	}

	{ // Normal healing
		int healed = 0;

		switch (item->healing_power) {
		case VERY_LITTLE:
			healed = gc.random(1, 5);
			break;
		case SOME:
			healed = gc.random(4, 10);
			break;
		case STRONG:
			healed = gc.random(10, 20);
			break;
		case VERY_STRONG:
			healed = gc.random(20, 30);
			break;
		default:
			break;
		}

		for (int i = 0; i < Party::Instance().party_size(); i++) {
			PlayerCharacter* pl = Party::Instance().get_player(i);
			if (healed) {
				if (pl->hp() < pl->hpm()) {
					pl->set_hp(min(pl->hpm(), pl->hp() + healed));

					// TODO: Update party view to signal healed players
					gc.draw_status();
					gc.printcon(pl->name() + " feels reinvigorated... (PRESS SPACE BAR)");
					em.get_key(" ");
				}
			}
		}
	}
}

/// See ShieldHelper.

Edible* EdiblesHelper::createFromLua(std::string array_name, lua_State* lua_state)
{
	std::string globArrayName = "Edibles";
	LuaWrapper lua(lua_state);
	Edible *w = new Edible();

	w->name(lua.get_item_prop<std::string>(globArrayName, array_name, "name"));
	w->plural_name(lua.get_item_prop<std::string>(globArrayName, array_name, "plural_name"));

	w->food_up = (int)(lua.get_item_prop<double>(globArrayName, array_name, "food_up"));
	w->healing_power = stringToEmphasis.at(boost::to_upper_copy<std::string>(lua.get_item_prop<std::string>(globArrayName, array_name, "healing_power")));
	w->poison_healing_power = stringToEmphasis.at(boost::to_upper_copy<std::string>(lua.get_item_prop<std::string>(globArrayName, array_name, "poison_healing_power")));
	w->poisonous = stringToEmphasis.at(boost::to_upper_copy<std::string>(lua.get_item_prop<std::string>(globArrayName, array_name, "poisonous")));
	w->is_magic_herb = lua.get_item_prop<bool>(globArrayName, array_name, "magic_herb");
	w->intoxicating = stringToEmphasis.at(boost::to_upper_copy<std::string>(lua.get_item_prop<std::string>(globArrayName, array_name, "intoxicating_effect")));

	w->weight((int)(lua.get_item_prop<double>(globArrayName, array_name, "weight")));
	w->icon = (int)(lua.get_item_prop<double>(globArrayName, array_name, "icon"));
	w->gold((int)(lua.get_item_prop<double>(globArrayName, array_name, "gold")));

	return w;
}

/// Returns true if the Lua array has an entry named item_name, false otherwise.

bool EdiblesHelper::existsInLua(std::string item_name, lua_State* lua_state)
{
	LuaWrapper lua(lua_state);
	return lua.hasEntry("Edibles", item_name);
}
