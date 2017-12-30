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

#include <iostream>
#include <utility>
#include <string>
#include <sstream>
#include <type_traits>
#include <cstdio>

#include <boost/unordered_set.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/algorithm/string.hpp>

#include "config.h"
#include "weapon.hh"
#include "shield.hh"
#include "eureka.hh"
#include "gamecontrol.hh"
#include "creature.hh"
#include "combat.hh"
#include "util.hh"
#include "playercharacter.hh"
#include "party.hh"
#include "console.hh"
#include "soundsample.hh"
#include "gamerules.hh"
#include "ztatswin.hh"
#include "itemfactory.hh"
#include "weaponhelper.hh"
#include "shieldhelper.hh"
#include "serviceshelper.hh"
#include "race.hh"
#include "luawrapper.hh"
#include "gold.hh"
#include "miniwin.hh"
#include "gameeventhandler.hh"
#include "eventchangeicon.hh"
#include "eventplaysound.hh"
#include "world.hh"
#include "ztatswincontentprovider.hh"

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lualib.h>
#include <lauxlib.h>
}

using namespace std;

Combat* combat;

// ///////////////////////////////////////////////////
// "Private" helper functions.
// ///////////////////////////////////////////////////

std::string get_string_from_table(lua_State* L, const std::string& str)
{
  lua_pushstring(L, str.c_str());
  lua_gettable(L, -2);
  std::string result = lua_tostring(L, -1);
  lua_pop(L,1);

  return result;
}

int get_int_from_table(lua_State* L, const std::string& str)
{
  lua_pushstring(L, str.c_str());
  lua_gettable(L, -2);
  lua_Number result = lua_tonumber(L, -1);
  lua_pop(L,1);

  return (int)result;
}

// ///////////////////////////////////////////////////
// "Public" functions.
// ///////////////////////////////////////////////////

// Assumption here is that l_pushweapon is called from Lua with a
// Weapon table as argument.  Argument L will refer to the table that
// Lua's call to simpl_pushfunction will contain.

/*
int l_pushweapon(lua_State* L)
{
  Weapon weap;

  weap.name(get_string_from_table(L, "name"));
  weap.plural_name(get_string_from_table(L, "plural_name"));
  weap.hands(get_int_from_table(L, "hands"));
  weap.dmg_min(get_int_from_table(L, "damage_min"));
  weap.dmg_max(get_int_from_table(L, "damage_max"));
  weap.dmg_bonus(get_int_from_table(L, "damage_bonus"));
  weap.range(get_int_from_table(L, "range"));
  weap.weight(get_int_from_table(L, "weight"));
  weap.icon = get_int_from_table(L, "icon");

  weapons_map.insert(std::make_pair(weap.name(), weap));
  return 0;
}

int l_pushshield(lua_State* L)
{
  Shield shield;

  shield.name(get_string_from_table(L, "name"));
  shield.plural_name(get_string_from_table(L, "plural_name"));
  shield.protection(get_int_from_table(L, "protection"));
  shield.weight(get_int_from_table(L, "weight"));
  shield.icon = get_int_from_table(L, "icon");

  shields_map.insert(std::make_pair(shield.name(), shield));
  return 0;
}
*/

int l_printcon(lua_State* L)
{
	// First argument MUST be a string, so check it
	if (!lua_isstring(L, 1)) {
		std::cerr << "ERROR: luaapi.cc: l_printcon needs to be called with a string argument.\n";
		exit(EXIT_FAILURE);
	}

	// Second argument is optional, but must be bool
	if (lua_gettop(L) == 2 && !lua_isboolean(L, 2)) {
		std::cerr << "ERROR: luaapi.cc: l_printcon can optionally take as second argument a bool, but only a bool.\n";
		exit(EXIT_FAILURE);
	}

	if (lua_gettop(L) == 1)
		GameControl::Instance().printcon(lua_tostring(L, 1));
	else if (lua_gettop(L) == 2)
		GameControl::Instance().printcon(lua_tostring(L, 1), lua_toboolean(L, 2));
	else {
		std::cerr << "ERROR: luaapi.cc: wrong number of arguments to l_printcon.\n";
		exit(EXIT_FAILURE);
	}

	return 0;
}

/**
 * Prompt user input on console, finished by pressing return key.
 * Returns user input as string.
 */

int l_getcon(lua_State* L)
{
	std::string input = Console::Instance().gets();
    lua_pushstring(L, input.c_str());
    return 1;
}

/**
 * Prompt single-key input from user.
 * Essentially, a wrapper around EventManager::get_key.
 *
 * When called, the Lua stack will contain a C-string as argument (see EM::get_key()),
 * and return a C-string, i.e., the key pressed.
 */

int l_getkey(lua_State* L)
{
	std::stringstream ss;
	std::string allowed_keys = (std::string)(lua_tostring(L, 1));
	ss << EventManager::Instance().get_key(allowed_keys.c_str());
	lua_pushstring(L, ss.str().c_str());
	return 1;
}

// On the Lua stack, at time of calling, are the 4 arguments for EventChangeIcon.
// Returns to Lua -1 on error, 1 in case of success.

int l_change_icon(lua_State* L)
{
	GameEventHandler gh;
	std::shared_ptr<Map> the_map(GameControl::Instance().get_map());
	std::shared_ptr<EventChangeIcon> new_ev(new EventChangeIcon());

	if (!lua_isnumber(L, 1)) {
		std::cerr << "Error: Lua: l_change_icon() wrong argument 1.\n";
		exit(EXIT_FAILURE);
	}
	new_ev->x = lua_tonumber(L, 1);

	if (!lua_isnumber(L, 2)) {
		std::cerr << "Error: Lua: l_change_icon() wrong argument 2.\n";
		exit(EXIT_FAILURE);
	}
	new_ev->y = lua_tonumber(L, 2);

	if (!lua_isnumber(L, 3)) {
		std::cerr << "Error: Lua: l_change_icon() wrong argument 3.\n";
		exit(EXIT_FAILURE);
	}
	new_ev->icon_now = lua_tonumber(L, 3);

	if (!lua_isnumber(L, 4)) {
		std::cerr << "Error: Lua: l_change_icon() wrong argument 4.\n";
		exit(EXIT_FAILURE);
	}
	new_ev->icon_new = lua_tonumber(L, 4);

	if (gh.handle_event_change_icon(new_ev, the_map))
		lua_pushnumber(L, 1);  // All OK
	else
		lua_pushnumber(L, -1); // Error happened

	return 1;
}

int l_ztatswin_update(lua_State* L)
{
	ZtatsWin::Instance().update_player_list();
	return 0;
}

int l_ztatswin_save(lua_State* L)
{
	ZtatsWin& zwin = ZtatsWin::Instance();
	zwin.save_texture();
	zwin.clear();
	return 0;
}

int l_ztatswin_restore(lua_State* L)
{
	ZtatsWin& zwin = ZtatsWin::Instance();
	zwin.display_last();
	zwin.blit();
	return 0;
}

// Returns number of selected player in party,
// -1, if action was somehow aborted by the user.

int l_choose_player(lua_State* L)
{
	ZtatsWin& zwin = ZtatsWin::Instance();

	zwin.update_player_list();
	int player = zwin.select_player();
	lua_pushnumber(L, player);

	return 1;
}

// Returns number of a group of monsters or -1 if user cancelled.

int l_choose_monster(lua_State* L)
{
	lua_pushnumber(L, combat->select_enemy());
	return 1;
}

/**
 * When called, on the Lua stack has to be a table of either weapons or edibles, etc.
 * This table will then be inserted into the ztats window display, and the user can select an item for purchase.
 *
 * Returns the name of the selected item, which can be turned into an actual item using, e.g., the methods in ItemFactory.
 * If no item was selected, "" is returned.
 */

int l_ztatswin_shopinteraction(lua_State* L)
{
	ZtatsWin& zwin = ZtatsWin::Instance();
	int left_border = 9;

	 // A formatted item name with its price in gold, and a plain and simple item name for easy identification without much string decomposition non-sense...
	std::vector<std::pair<StringAlignmentTuple, std::string>> content_page;
	ZtatsWinContentSelectionProvider<std::string> content_selection_provider; // Will return plain item name

	lua_pushnil(L);

	while (lua_next(L, -2) != 0) {
    	std::string name;
    	std::string gold;

    	lua_pushnil(L);

		while (lua_next(L, -2) != 0) {
			// Skip boolean table entries as we only want to extract name and gold anyway to display in the ztats window when shopping...
			// (Boolean, unlike an integer, cannot be implicitly converted to a string it seems, and a crash is the consequence if we try.  So let's not.)
			if (lua_type(L, -1) != LUA_TBOOLEAN) {
				std::string val = lua_tostring(L, -1);
				std::string key = lua_tostring(L, -2);         // Key is always a string

				if (key == "name")
					name = val;
				else if (key == "gold")
					gold = val;
			}

			lua_pop(L, 1);
    	}

    	std::stringstream ss;
    	ss << "(" << gold << " Au)";
    	int str_len = ss.str().length();
    	for (int i = str_len; i < left_border; i++)
    		ss << " ";
    	ss << name;

		content_page.push_back(std::pair<StringAlignmentTuple, std::string>(StringAlignmentTuple(ss.str(), Alignment::LEFTALIGN), name));
    	lua_pop(L, 1);
	}

	lua_pop(L, 1);

	content_selection_provider.add_content_page(content_page);
	std::vector<std::string> selected_items_names = zwin.execute(&content_selection_provider, SelectionMode::SingleItem);

	if (selected_items_names.size() > 0)
    	lua_pushstring(L, selected_items_names[0].c_str());
	else
		lua_pushstring(L, "");

    return 1;
}

/**
 * Displays inventory items on ztats window for the player to choose one item to sell.
 * If there is more of the chosen item, this method will also prompt the user to enter
 * how many of that item she wants to sell.
 *
 * Method then removes the items from the inventory and gives the party the gold.
 */

int l_ztatswin_sell_arms_from_inventory(lua_State* L)
{
	int price_reduction = (lua_tonumber(L, 1));

	Party& party = Party::Instance();
	ZtatsWin& zwin = ZtatsWin::Instance();

	if (party.inventory()->size() == 0) {
		GameControl::Instance().printcon("Looks like you have nothing to offer.");
		return 0;
	}

	std::shared_ptr<ZtatsWinContentSelectionProvider<Item*>> content_selection_provider = party.inventory()->create_content_selection_provider(InventoryType::Wearables);
	std::vector<Item*> selected_items = zwin.execute(content_selection_provider.get(), SelectionMode::SingleItem);
	// Return item name or "" if none was selected
    if (selected_items.size() > 0) {
    	Item* selected_item = selected_items[0];
		unsigned selling_how_many = 1;

		stringstream ss;
		ss << "How many (0 - " << party.inventory()->how_many_of(selected_item->name(), selected_item->description()) << ")?";
		GameControl::Instance().printcon(ss.str() + " ");

		try {
			std::string input;
			input = Console::Instance().gets();
			selling_how_many = std::stoi(input);
			GameControl::Instance().printcon(input + " ");
		}
		catch (const std::invalid_argument& e) {
			// User didn't enter a valid number
			GameControl::Instance().printcon("Huh? I guess, some other time then.");
			return 0;
		}

		if (selling_how_many > party.inventory()->how_many_of(selected_item->name(), selected_item->description())) {
			GameControl::Instance().printcon("Sadly, you don't have that many.");
			return 0;
		}
		else if (selling_how_many == 0) {
			GameControl::Instance().printcon("Changed your mind then, eh?");
			return 0;
		}

    	// Simple test to see, if the item can be sold at all.
    	// Not nice to allocate and deallocate for that purpose, but not so bad either...
    	Item* tmp_item = NULL;
    	try {
    		tmp_item = ItemFactory::create_plain_name(selected_item->name());
    	}
		catch (std::exception const& e) {
		    std::cerr << "EXCEPTION: luaapi.cc: " << e.what() << "\n";
		    return 0;
		}

    	if (tmp_item != NULL) {
    		// Buying price is item price - price_reduction %
    		int item_price = tmp_item->gold() - (int)((float)tmp_item->gold() / 100.0 * (float)price_reduction);

			GameControl::Instance().printcon("I can offer you " + std::to_string(item_price) + " per piece. Agree (y/n)?");
			char agree = EventManager::Instance().get_key("yn");

			if (agree == 'y') {
				// Now add items to inventory...
				for (unsigned i = 0; i < selling_how_many; i++) {
					party.inventory()->remove(tmp_item->name(), "");  // Empty item descr. should be OK as only standard items can be sold.
					party.set_gold(party.gold() + item_price);
				}
				GameControl::Instance().printcon("'t was a pleasure doing business with you.");
			}
			else
				GameControl::Instance().printcon("Too bad. That's all I can offer you.");

    		delete tmp_item;
        	GameControl::Instance().redraw_graphics_status(false);
    	}
    	else
    		std::cerr << "WARNING: Selling of item " << selected_item->name() << " failed. (This could be an error, but not necessarily so.)\n";
    }

    return 0;
}

/**
 * At call time, the name of the item is on the Lua stack
 * and will then be added to the party's inventory.
 * No return value.
 * (See also l_buyitem().)
 */

int l_additemtoinv(lua_State* L)
{
	std::string item_name = (std::string)(lua_tostring(L, 1));
	Item* item = NULL;
	try {
		item = ItemFactory::create_plain_name(item_name);
	}
	catch (std::exception const& e) {
	    std::cerr << "EXCEPTION: gamecontrol.cc: " << e.what() << "\n";
	    return 0;
	}

	if (item == NULL) {
		std::cerr << "WARNING: luaapi.cc::l_additemtoinv: cannot create item " << item_name << ".\n";
		return 0;
	}

	Party::Instance().inventory()->add(item);
	GameControl::Instance().redraw_graphics_status(false);
	return 0;
}

/**
 * Like l_additemtoinv, except that it checks if
 * a) the inventory has room for it
 * b) the party has enough gold
 *
 * If a) fails, -1 is returned. If b) fails, -2 is returned.
 * If another error occurs, -3 is returned.
 *
 * On success 0 is returned.
 */

int l_buyitem(lua_State* L)
{
	std::string item_name = (std::string)(lua_tostring(L, 1));
	Item* item = NULL;
	try {
		item = ItemFactory::create_plain_name(item_name);
	}
	catch (std::exception const& e) {
	    std::cerr << "EXCEPTION: luaapi.cc: " << e.what() << "\n";
	}

	int how_many = 0;
	GameControl::Instance().printcon("How many?");
	try {
		std::string input = Console::Instance().gets();
		how_many = std::stoi(input);
		GameControl::Instance().printcon(input + " ");
	}
	catch (const std::invalid_argument& e) {
		// User didn't enter a valid number
		GameControl::Instance().printcon("Huh? I guess, some other time then.");
		if (item != NULL) {
			delete item;
			item = NULL;
		}
    	lua_pushnumber(L, -3);
	}

	if (item != NULL) {
		if (true) { // TODO: Replace and check weight restrictions...
			if (Party::Instance().gold() >= item->gold() * how_many) {  // Check if sufficient gold
				Party::Instance().set_gold(Party::Instance().gold() - item->gold() * how_many);
				// Add the bought item, which we also checked to see if the chosen name was correct, etc.
				std::cout << "ADDING " << item->description() << item->name() << " TO INV.\n";
				Party::Instance().inventory()->add(item);
				// Now create and add the rest of the bought items...
				for (int i = 0; i < how_many - 1; i++)
					Party::Instance().inventory()->add(ItemFactory::create_plain_name(item_name));
				GameControl::Instance().redraw_graphics_status(false);
		    	lua_pushnumber(L, 0);
			}
			else {
				delete item;
		    	lua_pushnumber(L, -2);
			}
		}
		else {
			delete item;
	    	lua_pushnumber(L, -1);
		}

	}
	else
    	lua_pushnumber(L, -3); // Other error occurred

	return 1;
}

/// Level up all players that can be leveled up according to their respective EPs.
/// Return the number of players leveled-up, 0 if none was leveled up, a positive integer otherwise.

int l_level_up(lua_State* L)
{
	int leveled_up = 0;

	for (int i = 0; i < Party::Instance().party_size(); i++) {
		PlayerCharacter* pl = Party::Instance().get_player(i);

		if (pl->condition() != DEAD && pl->hp() > 0 &&  // Cannot level up dead players
				pl->level() < pl->potential_level())
		{
			std::cout << "INFO: luaapi.cc: Leveled up player " << pl->name()  << " from " << pl->level() << " to " << pl->potential_level() << std::endl;
			pl->set_level_actively(pl->potential_level());
			GameControl::Instance().printcon(pl->name() + " now has experience level " + std::to_string(pl->level()) + ".");
			leveled_up++;
		}
	}

    ZtatsWin::Instance().update_player_list();

	lua_pushnumber(L, leveled_up);
	return 1;
}

/**
 * On success, 0 is returned.
 * -1 is returned if the party doesn't have enough money to afford the service.
 * 0 is returned on success.
 */

int l_buyservice(lua_State* L)
{
    std::string selected_service = lua_tostring(L, 2);
    int selected_player          = lua_tonumber(L, 1);

    std::cout << "SERVICE CHOSEN: " << selected_service << "\n";

    Service* service = NULL;
    try {
    	service = (Service*)(ItemFactory::create_plain_name(selected_service));
    }
    catch (std::exception const& e) {
	    std::cerr << "EXCEPTION: luaapi.cc: " << e.what() << "\n";
    }

    if (service != NULL) {
		if (service->gold() > Party::Instance().gold()) {
			GameControl::Instance().printcon("Sorry, it seems you cannot afford this.");
			lua_pushnumber(L, -1);
		}
		else {
			ServicesHelper::apply(service, selected_player);
			lua_pushnumber(L, 0);
		}

		ZtatsWin::Instance().update_player_list();

		delete service;
		return 1;
    }

    std::cerr << "ERROR: luaapi.cc: l_buyservice could not create service " << selected_service << "\n";
	lua_pushnumber(L, 0);
	return 1;
}

/**
  * Topmost on the Lua stack is a Lua-table-element of type "Service", when this
  * function is called, and below that a player number to perform this service to.
  * (I.e., it was called like this simpl_buyservice(player, item).)
  * If this isn't the case, bad things will happen...
  *
  * On success, 0 is returned.
  * -1 is returned if the party doesn't have enough money to afford the service.
  */

int l_buyservice_old(lua_State* L)
{
    int heal                = 0;
    bool heal_poison        = 0;
    bool resurrect          = false;
    std::string print_after = "";
    int gold                = 0;

    // //////////////////////////////////////////////////////////////////////////////////////
    // INFO:
    // In Lua we count down from the current stack pointer and up from the bottom.
    // I.e., -1 refers to one below the SP (= topmost element), and 1 refers to the oldest
    // element on the stack.
    // //////////////////////////////////////////////////////////////////////////////////////

    std::cout << "BUYSERVICE CALLED WITH " << lua_gettop(L) << " ARGUMENTS\n";

    // Push nil on top of stack and set Lua stack pointer to top.
    lua_pushnil(L);

	// The table begins two below the stack pointer...
    while (lua_next(L, -2) != 0) {
    	std::string key = lua_tostring(L, -2);

        std::cout << "LOOP...\n";

    	if (key == "heal")
    		heal = lua_tonumber(L, -1);
    	else if (key == "heal_poison")
    		heal_poison = lua_toboolean(L, -1);
    	else if (key == "resurrect")
    		resurrect = lua_toboolean(L, -1);
    	else if (key == "print_after")
    		print_after = lua_tostring(L, -1);
    	else if (key == "gold")
    		gold = lua_tonumber(L, -1);
    	else
    		std::cerr << "ERROR: luaapi.cc: Unknown service bought: " << key << "\n";

    	lua_pop(L, 1);
    }

    // The bottom-most (i.e., oldest) argument is the player number.
    int selected_player = lua_tonumber(L, 1);
    std::cout << "Selected PLAYER " << selected_player << "\n";

    // Now change party stats according to the values
    // TODO

	// Now print successful response of purchased service
	GameControl::Instance().printcon(print_after);

	lua_pushnumber(L, 0);

	return 1;
}

// Pass current datapath to Lua-land

int l_datapath(lua_State* L)
{
	boost::filesystem::path tmp_path = boost::filesystem::path((std::string)DATADIR);
	tmp_path = tmp_path / (std::string)PACKAGE_NAME / "data" / World::Instance().get_name();
	lua_pushstring(L, tmp_path.c_str());
	return 1;
}

int l_set_combat_ptr(lua_State* L)
{
  void *addr;
  sscanf(lua_tostring(L, 1), "%p", (void **)&addr);
  combat = ((Combat*)addr);

  return 0;
}

int l_bonus(lua_State* L)
{
  lua_pushnumber(L, GameRules::bonus((int)lua_tonumber(L, 1)));
  return 1;
}

int l_get_partysize(lua_State* L)
{
	lua_pushnumber(L, Party::Instance().party_size());
	return 1;
}

int l_get_level(lua_State* L)
{
  int player_number = lua_tonumber(L, 1);

  if (player_number < 0 || player_number >= Party::Instance().party_size()) {
	  std::cerr << "ERROR: luaapi.cc: Cannot determine level of player " << player_number << "\n";
	  lua_pushnumber(L, 0);
	  return 1;
  }

  PlayerCharacter* player = Party::Instance().get_player(player_number);
  if (player == NULL) {
	  std::cerr << "ERROR: luaapi.cc: Cannot determine level of player " << player_number << "\n";
	  lua_pushnumber(L, 0);
	  return 1;
  }

  lua_pushnumber(L, player->level());
  return 1;
}

int l_get_player_is_alive(lua_State* L)
{
	std::string player_name = (std::string)(lua_tostring(L, 1));
	PlayerCharacter* player = Party::Instance().get_player(player_name);
	lua_pushboolean(L, player->condition() != PlayerCondition::DEAD);
	return 1;
}

int l_get_player_ac(lua_State* L)
{
	std::string player_name = (std::string)(lua_tostring(L, 1));
	PlayerCharacter* player = Party::Instance().get_player(player_name);
	if (player == NULL) {
		std::cerr << "ERROR: luaapi.cc: no player named " << player_name << " found.\n";
		exit(EXIT_FAILURE);
	}
	lua_pushnumber(L, player->armour_class());
	return 1;
}

int l_player_set_condition(lua_State* L)
{
	std::string player_name = (std::string)(lua_tostring(L, 1));
	std::string condition = (std::string)(lua_tostring(L, 2));

	PlayerCharacter* player = Party::Instance().get_player(player_name);

	if (condition == "POISONED")
		player->set_condition(PlayerCondition::POISONED);
	else if (condition == "DEAD")
		player->set_condition(PlayerCondition::DEAD);
	else // if (condition == "GOOD")
		player->set_condition(PlayerCondition::GOOD);

	return 0;
}

int l_player_change_hp(lua_State* L)
{
	std::string player_name = (std::string)(lua_tostring(L, 1));
	PlayerCharacter* player = Party::Instance().get_player(player_name);
	int old_hp = player->hp();
	int change = lua_tonumber(L, 2);
	player->set_hp(max(0, player->hp() + change));

	if (player->hp() == 0) {
		player->set_condition(DEAD);

		if (old_hp != 0) // Player died NOW, not some time before...
			GameControl::Instance().printcon(player->name() + " has died.");
	}

	return 0;
}

int l_notify_party_hit(lua_State* L)
{
	static SoundSample sample;

	Console::Instance().alarm();
	sample.play_predef(HIT);

	return 0;
}

// Returns the names of at most n randomly chosen players from the
// front row of the party (by putting them on the Lua stack).
// If the party is smaller, it still returns n strings, some of which
// are empty.

int l_rand_player(lua_State* L)
{
  boost::unordered_set<PlayerCharacter*> players;
  int requested = (int)lua_tonumber(L, 1);
  int count = min(min(Party::Instance().party_size(), 3), requested);

  while ((int)players.size() < count) {
    int rnd = GameControl::Instance().random(0, min(Party::Instance().party_size() - 1, 2));
    PlayerCharacter* p = Party::Instance().get_player(rnd);
    if (p->condition() != DEAD)
    	players.insert(p);
  }

  // Prepare return values
  for (auto player = players.begin(); player != players.end(); player++)
    lua_pushstring(L, (*player)->name().c_str());

  // If more players than available were requested, "pad" the result with empty strings
  int diff = requested - players.size();
  for (int i = 0; i < diff; i++)
    lua_pushstring(L, "");

  return requested;
}

// Advances foes, if possible, and returns true on success, false otherwise.

int l_advance_foes(lua_State* L)
{
  lua_pushboolean(L, combat->advance_foes().size() > 0);
  return 1;
}

int l_flee_foe(lua_State* L)
{
  int foe = lua_tonumber(L, 1);
  combat->flee_foe(foe);
  return 0;
}

int l_random(lua_State* L)
{
  lua_pushnumber(L, GameControl::Instance().random(lua_tonumber(L, 1), lua_tonumber(L, 2)));
  return 1;
}

/**
 * Gets as argument a signed integer, which is then added to the party's gold.
 * I.e., remove or add party gold. Useful for donations, trading, or theft.
 *
 * @return false if -n >= party_gold, true otherwise.
 */

int l_modify_gold(lua_State* L)
{
	int new_gold   = lua_tonumber(L, 1);
	int party_gold = Party::Instance().gold();

	if (new_gold + party_gold >= 0) {
		Party::Instance().set_gold(party_gold + new_gold);
		GameControl::Instance().redraw_graphics_status(false);
		lua_pushboolean(L, true);
	}
	else
		lua_pushboolean(L, false);

	return 1;
}

int l_get_gold(lua_State* L)
{
	lua_pushnumber(L, Party::Instance().gold());
	return 1;
}

int l_party_size(lua_State* L)
{
	lua_pushnumber(L, Party::Instance().party_size());
	return 1;
}

// THIS METHOD IS NOT VISIBLE TO LUA.  IT IS USED INTERNALLY ONLY, e.g., by l_join().
// Basically extracts the c_values field from a city person or the like and creates an
// according PlayerCharacter.

std::shared_ptr<PlayerCharacter> create_character_values_from_lua(lua_State* L)
{
	std::shared_ptr<PlayerCharacter> player(new PlayerCharacter());

	lua_pushstring(L, "name");
	lua_gettable(L, -2);
	player->set_name(lua_tostring(L, -1)); // Extract result, which is now on the stack
	lua_pop(L,1);                          // Tidy up stack by getting rid of the extra we just put on

	lua_pushstring(L, "race");
	lua_gettable(L, -2);
	std::string race = lua_tostring(L, -1);
	if (race == "HUMAN")
		player->set_race(HUMAN);
	else if (race == "ELF")
		player->set_race(ELF);
	else if (race == "HALF_ELF")
		player->set_race(HALF_ELF);
	else if (race == "HOBBIT")
		player->set_race(HOBBIT);
	else // if (race == "DWARF")
		player->set_race(DWARF);
	lua_pop(L,1);

	lua_pushstring(L, "ep");
	lua_gettable(L, -2);
	player->inc_ep(lua_tonumber(L, -1));
	lua_pop(L,1);

	lua_pushstring(L, "hp");
	lua_gettable(L, -2);
	player->set_hp(lua_tonumber(L, -1));
	lua_pop(L,1);

	lua_pushstring(L, "hpm");
	lua_gettable(L, -2);
	player->set_hpm(lua_tonumber(L, -1));
	lua_pop(L,1);

	lua_pushstring(L, "sp");
	lua_gettable(L, -2);
	player->set_sp(lua_tonumber(L, -1));
	lua_pop(L,1);

	lua_pushstring(L, "spm");
	lua_gettable(L, -2);
	player->set_spm(lua_tonumber(L, -1));
	lua_pop(L,1);

	lua_pushstring(L, "str");
	lua_gettable(L, -2);
	player->set_str(lua_tonumber(L, -1));
	lua_pop(L,1);

	lua_pushstring(L, "luck");
	lua_gettable(L, -2);
	player->set_luck(lua_tonumber(L, -1));
	lua_pop(L,1);

	lua_pushstring(L, "dxt");
	lua_gettable(L, -2);
	player->set_dxt(lua_tonumber(L, -1));
	lua_pop(L,1);

	lua_pushstring(L, "wis");
	lua_gettable(L, -2);
	player->set_wis(lua_tonumber(L, -1));
	lua_pop(L,1);

	lua_pushstring(L, "charr");
	lua_gettable(L, -2);
	player->set_char(lua_tonumber(L, -1));
	lua_pop(L,1);

	lua_pushstring(L, "iq");
	lua_gettable(L, -2);
	player->set_iq(lua_tonumber(L, -1));
	lua_pop(L,1);

	lua_pushstring(L, "endd");
	lua_gettable(L, -2);
	player->set_end(lua_tonumber(L, -1));
	lua_pop(L,1);

	lua_pushstring(L, "sex");
	lua_gettable(L, -2);
	std::string sex_str = lua_tostring(L, -1);
	bool sex = false;
	if (boost::to_upper_copy(sex_str) == "MALE")
		sex = true;
	player->set_sex(sex);
	lua_pop(L,1);

	lua_pushstring(L, "profession");
	lua_gettable(L, -2);
	std::string prof_string = lua_tostring(L, -1);
	prof_string = boost::to_upper_copy(prof_string);
	player->set_profession(stringToProfession.at(prof_string));
	lua_pop(L,1);

	LuaWrapper lua(L);
	// Only if Lua-land returns non-empty weapon-descriptions, set one accordingly.
	// Otherwise monster has none.
	// (Default values should be NULL for weapon etc. anyway, indicating, None.)
	if (lua.call_fn<std::string>("get_weapon").length() > 0)
		player->set_weapon(WeaponHelper::createFromLua(lua.call_fn<std::string>("get_weapon"), L));
	if (lua.call_fn<std::string>("get_shield").length() > 0)
		player->set_shield(ShieldHelper::createFromLua(lua.call_fn<std::string>("get_shield"), L));

    return player;
}

// TODO:
// Accessing table like this:
// http://www.wellho.net/mouth/1845_Passing-a-table-from-Lua-into-C.html
//
// Returns true if join successful, false otherwise
//
// IMPORTANT: This sunction has the c_values table on the Lua stack when called!

int l_join(lua_State* L)
{
	if (Party::Instance().party_size() >= 6)
		lua_pushboolean(L, false);

	std::shared_ptr<PlayerCharacter> player = create_character_values_from_lua(L);

	// TODO: Nothing.  Delete this comment.  :-)

	Party::Instance().add_player(*(player.get()));
    ZtatsWin::Instance().update_player_list();

    lua_pushboolean(L, true);
    return 1;
}

int l_party_is_outdoors(lua_State* L)
{
	lua_pushboolean(L, GameControl::Instance().is_arena_outdoors());
	return 1;
}

// Item id is on the Lua stack at calling time.  Item will then be removed from the map,
// because either it died in battle, joined the party, etc.  Map is usually indoors as outdoors,
// we don't show detailed icon view.

int l_remove_from_current_map(lua_State* L)
{
	std::string id = lua_tostring(L, 1); // This is the id used inside the current map XML-file!
	GameControl::Instance().get_map()->rm_obj_by_id(id);

	return 0;
}

// Gets two arguments on Lua stack:
// 1. number of party member, 2. a number that is to be added to member's hp
// (if that number is negative, subtraction actually occurs, naturally).

int l_add_hp(lua_State* L)
{
	int player_no = lua_tonumber(L, 1);
	int value     = lua_tonumber(L, 2);

	PlayerCharacter* player = Party::Instance().get_player(player_no);
	int hp_old = player->hp();

	// We cannot up the HP of a dead player
	if (player->condition() == DEAD) {
	    GameControl::Instance().printcon("But " + player->name() + " is dead.");
		return 0;
	}

	if (value >= 0)
		player->set_hp(min(player->hpm(), player->hp() + value));
	else
		player->set_hp(max(0, player->hp() + value));

	if (player->hp() > hp_old)
		lua_pushboolean(L, true);
	else
		lua_pushboolean(L, false);

	return 1;
}

// Returns true if player referred to by number on Lua stack is dead.
// (Will crash, if that number is out of range for some reason.)

int l_is_dead(lua_State* L)
{
	int player_no = lua_tonumber(L, 1);
	lua_pushboolean(L, Party::Instance().get_player(player_no)->condition() == DEAD);
	return 1;
}

int l_make_guards(lua_State* L)
{
	if (!lua_isstring(L, 1)) {
		std::cerr << "ERROR: luaapi.cc: l_make_guards() wrong argument.\n";
		exit(EXIT_FAILURE);
	}

	string guard_mode = lua_tostring(L, 1);

	if (boost::to_upper_copy(guard_mode) == "HOSTILE")
	    GameControl::Instance().make_guards(HOSTILE);
	else if (boost::to_upper_copy(guard_mode) == "RIGHTEOUS")
	    GameControl::Instance().make_guards(RIGHTEOUS);
	else
	    GameControl::Instance().make_guards(NEUTRAL);

	return 0;
}

int l_play_sound(lua_State* L)
{
	GameEventHandler gh;
	std::string filename = lua_tostring(L, 1);
	int loop = 0; // Default for samples.
	int volume = SoundSample::sample_volume; // Default for samples.  For music, write new separate function!

	boost::algorithm::trim(filename);
	std::shared_ptr<EventPlaySound> new_ev(new EventPlaySound(filename, loop, volume));

	if (gh.handle_event_playsound(new_ev, NULL))
		lua_pushnumber(L, 1);  // All OK
	else
		lua_pushnumber(L, -1); // Error happened

	return 1;
}

// True, if party is currently in battle

int l_is_in_battle(lua_State* L)
{
	lua_pushboolean(L, Party::Instance().is_in_combat);
	return 1;
}

// On the stack is the number of the party member

int l_get_player_name(lua_State* L)
{
	int player_no = lua_tonumber(L, 1);
	lua_pushstring(L, Party::Instance().get_player(player_no)->name().c_str());
	return 1;
}

// On the stack is the number of the monster to be attacked.
// Returns the single name of the monster.
// PRECONDITION: ASSUMES THAT COMBAT PTR IS SET!!!

int l_get_single_monster_name(lua_State* L)
{
	int group = lua_tonumber(L, 1);
	std::string attacked_single_name = "";

	int j = 1;
	for (auto foe : *(combat->get_foes().count())) {
		if (group == j)
			attacked_single_name = foe.first;
		j++;
	}

	lua_pushstring(L, attacked_single_name.c_str());
	return 1;
}

int l_magic_attack(lua_State* L)
{
	static SoundSample sample;

	// Get Lua parameters; see attack spell script.
	int  targets             = lua_tonumber(L, 1);
	bool attack_whole_group  = lua_tonumber(L, 2) == 1? true : false;
	int  resistance          = lua_tonumber(L, 3);
	int  range               = lua_tonumber(L, 4);
	int  damage              = lua_tonumber(L, 5);
	int  spell_lasts         = lua_tonumber(L, 6);
    std::string caster       = lua_tostring(L, 7);

    if (caster.length() == 0) {
        std::cerr << "ERROR: luaapi.cc: caster not set. (This is serious. Could be a bug in the spell script.)\n";
        return 0;
    }

    PlayerCharacter* player = Party::Instance().get_player(caster);

	// Get the monster that is to be attacked this round
	Creature* opponent = NULL;
	int opponent_offset = 0;

	int j = 1;
	for (auto foe : *(combat->get_foes().count())) {
		if (j == targets) {
			int k = 0;
			for (auto _foe = combat->get_foes().begin(); _foe != combat->get_foes().end(); _foe++, k++) {
				if (foe.first == (*_foe)->name()) {
					opponent = _foe->get(); // Opponent now points to the monster to be attacked
					opponent_offset = k;
					break;
				}
			}
		}
		j++;
	}

	if (opponent == NULL) {
		std::cerr << "ERROR: luaapi.cc: opponent == null (No monster to attack.)\n";
		return 0;
	}

	if (opponent->distance() <= range) {
		stringstream ss;

		int temp_AC = 10; // TODO: Replace this with the actual AC of opponent!  This AC needs to be computed from weapons, dex, etc.

		if (GameControl::Instance().random(1, 20) > temp_AC) {
			LuaWrapper lua(L);

			if (opponent->hp() - damage > 0) {
				ss << player->name() << " casts a spell, causing the " << opponent->name() << " " << damage << " points of damage.";
				opponent->set_hp(opponent->hp() - damage);
				lua.push_fn_arg((double)(opponent->hp() - damage));
				lua.call_void_fn("set_hp");

				MiniWin::Instance().alarm();
				sample.play_predef(FOE_HIT);
			}
			else {
				ss << player->name() << " casts a spell, killing the " << opponent->name() << ".";
			    combat->get_foes().remove(opponent_offset);

				// Add experience points to player's balance
				player->inc_ep(lua.call_fn<double>("get_ep"));

				MiniWin::Instance().alarm();
				sample.play_predef(FOE_HIT);

				// Now add monster's items to bounty items to be collected
				// by party in case of battle victory.
				if (opponent->weapon() != NULL)
					combat->add_to_bounty(opponent->weapon());

				// Add monster's gold
				int gold_coins = lua.call_fn<double>("get_gold");
				for (int ii = 0; ii < gold_coins; ii++)
					combat->add_to_bounty(new Gold());
			}
		    GameControl::Instance().printcon(ss.str(), true);
			MiniWin::Instance().alarm();
		}
		else {
			ss << player->name() << " casts a spell, but misses.";
			GameControl::Instance().printcon(ss.str(), true);
		}
	}

	return 0;
}

// Get hp from n-th monster that attacks, where n is an integer on the Lua stack.
// Note it is not the n-th group of monsters, but really the n-th monster of Combat::foes array!
//
// NOTE: Make sure to set combat pointer properly from the Lua script, prior to calling this
// or the program will crash!!  (See above function to achieve this from Lua land.)
//int simpl_get_hp(lua_State* L)
//{
//	int foe_nr = lua_tonumber(L, 1);
//	Creature* c = combat->get_foes().get(foe_nr);
//	lua_pushnumber(L, c->hp());
//	return 1;
//}

// At time of calling, the Lua stack contains an integer that describes how large the light radius around the party should be.

int l_set_magic_light_radius(lua_State* L)
{
	Party& party = Party::Instance();
	int radius   = lua_tonumber(L, 1);

	party.set_magic_light_radius(radius);

	return 0;
}

// Contains on the Lua stack at time of calling:
// 1. string containing caster's name,
// 2. string containing the spell name,
// 3. duration of spell.
// We need this information to reconstruct the full file path of the spell.

int l_set_spell_duration_party(lua_State* L)
{
	std::string caster_name = lua_tostring(L, 1);
	std::string spell_name = lua_tostring(L, 2);
	int duration = lua_tonumber(L, 3);

	PlayerCharacter* caster = Party::Instance().get_player(caster_name);

	if (caster == NULL) {
		std::cerr << "ERROR: luaapi.cc: caster == NULL. Spell casting failed! THIS IS A SERIOUS BUG, THE PROGRAM MAY ABORT UNEXPECTEDLY.\n";
		return 0;
	}

	for (auto spell : *(World::Instance().get_spells())) {
		if (spell.profession == caster->profession() && spell.name == spell_name)
			Party::Instance().add_active_spell(spell.full_file_path, duration);
	}

	return 0;
}

int l_make_icon_walkable(lua_State* L)
{
	Party::Instance().add_icon_to_walkable(lua_tonumber(L, 1));
	return 0;
}

int l_remove_icon_walkable(lua_State* L)
{
	Party::Instance().remove_icon_from_walkable(lua_tonumber(L, 1));
	return 0;
}

int l_set_spell_duration_player(lua_State* L)
{
	std::string caster_name = lua_tostring(L, 1); // Name of casting player
	std::string spell_name = lua_tostring(L, 2);
	int duration = lua_tonumber(L, 3);
	std::string player_name = lua_tostring(L, 4); // Name of player affected

	PlayerCharacter* caster = Party::Instance().get_player(caster_name);
	PlayerCharacter* player = Party::Instance().get_player(player_name);

	if (caster == NULL || player == NULL) {
		std::cerr << "ERROR: luaapi.cc: caster == NULL || player == NULL. Spell casting failed! THIS IS A SERIOUS BUG, THE PROGRAM MAY ABORT UNEXPECTEDLY.\n";
		return 0;
	}

	for (auto spell : *(World::Instance().get_spells())) {
		if (spell.profession == caster->profession() && spell.name == spell_name)
			player->add_active_spell(spell.full_file_path, duration);
	}

	return 0;
}

int l_walk_through_fields(lua_State* L)
{
	int rounds = lua_tonumber(L, 1); // How many rounds does the effect last?
	Party::Instance().immunize_from_fields(rounds);
	return 0;
}

void publicize_api(lua_State* L)
{
  // Lua 5.1

/*
  lua_pushcfunction(L, l_pushweapon);
  lua_setglobal(L, "simpl_pushweapon");

  lua_pushcfunction(L, l_pushshield);
  lua_setglobal(L, "simpl_pushshield");
*/

  lua_pushcfunction(L, l_datapath);
  lua_setglobal(L, "simpl_datapath");

  lua_pushcfunction(L, l_printcon);
  lua_setglobal(L, "simpl_printcon");

  lua_pushcfunction(L, l_getcon);
  lua_setglobal(L, "simpl_getcon");

  lua_pushcfunction(L, l_getkey);
  lua_setglobal(L, "simpl_getkey");

  lua_pushcfunction(L, l_ztatswin_save);
  lua_setglobal(L, "simpl_ztatssave");

  lua_pushcfunction(L, l_ztatswin_update);
  lua_setglobal(L, "simpl_ztatsupdate");

  lua_pushcfunction(L, l_ztatswin_restore);
  lua_setglobal(L, "simpl_ztatsrestore");

  lua_pushcfunction(L, l_ztatswin_shopinteraction);
  lua_setglobal(L, "simpl_ztatsshopinteraction");

  lua_pushcfunction(L, l_ztatswin_sell_arms_from_inventory);
  lua_setglobal(L, "simpl_ztatssellarmsfrominventory");

  lua_pushcfunction(L, l_additemtoinv);
  lua_setglobal(L, "simpl_additemtoinv");

  lua_pushcfunction(L, l_set_combat_ptr);
  lua_setglobal(L, "simpl_set_combat_ptr");

  lua_pushcfunction(L, l_bonus);
  lua_setglobal(L, "simpl_bonus");

  lua_pushcfunction(L, l_get_level);
  lua_setglobal(L, "simpl_get_level");

  lua_pushcfunction(L, l_get_partysize);
  lua_setglobal(L, "simpl_get_partysize");

  lua_pushcfunction(L, l_get_player_is_alive);
  lua_setglobal(L, "simpl_get_player_is_alive");

  lua_pushcfunction(L, l_get_player_ac);
  lua_setglobal(L, "simpl_get_ac");

  lua_pushcfunction(L, l_player_set_condition);
  lua_setglobal(L, "simpl_set_player_condition");

  lua_pushcfunction(L, l_player_change_hp);
  lua_setglobal(L, "simpl_player_change_hp");

  lua_pushcfunction(L, l_notify_party_hit);
  lua_setglobal(L, "simpl_notify_party_hit");

  lua_pushcfunction(L, l_rand_player);
  lua_setglobal(L, "simpl_rand_player");

  lua_pushcfunction(L, l_advance_foes);
  lua_setglobal(L, "simpl_advance");

  lua_pushcfunction(L, l_flee_foe);
  lua_setglobal(L, "simpl_flee");

  lua_pushcfunction(L, l_random);
  lua_setglobal(L, "simpl_rand");

  lua_pushcfunction(L, l_modify_gold);
  lua_setglobal(L, "simpl_modify_gold");

  lua_pushcfunction(L, l_get_gold);
  lua_setglobal(L, "simpl_get_gold");

  lua_pushcfunction(L, l_party_size);
  lua_setglobal(L, "simpl_partysize");

  lua_pushcfunction(L, l_remove_from_current_map);
  lua_setglobal(L, "simpl_remove_from_current_map");

  lua_pushcfunction(L, l_buyitem);
  lua_setglobal(L, "simpl_buyitem");

  lua_pushcfunction(L, l_join);
  lua_setglobal(L, "simpl_join");

  lua_pushcfunction(L, l_choose_player);
  lua_setglobal(L, "simpl_choose_player");

  lua_pushcfunction(L, l_choose_monster);
  lua_setglobal(L, "simpl_choose_monster");

  lua_pushcfunction(L, l_add_hp);
  lua_setglobal(L, "simpl_add_hp");

  lua_pushcfunction(L, l_is_dead);
  lua_setglobal(L, "simpl_is_dead");

  lua_pushcfunction(L, l_play_sound);
  lua_setglobal(L, "simpl_play_sound");

  lua_pushcfunction(L, l_is_in_battle);
  lua_setglobal(L, "simpl_party_in_combat");

  lua_pushcfunction(L, l_get_player_name);
  lua_setglobal(L, "simpl_get_player_name");

  lua_pushcfunction(L, l_get_single_monster_name);
  lua_setglobal(L, "simpl_get_single_monster_name");

  lua_pushcfunction(L, l_magic_attack);
  lua_setglobal(L, "simpl_magic_attack");

  lua_pushcfunction(L, l_buyservice);
  lua_setglobal(L, "simpl_buyservice");

  lua_pushcfunction(L, l_change_icon);
  lua_setglobal(L, "simpl_change_icon");

  lua_pushcfunction(L, l_make_guards);
  lua_setglobal(L, "simpl_make_guards");

  lua_pushcfunction(L, l_level_up);
  lua_setglobal(L, "simpl_level_up");

  lua_pushcfunction(L, l_set_spell_duration_party);
  lua_setglobal(L, "simpl_set_spell_duration_party");

  lua_pushcfunction(L, l_set_spell_duration_player);
  lua_setglobal(L, "simpl_set_spell_duration_player");

  lua_pushcfunction(L, l_set_magic_light_radius);
  lua_setglobal(L, "simpl_set_magic_light_radius");

  lua_pushcfunction(L, l_make_icon_walkable);
  lua_setglobal(L, "simpl_make_icon_walkable");

  lua_pushcfunction(L, l_remove_icon_walkable);
  lua_setglobal(L, "simpl_remove_icon_walkable");

  lua_pushcfunction(L, l_walk_through_fields);
  lua_setglobal(L, "simpl_walk_through_fields");

  lua_pushcfunction(L, l_party_is_outdoors);
  lua_setglobal(L, "simpl_party_is_outdoors");

  // Lua 5.2 and newer:
  //  static const luaL_Reg methods[] = {
  //    {"simpl_pushweapon", l_pushweapon},
  //    {"simpl_datapath", l_datapath},
  //    {"simpl_printcon", l_printcon},
  //    {NULL,NULL}
  //  };
  //  lua_pushglobaltable(L);
  //  lua_pushglobaltable(L);
  //  lua_setfield(L, -2, "_G");
  //  luaL_setfuncs(L, methods, 0);
}
