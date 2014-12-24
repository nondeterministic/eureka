//
//
// Copyright (c) 2012  Andreas Bauer <baueran@gmail.com>
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
#include "config.h"
#include "weapon.hh"
#include "shield.hh"
#include "simplicissimus.hh"
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
#include "race.hh"

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
  if (lua_gettop(L) == 1)
    GameControl::Instance().printcon(lua_tostring(L, 1));
  else if (lua_gettop(L) == 2)
    GameControl::Instance().printcon(lua_tostring(L, 1), lua_toboolean(L, 2));
  else {
    std::cerr << "Error: Lua: wrong number of arguments to l_printcon.\n";
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

int l_ztatswin_save(lua_State* L)
{
	ZtatsWin& zwin = ZtatsWin::Instance();
	zwin.save_surf();
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

/**
 * When called, on the Lua stack has to be a table of weapons.
 * This table will then be inserted into the ztats window display,
 * and the user can select an item for purchase.
 *
 * Returns the name of the selected item, which can be turned
 * into an actual item using, e.g., the methods in ItemFactory.
 * If no item was selected, "" is returned.
 */

int l_ztatswin_shopinteraction(lua_State* L)
{
	int left_border = 9;
	ZtatsWin& zwin = ZtatsWin::Instance();
	std::vector<line_tuple> lines;

	lua_pushnil(L);

	while (lua_next(L, -2) != 0) {
		lua_pushnil(L);

    	std::string name;
    	std::string gold;

		while (lua_next(L, -2) != 0) {
            std::string val = lua_tostring(L, -1);
            std::string key = lua_tostring(L, -2);

        	if (key == "name")
        		name = val;
        	else if (key == "gold")
        		gold = val;

        	lua_pop(L, 1);
    	}

    	std::stringstream ss;
    	ss << "(" << gold << " Au)";
    	int str_len = ss.str().length();
    	for (int i = str_len; i < left_border; i++)
    		ss << " ";
    	ss << name;
    	lines.push_back(line_tuple(ss.str(), LEFTALIGN));

    	lua_pop(L, 1);
	}

	lua_pop(L, 1);

	zwin.set_lines(lines);
    zwin.clear();

    // Now determine name of selected item
    int selected = zwin.select_item();

	// Return item name or "" if none was selected
    if (selected >= 0) {
    	std::string item_name = lines[selected].get<0>().substr(left_border);
    	lua_pushstring(L, item_name.c_str());
    }
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
	int left_border = 3;
	int price_reduction = (lua_tonumber(L, 1));

	Party& party = Party::Instance();
	ZtatsWin& zwin = ZtatsWin::Instance();

	if (party.inventory()->size() == 0) {
		GameControl::Instance().printcon("Looks like you have nothing to offer.");
		return 0;
	}

	std::map<std::string, int> tmp = party.inventory()->list_wearables();
	std::vector<line_tuple> tmp2 = Util::to_line_tuples(tmp);
	zwin.set_lines(tmp2);
	zwin.clear();

    // Now determine name of selected item
    int selected = zwin.select_item();

	// Return item name or "" if none was selected
    if (selected >= 0) {
    	std::string sel_name = tmp2[selected].get<0>().substr(left_border);
    	std::string item_name = sel_name;
    	int how_many = 1;

    	// If there are more than 1 in the inventory, we need to do a bit of extra work...
    	// We know that, because the inventory is then as follows
    	// "1) arrow (22)"
    	// "2) sword (4)"
    	// "3) axe"
    	// "4) sling"

    	if (sel_name.find("(") != string::npos) {
    		item_name = sel_name.substr(0, sel_name.find("(") - 1);
    		how_many = std::stoi(sel_name.substr(sel_name.find("(") + 1 , sel_name.length() - sel_name.find("(") - 2));
    	}

		int selling_how_many = 1; // How many are actually sold of the #how_many
    	if (how_many > 1) {
    		stringstream ss;
    		ss << "How many (0 - " << how_many << ")?";
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

    		if (selling_how_many > how_many) {
    			GameControl::Instance().printcon("Sadly, you don't have that many.");
    			return 0;
    		}
    		else if (selling_how_many == 0) {
    			GameControl::Instance().printcon("Changed your mind then, eh?");
    			return 0;
    		}
    	}

    	// Simple test to see, if the item can be sold at all.
    	// Not nice to allocate and deallocate for that purpose, but not so bad either...
    	Item* item = ItemFactory::create_plain_name(item_name);

    	if (item != NULL) {
    		// Buying price is item price - price_reduction %
    		int item_price = item->gold() - (int)((float)item->gold() / 100.0 * (float)price_reduction);

			GameControl::Instance().printcon("I can offer you " + std::to_string(item_price) + " per piece. Agree (y/n)?");
			char agree = EventManager::Instance().get_key("yn");

			if (agree == 'y') {
				// Now add items to inventory...
				for (int i = 0; i < selling_how_many; i++) {
					std::cout << "Removing...\n";
					party.inventory()->remove(item->name());
					party.set_gold(party.gold() + item_price);
				}
				GameControl::Instance().printcon("Was a pleasure doing business with you.");
			}
			else {
				GameControl::Instance().printcon("Too bad. That's all I can offer you.");
			}

    		delete item;
        	GameControl::Instance().draw_status(false);
    	}
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
	int left_border = 9;

	std::string item_name = (std::string)(lua_tostring(L, 1));
	Item* item = ItemFactory::create_plain_name(item_name);

	if (item == NULL) {
		std::cerr << "WARNING: luaapi.cc::l_additemtoinv: cannot create item " << item_name << ".\n";
		return 0;
	}

	Party::Instance().inventory()->add(item);
	GameControl::Instance().draw_status(false);
	return 0;
}

/**
 * Like l_additemtoinv, except that it checks if
 * a) the inventory has room for it
 * b) the party has enough gold
 *
 * If a) fails, -1 is returned. If b) fails, -2 is returned.
 * If another error occurs, -3 is returned.
 * On success 0 is returned.
 */

int l_buyitem(lua_State* L)
{
	std::string item_name = (std::string)(lua_tostring(L, 1));
	Item* item = ItemFactory::create_plain_name(item_name);

	if (item != NULL) {
		if (true) { // TODO: Replace and check weight restrictions...
			if (Party::Instance().gold() >= item->gold()) {  // Check if sufficient gold
				Party::Instance().set_gold(Party::Instance().gold() - item->gold());
				Party::Instance().inventory()->add(item);
				GameControl::Instance().draw_status(false);
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

int l_datapath(lua_State* L)
{
  lua_pushstring(L, ((string)DATADIR + "/" + (string)PACKAGE + "/data/" + (string)WORLD_NAME + "/").c_str());
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

int l_get_player_ac(lua_State* L)
{
  std::string player_name = (std::string)(lua_tostring(L, 1));
  PlayerCharacter* player = Party::Instance().get_player(player_name);
  if (player == NULL) {
    std::cerr << "Error: no player named " << player_name << " found.\n";
    exit(EXIT_FAILURE);
  }
  lua_pushnumber(L, player->armour_class());
  return 1;
}

int l_player_change_hp(lua_State* L)
{
  std::string player_name = (std::string)(lua_tostring(L, 1));
  PlayerCharacter* player = Party::Instance().get_player(player_name);
  int change = lua_tonumber(L, 2);
  player->set_hp(player->hp() + change);

  return 0;
}

int l_notify_party_hit(lua_State* L)
{
  static SoundSample sample;

  Console::Instance().alarm();
  sample.play(HIT);

  return 0;
}

// Returns the names of at most n randomly chosen players from the
// front row of the party.  If the party is smaller, it still returns
// n strings, some of which are empty.

int l_rand_player(lua_State* L)
{
  boost::unordered_set<PlayerCharacter*> players;
  int requested = (int)lua_tonumber(L, 1);
  int count = min(min(Party::Instance().party_size(), 3), requested);

  // std::cout << "Simpl: requested: " << requested << ", count: " << count << std::endl;

  while ((int)players.size() < count) {
    int rnd = GameControl::Instance().random(0, min(Party::Instance().party_size() - 1, 2));
    players.insert(Party::Instance().get_player(rnd));
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
		GameControl::Instance().draw_status(false);
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

// TODO:
// Accessing table like this:
// http://www.wellho.net/mouth/1845_Passing-a-table-from-Lua-into-C.html

int l_join(lua_State* L)
{
	if (Party::Instance().party_size() >= 6)
		return 0;

	PlayerCharacter player;

	lua_pushstring(L, "name");
	lua_gettable(L, -2);
	player.set_name(lua_tostring(L, -1)); // Extract result, which is now on the stack
	lua_pop(L,1);                         // Tidy up stack by getting rid of the extra we just put on

	lua_pushstring(L, "race");
	lua_gettable(L, -2);
	std::string race = lua_tostring(L, -1);
	if (race == "HUMAN")
		player.set_race(HUMAN);
	else if (race == "ELF")
		player.set_race(ELF);
	else if (race == "HALF_ELF")
		player.set_race(HALF_ELF);
	else if (race == "HOBBIT")
		player.set_race(HOBBIT);
	else // if (race == "DWARF")
		player.set_race(DWARF);
	lua_pop(L,1);

	lua_pushstring(L, "ep");
	lua_gettable(L, -2);
	player.inc_ep(lua_tonumber(L, -1));
	lua_pop(L,1);

	lua_pushstring(L, "hp");
	lua_gettable(L, -2);
	player.set_hp(lua_tonumber(L, -1));
	lua_pop(L,1);

	lua_pushstring(L, "hpm");
	lua_gettable(L, -2);
	player.set_hpm(lua_tonumber(L, -1));
	lua_pop(L,1);

	lua_pushstring(L, "sp");
	lua_gettable(L, -2);
	player.set_sp(lua_tonumber(L, -1));
	lua_pop(L,1);

	lua_pushstring(L, "spm");
	lua_gettable(L, -2);
	player.set_spm(lua_tonumber(L, -1));
	lua_pop(L,1);

	lua_pushstring(L, "str");
	lua_gettable(L, -2);
	player.set_str(lua_tonumber(L, -1));
	lua_pop(L,1);

	lua_pushstring(L, "luck");
	lua_gettable(L, -2);
	player.set_luck(lua_tonumber(L, -1));
	lua_pop(L,1);

	lua_pushstring(L, "dxt");
	lua_gettable(L, -2);
	player.set_dxt(lua_tonumber(L, -1));
	lua_pop(L,1);

	// TODO
	/// ....

	Party::Instance().add_player(player);
    ZtatsWin::Instance().update_player_list();

	return 0;
}

// Item id is on the Lua stack at calling time.  Item will then be removed from the map,
// because either it died in battle, joined the party, etc.  Map is usually indoors as outdoors,
// we don't show detailed icon view.

int l_remove_from_current_map(lua_State* L)
{
	std::string id = lua_tostring(L, 1); // This is the id used inside the current map XML-file!
	GameControl::Instance().get_map()->pop_obj(id);

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

  lua_pushcfunction(L, l_get_player_ac);
  lua_setglobal(L, "simpl_get_ac");

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
