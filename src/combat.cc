//
//
// Copyright (c) 2005  Andreas Bauer <baueran@gmail.com>
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
#include <sstream>
#include <string>
#include <algorithm>
#include <memory>
#include <boost/unordered_set.hpp>
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include <SDL.h>
#include <SDL_keysym.h>
#include "simplicissimus.hh"
#include "soundsample.hh"
#include "console.hh"
#include "creature.hh"
#include "gamecontrol.hh"
#include "combat.hh"
#include "miniwin.hh"
#include "eventmanager.hh"
#include "weaponhelper.hh"
#include "shieldhelper.hh"
#include "luaapi.hh"
#include "luawrapper.hh"
#include "playercharacter.hh"
#include "party.hh"
#include "ztatswin.hh"
#include "util.hh"
#include "gold.hh"
#include "gamerules.hh"
#include "config.h"

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lualib.h>
#include <lauxlib.h>
}

#ifndef SLOWPRINT
#define SLOWPRINT true
#endif

using namespace std;

Combat::Combat()
{
  party = &Party::Instance();
  em = &EventManager::Instance();
  fled = false;
}

Combat::~Combat()
{
}

// Returns true on victory, false otherwise.

bool Combat::initiate()
{
	if (foes.size() > 0) {
		std::stringstream ss;
		ss << "\nYou're faced with "
				<< foes.to_string() << ".\n"
				<< "Do you wish to "
				<< (foes.closest_range() > 10 ? "(a)dvance, " : "")
				<< "(f)ight or attempt to (r)un?";
		printcon(ss.str());

		switch (em->get_key(foes.closest_range() > 10 ? "afr" : "fr")) {
		case 'a':
			if (foes.closest_range() > 10) {
				advance_party();
				foes_fight();
				return initiate();
				// return false; // TODO is this return necessary?
			}
			break;
		case 'f':
			fight(attack_options());

			if (foes.size() > 0)
				return initiate();
			else {
				victory();
				return true;
			}
			break;
		default:
			printcon("You got away this time!");
			MiniWin::Instance().display_last();
			return false;
		}
	}

	// Monster nearby...
	if (create_random_monsters()) {
		string _name = noticed_monsters();
		if (_name.length() > 0) {
			printcon(_name + (std::string)" heard some suspicious " +
					(std::string)"noise nearby. Do you wish to " +
					(std::string)"(i)nvestigate or (m)ove on?");

			if (em->get_key("im") == 'm') {
				printcon("You got away this time!");
				MiniWin::Instance().display_last();
				return false;
			}

			MiniWin::Instance().display_surf(foes.pic());
			std::stringstream ss;
			ss << "Upon getting closer you spotted "
					<< foes.to_string() << ".\n"
					<< "Do you wish to "
					<< (foes.closest_range() > 10 ? "(a)dvance, " : "")
					<< "(f)ight or attempt to (s)neak away?";
			printcon(ss.str());

			switch (em->get_key(foes.closest_range() > 10 ? "afs" : "fs")) {
			case 'a':
				if (foes.closest_range() > 10) {
					advance_party();
					foes_fight();
					return initiate();
				}
				break;
			case 'f':
				fight(attack_options());

				if (foes.size() <= 0) {
					victory();
					return true;
				}
				else
					return initiate();
				break;
			default:
				printcon("You got away this time!");
				MiniWin::Instance().display_last();
				return false;
			}
		}
		else {
			MiniWin::Instance().display_surf(foes.pic());
			std::stringstream ss;
			ss << "Your stalwart party is brought to a halt by ";
			ss << foes.to_string() << ".\n";
			ss << "Do you wish to ";
			ss << (foes.closest_range() > 10 ? "(a)dvance, " : "");
			ss << "(f)ight or attempt to (r)un?";
			printcon(ss.str());

			switch (em->get_key(foes.closest_range() > 10 ? "afr" : "fr")) {
			case 'a':
				if (foes.closest_range() > 10) {
					advance_party();
					foes_fight();
					return initiate();
				}
				break;
			case 'f':
				fight(attack_options());

				if (foes.size() <= 0) {
					victory();
					return true;
				}
				else
					return initiate();
				break;
			default:
				printcon("You got away this time!");
				MiniWin::Instance().display_last();
				return false;
			}
		}
	}

	return false;
}

std::vector<int> Combat::attack_options()
{
  // Stores the choices of the party members.  The individual values
  // of choices are as follows: if the player defends, -1 is stored at
  // the respective position in array, otherwise the number of the
  // monster that is to be attacked.
  std::vector<int> choices;
  choices.reserve(party->party_size());
  choices.resize(party->party_size());

  for (int i = 0; i < party->party_size(); i++) {
    PlayerCharacter* player = party->get_player(i);

    if (player->condition() == DEAD)
    	continue;

    ZtatsWin::Instance().highlight_lines(i * 2, i * 2 + 2);

    std::stringstream ss;
    ss << player->name() << " has these options this battle round:\n";
    ss << "(A)ttack foes with ";
    if (player->weapon() != NULL) {
      ss << (Util::vowel(player->weapon()->name()[0])? "an " : "a ") << player->weapon()->name();
      ss << ".\n(D)efend.\n(R)eady item.";
    }
    else
      ss << "bare hands.\n(D)efend.\n(R)eady item.";
    printcon(ss.str());
    ss.str("");

    char input = em->get_key("adr");
    if (input == 'a') {
      if (foes.count()->size() == 1)
        choices[i] = 1;
      else {
        int attacked = select_enemy(i);
	
        string attacked_name;
        int j = 1;
        for (auto foe : *(foes.count())) {
          if (attacked == j)
            attacked_name = foe.first;
          j++;
        }
        printcon(player->name() + " will attack " +
                 (Util::vowel(attacked_name[0]) ? "an " : "a ") +
                 attacked_name + " in the next round.");
        choices[i] = attacked;
      }
    }
    else if (input == 'd') {
      printcon(player->name() + " will defend in the next round.");
      choices[i] = -1;
    }
    else { // (R)eady item
      std::string new_weapon = GameControl::Instance().ready_item(i);

      if (new_weapon != "") {
	ss << player->name() + " will ready a" << 
	  (Util::vowel(new_weapon[0])? "an " : "a ") << new_weapon << " in the next round.";
	printcon(ss.str());
      }
      else
	printcon(player->name() + " will defend in the next round.");

      choices[i] = -1;
    }
    ZtatsWin::Instance().unhighlight_lines(i * 2, i * 2 + 2);
  }

  return choices;
}

int Combat::fight(const std::vector<int> choices)
{
  party_fight(choices);
  foes_fight();
  return 0;
}

// Returns number of monsters left after a round of melee has taken
// place, also handles the actual melee itself after the characters
// have chosen their respective actions for this round.
//
// choices contains the choices the attacking party made, see
// attack_options() for details.

int Combat::party_fight(const std::vector<int> choices)
{
	LuaWrapper lua(_lua_state);
	static SoundSample sample;

	if ((int)choices.size() < party->party_size())
		std::cerr << "Warning: Attack choices < party size. This is serious.\n";

	// The party's moves...
	int i = 0;
	for (auto player = party->party_begin(); player != party->party_end(); i++, player++) {
		if (player->condition() == DEAD)
			continue;

		if (choices[i] > -1) {
			// Choose weapon to attack with
			Weapon* wep = player->weapon();

			// Get the monster that is to be attacked this round
			Creature* opponent = NULL;
			int opponent_offset = 0;

			int j = 1;
			for (auto foe : *(foes.count())) {
				if (j == choices[i]) {
					int k = 0;
					for (auto _foe = foes.begin(); _foe != foes.end(); _foe++, k++) {
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
				std::cerr << "Warning: uicombat:: opponent == null (No monster to attack.)\n";
				break;
			}

			if (wep != NULL && opponent->distance() <= wep->range()) {
				stringstream ss;
				ss << player->name() + " swings the " + wep->name() + " at " << opponent->name();

				int temp_AC = 10; // TODO: Replace this with the actual AC of opponent!  This AC needs to be computed from weapons, dex, etc.

				if (random(1, 20) > temp_AC) {
					int damage = random(wep->dmg_min(), wep->dmg_max());
					ss << " and hits for " << damage << " points of damage.";
					if (opponent->hp() - damage > 0) {
						opponent->set_hp(opponent->hp() - damage);
						lua.push_fn_arg((double)(opponent->hp() - damage));
						lua.call_void_fn("set_hp");
					}
					else {
						ss << " killing the " << opponent->name() << ".";
						foes.remove(opponent_offset);

						// Add experience points to player's balance
						player->inc_ep(lua.call_fn<double>("get_ep"));

						// Now add monster's items to bounty items to be collected
						// by party in case of battle victory.
						_bounty_items.add(opponent->weapon());

						// Add monster's gold
						int gold_coins = lua.call_fn<double>("get_gold");
						for (int ii = 0; ii < gold_coins; ii++)
							_bounty_items.add(new Gold());
					}
					printcon(ss.str(), true);
					MiniWin::Instance().alarm();
					sample.play(FOE_HIT);
				}
				else {
					ss << " and misses.";
					printcon(ss.str(), true);
				}
			}
			else if (wep != NULL) {
				stringstream ss;
				ss << player->name() << " tries to attack " << opponent->name() << " but cannot reach.";
				printcon(ss.str(), true);
			}
			// Attack with bare hands...
			else {
				stringstream ss;
				ss << player->name() << " attempts a futile karate punch at " << opponent->name() << " but fails.";
				printcon(ss.str(), true);
			}
		}
	}

	return 0;
}

void Combat::victory()
{
  printcon("Your party emerged victorious!", true);
  MiniWin::Instance().display_last();

  if (_bounty_items.size() > 0) {
    ZtatsWin& zwin = ZtatsWin::Instance();
    printcon("As the dust of battle lifts, some items are left over. Dost thou wish to pick them up? (y/n)", true);

    switch (em->get_key("yn")) {
    case 'y': {
      MiniWin&  mwin = MiniWin::Instance();
      mwin.save_surf();
      mwin.clear();
      mwin.println(0, "Pick up items", CENTERALIGN);
      mwin.println(1, "(Press space to select an item, q when done)", CENTERALIGN);
          
      std::map<std::string, int> tmp = _bounty_items.list_all();
      std::vector<line_tuple>   tmp2 = Util::to_line_tuples(tmp);
      zwin.set_lines(tmp2);
      zwin.clear();
      zwin.select_items();

      // Remove gold, if any, from bounty and add it to party's gold account instead
      Gold tmp_gold;
      int new_gold = _bounty_items.remove_all(tmp_gold.name());
      party->set_gold(party->gold() + new_gold);
      GameControl::Instance().draw_status();

      party->inventory()->add_all(_bounty_items);
      mwin.display_last();
      break;
    }
    case 'n':
      return;
    }
  }
  // No need to clean bounty items as they are destroyed when combat is over!
}

int Combat::foes_fight()
{
	LuaWrapper lua(_lua_state);
	boost::unordered_set<std::string> moved;

	for (int i = 0; i < foes.size(); i++) {
		Creature* foe = foes.get()->at(i).get();

		// Load corresponding Lua monster definition
		if (luaL_dofile(_lua_state, ((std::string)DATADIR + "/" +
				(std::string)PACKAGE + "/data/" +
				(std::string)WORLD_NAME + "/bestiary/" +
				boost::algorithm::to_lower_copy(foe->name()) +
				(std::string)".lua").c_str()))
		{
			cerr << "INFO: combat.cc::foes_fight(): Couldn't execute Lua file: " << lua_tostring(_lua_state, -1) << endl;
			cerr << "Assuming instead that we're fighting with someone from an indoors map...\n";

			if (lua.call_fn<bool>("attack") && !fled)
				lua.call_fn<double>("fight");

			// All of the party died of the attack?
			if (Party::Instance().party_alive() == 0) {
				GameControl::Instance().game_over();
				return 0;
			}
		}
		// Fight against ordinary monster, defined in monster definition file
		else {
			// Convert the this-pointer to string and push it to Lua-Land
			// along with i, such that Lua knows which monster is referred
			// to.  (I know, this is very crazy code, but life is crazy.)
			std::ostringstream thiss;
			thiss << (void const *)this;
			lua.push_fn_arg((double)i);
			lua.push_fn_arg((std::string)thiss.str());
			lua.call_void_fn("set_combat_ptr");

			lua.push_fn_arg((double)foe->gold());
			lua.call_void_fn("set_gold");

			lua.push_fn_arg((std::string)foe->weapon()->name());
			lua.call_void_fn("set_weapon");

			lua.push_fn_arg((double)foe->luck());
			lua.call_void_fn("set_luck");

			lua.push_fn_arg((double)foe->dxt());
			lua.call_void_fn("set_dxt");

			lua.push_fn_arg((double)foe->hp());
			lua.call_void_fn("set_hp");

			lua.push_fn_arg((double)foe->hpm());
			lua.call_void_fn("set_hp_max");

			lua.push_fn_arg((double)foe->distance());
			lua.call_void_fn("set_distance");

			if (lua.call_fn<bool>("advance") &&                // Foes want to advance? - as opposed to, say, fight from the distance
					moved.find(foe->name()) == moved.end())    // Foes haven't yet advanced in this round?
				moved = advance_foes();                        // Attempt to move

			if (moved.find(foe->name()) == moved.end() && lua.call_fn<bool>("attack") && !fled)
				lua.call_fn<double>("fight");

			// All of the party died of the attack?
			if (Party::Instance().party_alive() == 0) {
				GameControl::Instance().game_over();
				return 0;
			}

			if (fled)
				i--;
			fled = false;
		}

		ZtatsWin::Instance().update_player_list();
	}

	return 0;
}

// Returns which monster is going to be attacked in the next round by
// player player_no.

int Combat::select_enemy(int player_no)
{
  std::stringstream options;
  int i = 0;

  for (auto foe : *(foes.count())) {
    options << ++i;
    std::stringstream ss;
    ss << i << ") " << foe.second << " ";
    if (foe.second > 1)
      ss << foes.get_plural_name(foe.first);
    else
      ss << foe.first;
    ss << " (" << foes.get_distance(foe.first) << "')";
    printcon(ss.str());
  }

  char pressed = em->get_key(options.str().c_str());
  return atoi(&pressed);
}

// Makes the n-th foe in vector foes try to leave combat without a fight.
// If it was the last foe of a group, the group disappears (e.g., group consisting of 1 Orc only).
// If it was the last foe in combat, the combad ends.

void Combat::flee_foe(int n)
{
  Creature* foe = foes.get()->at(n).get();
  std::stringstream ss;
  if (foes.count()->at(foe->name()) == 1)
    ss << "The " << foe->name() << " flees.";
  else
    ss << "A" << (Util::vowel(foe->name()[0])? "n " : " ")  << foe->name() << " flees.";
  printcon(ss.str(), true);

  std::cout << "About to remove " << n << "-th foe\n";
  foes.remove(n);
  fled = true;
}

// Called advance party, but really decreases monsters' distances.

void Combat::advance_party()
{
  for (auto itr = foes.begin(); itr != foes.end(); itr++) {
    if ((*itr)->distance() > 10)
      (*itr)->set_distance((*itr)->distance() - 10);
  }
  printcon("Your party advances.", true);
}

Attackers Combat::get_foes()
{
	return foes;
}

void Combat::set_foes(Attackers new_foes)
{
	foes = new_foes;
}

// Advances the foes by 10' and returns the name of the foes that have moved.

boost::unordered_set<std::string> Combat::advance_foes()
{
  boost::unordered_set<string> moved;
  boost::unordered_map<int, std::string> distances = foes.distances();
  for (auto foe = foes.begin(); foe != foes.end(); foe++) {
    if ((*foe)->distance() >= 20 && moved.find((*foe)->name()) == moved.end()) {
      try {
        // See if foe can move backwards or if this slot is blocked by other foes.
        // If there is no one there, then move (i.e., execute catch block).
        distances.at((*foe)->distance() - 10);
      }
      catch (std::out_of_range& oor) {
        foes.move((*foe)->name().c_str(), -10);
        moved.insert((*foe)->name());
        distances = foes.distances();
        if (foes.count()->at((*foe)->name()) == 1)
          printcon("The " + (*foe)->name() + " advances.", true);
        else
          printcon("The " + (*foe)->plural_name() + " advance.", true);
      }
    }
  }
  return moved;
}

// Used for town folk, when the player attacks, e.g. a guard or runs amok otherwise.
// Then we need to initiate battle, too, but create the "monsters", so to speak, rather
// differently.  Script_path is the path to the Lua script defining the person in the town.

bool Combat::create_monsters_from_init_path(std::string script_file)
{
	LuaWrapper lua(_lua_state);
	std::string lua_scripts_path = (std::string)DATADIR + "/" + (std::string)PACKAGE + "/data/" + (std::string) WORLD_NAME + "/";

	// Load corresponding Lua conversation file
	if (luaL_dofile(_lua_state, (lua_scripts_path + boost::algorithm::to_lower_copy(script_file)).c_str())) {
		std::cerr << "ERROR: combat.cc::create_monsters_from_init_path(): Couldn't execute Lua file: " << lua_tostring(_lua_state, -1) << std::endl;
		exit(EXIT_FAILURE);
	}

	// Load generic Lua script for fighting which is referenced by every town folk
	// Town folk don't have dedicated combat functions as it's not the norm that the party will attack town people.
	lua.push_fn_arg(lua_scripts_path + (std::string)"people/generic_fight.lua");
	lua.call_fn_leave_ret_alone("load_generic_fight_file");

	// Push c_values table onto Lua stack...
	// In fact, pushes only one cell to stack, which is then popped below,
	// cf. http://stackoverflow.com/questions/1217423/how-to-use-lua-pop-function-correctly
	lua_getglobal(_lua_state, "c_values");
	// ...then access its values
    std::shared_ptr<GameCharacter> foe = std::static_pointer_cast<GameCharacter>(create_character_values_from_lua(_lua_state));
    // Pop Lua stack!
    lua_pop(_lua_state, 1);

    foes.count()->insert(std::make_pair(foe->name(), 1));
    foes.add(std::static_pointer_cast<Creature>(foe));

    // Set distance of foe to 10'
    for (auto f: foes)
    	f->set_distance(10);

	return true;
}

// Usually used for monsters inside dungeons...

bool Combat::create_monsters_from_combat_path(std::string script_file)
{
	LuaWrapper lua(_lua_state);
	std::string lua_scripts_path = (std::string)DATADIR + "/" + (std::string)PACKAGE + "/data/" + (std::string) WORLD_NAME + "/";

	// Load corresponding Lua combat file (i.e., a monster definition)
	if (luaL_dofile(_lua_state, (lua_scripts_path + boost::algorithm::to_lower_copy(script_file)).c_str())) {
		std::cerr << "ERROR: combat.cc::create_monsters_from_combat_path(): Couldn't execute Lua file: " << lua_tostring(_lua_state, -1) << std::endl;
		exit(EXIT_FAILURE);
	}

	// TODO: here is some code missing to create a proper set of foes,
	// consisting of different kinds of enemies, rather than just n of
	// the same!

	int number_of_enemies = random(1, 6);
	int distance = random(1,3) * 10;
	std::string name = lua.call_fn<std::string>("get_name");

	for (int i = 0; i < number_of_enemies; i++) {
		std::shared_ptr<Creature> monster(new Creature());

		// Set distance
		monster->set_distance(distance);
		lua.push_fn_arg((double)distance);
		lua.call_void_fn("set_distance");

		// Set rest of params
		monster->set_img(lua.call_fn<std::string>("img_path"));
		monster->set_name(lua.call_fn<std::string>("get_name"));
		monster->set_plural_name(lua.call_fn<std::string>("get_plural_name"));

		// Create lua instance of monster and set further params
		lua.call_void_fn("create_instance");
		monster->set_hp(lua.call_fn<double>("get_hp"));
		monster->set_hpm(lua.call_fn<double>("get_hp_max"));
		monster->set_str(lua.call_fn<double>("get_strength"));
		monster->set_luck(lua.call_fn<double>("get_luck"));
		monster->set_gold(lua.call_fn<double>("get_gold"));
		Weapon* wep = WeaponHelper::createFromLua(lua.call_fn<std::string>("get_weapon"));
		monster->set_weapon(wep);

		// Add monster to rooster of attackers
		foes.add(monster);
	}

    foes.count()->insert(std::make_pair(name, number_of_enemies));

	return true;
}

bool Combat::create_random_monsters()
{
  LuaWrapper lua(_lua_state);
  lua.push_fn_arg((std::string)"plain");
  lua.call_fn_leave_ret_alone("rand_encounter");

  int __distance = -1;

  // Iterate through result table
  lua_pushnil(_lua_state);
  while (lua_next(_lua_state, -2) != 0) {
    string __name = "";
    int __number = -1;

    lua_pushnil(_lua_state);
    while (lua_next(_lua_state, -2) != 0) {
      string __key = lua_tostring(_lua_state, -2);

      // Name of monster
      if (__key == "__name") {
        __name = lua_tostring(_lua_state, -1);
      }
      // Distance, we only determine one and then derive the others below
      else if (__key == "__distance") {
        if (__distance < 0) {
          if (lua_tonumber(_lua_state, -1) > 0)
            __distance = lua_tonumber(_lua_state, -1);
          else
            cerr << "Lua error: " << lua_tostring(_lua_state, -1) << endl;
        }
      } // How many of the monster attack?
      else if (__key == "__number") {
        __number = lua_tonumber(_lua_state, -1);
        if (__number > 0)
          foes.count()->insert(std::make_pair(__name, __number));
      }
      else {
        cerr << "Error: combat.cc: Did you fiddle with the bestiary/defs.lua file?\n";
        exit(EXIT_FAILURE);
      }
      lua_pop(_lua_state, 1);
    }

    lua_pop(_lua_state, 1);
  }

  // Now populate foe data structure with some further monster stats
  // according to the individual lua monster definitions

  // TODO: The way we iterate through the monsters means they always
  // appear in the same order when attacking in groups
  for (auto foe = foes.count()->begin(); foe != foes.count()->end(); foe++, __distance += 10) {
    std::string __name = foe->first;
    int count = foe->second;

    // Create count many monsters named name, respectively
    for (int i = 0; i < count; i++) {
      std::shared_ptr<Creature> monster(new Creature());

      // Load corresponding Lua monster definition
      if (luaL_dofile(_lua_state, ((string) DATADIR + "/" + (string)PACKAGE + "/data/" +
                                   (string) WORLD_NAME + "/bestiary/" +
                                   boost::algorithm::to_lower_copy(__name) +
                                   (string) ".lua").c_str())) {
        cerr << "ERROR: combat.cc::create_random_monsters(): Couldn't execute Lua file: " << lua_tostring(_lua_state, -1) << endl;
        exit(EXIT_FAILURE);
      }

      // Set distance
      monster->set_distance(__distance);
      lua.push_fn_arg((double)__distance);
      lua.call_void_fn("set_distance");

      // Set rest of params
      monster->set_img(lua.call_fn<std::string>("img_path"));
      monster->set_name(lua.call_fn<std::string>("get_name"));
      monster->set_plural_name(lua.call_fn<std::string>("get_plural_name"));

      // Create lua instance of monster and set further params
      lua.call_void_fn("create_instance");
      monster->set_hp(lua.call_fn<double>("get_hp"));
      monster->set_hpm(lua.call_fn<double>("get_hp_max"));
      monster->set_str(lua.call_fn<double>("get_strength"));
      monster->set_luck(lua.call_fn<double>("get_luck"));
      monster->set_gold(lua.call_fn<double>("get_gold"));
      Weapon* wep = WeaponHelper::createFromLua(lua.call_fn<std::string>("get_weapon"));
      monster->set_weapon(wep);

      // Add monster to rooster of attackers
      foes.add(monster);
    }
  }

  return foes.count()->size() > 0;
}

// Returns the name of a player, if the player noticed nearby monsters
// (before they noticed him).  Otherwise an empty string is returned.

std::string Combat::noticed_monsters()
{
	// Determine who sees the other first, try each player,
	// individually. (TODO: Dogs are to be handled separately.)
	std::string _name = "";
	for (auto curr_player = party->party_begin(); curr_player != party->party_end(); curr_player++) {
		if (curr_player->condition() == DEAD)
			continue;

		if (GameRules::bonus(curr_player->luck()) + random(1, 12) >= 9) {
			_name = curr_player->name();
			break;
		}
	}

	return _name;
}

int Combat::random(int min, int max)
{
  return GameControl::Instance().random(min, max);
}

void Combat::printcon(const std::string s, bool wait)
{
  Charset normal_font;
  Console::Instance().print(&normal_font, s, wait);
}
