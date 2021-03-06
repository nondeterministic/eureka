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

#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <memory>
#include <cmath>

#include <boost/regex.hpp>
#include <boost/bind.hpp>
#include <boost/unordered_set.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>

#include "eureka.hh"
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
#include "spellcasthelper.hh"
#include "attackers.hh"
#include "outdoorsicons.hh"
#include "indoorsicons.hh"
#include "world.hh"
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

Combat::Combat()
{
	_party = &Party::Instance();
	_em = &EventManager::Instance();
	_fled = false;
	_party->is_in_combat = true;
}

Combat::~Combat()
{
	_party->is_in_combat = false;
}

// Returns true on victory, false otherwise.

Combat_Return_Codes Combat::initiate()
{
	MiniWin& mwin = MiniWin::Instance();

	// This if-block is executed, when foes are left over from a previous encounter.
	// Otherwise, the random monster generation below is executed first.
	if (_foes.size() > 0) {
		mwin.display_texture(_foes.pic(mwin.get_renderer()));

		std::stringstream ss;
		ss << "You're faced with "
				<< _foes.to_string() << ".\n"
				<< "Do you wish to "
				<< (_foes.closest_range() > 10 ? "(a)dvance, " : "")
				<< "(f)ight or attempt to (r)un?";
		printcon(ss.str());

		switch (_em->get_key(_foes.closest_range() > 10 ? "afr" : "fr")) {
		case 'a':
			if (_foes.closest_range() > 10) {
				advance_party();
				foes_fight();
				return initiate();
			}
			break;
		case 'f':
			fight(attack_options());

			if (_foes.size() > 0)
				return initiate();
			else {
				victory();
				return VICTORY;
			}
			break;
		default:
			printcon("You got away this time!");
			return FLED;
		}
	}

	// Monster nearby...
	if (create_random_monsters()) {
		std::string nameWhoNoticedMonsters = noticed_monsters();
		if (nameWhoNoticedMonsters.length() > 0) {
			if (_party->is_resting) {
				printcon(nameWhoNoticedMonsters + (std::string)" heard some suspicious " +
						(std::string)"noise nearby. Do you wish to " +
						(std::string)"(i)nvestigate or (k)eep still?");

				if (_em->get_key("ik") == 'k') {
					printcon("Indeed, it seems it was nothing.");
					mwin.display_last();
					return NOT_INITIATED;
				}
			}
			else {
				printcon(nameWhoNoticedMonsters + (std::string)" heard some suspicious " +
						(std::string)"noise nearby. Do you wish to " +
						(std::string)"(i)nvestigate or (m)ove on?");

				if (_em->get_key("im") == 'm') {
					printcon("You got away this time!");
					mwin.display_last();
					return NOT_INITIATED;
				}
			}

			mwin.save_texture();
			mwin.display_texture(_foes.pic(mwin.get_renderer()));
			std::stringstream ss;
			ss << "Upon getting closer you spotted "
					<< _foes.to_string() << ".\n"
					<< "Do you wish to "
					<< (_foes.closest_range() > 10 ? "(a)dvance, " : "")
					<< "(f)ight or attempt to (s)neak away?";
			printcon(ss.str());

			switch (_em->get_key(_foes.closest_range() > 10 ? "afs" : "fs")) {
			case 'a':
				if (_foes.closest_range() > 10) {
					advance_party();
					foes_fight();
					return initiate();
				}
				break;
			case 'f':
				fight(attack_options());

				if (_foes.size() <= 0) {
					victory();
					return VICTORY;
				}
				else
					return initiate();
				break;
			default:
				printcon("You got away this time!");
				mwin.display_last();
				return FLED;
			}
		}
		// Monsters were not noticed by party first...
		else {
			mwin.save_texture();
			mwin.display_texture(_foes.pic(mwin.get_renderer()));
			std::stringstream ss;
			if (_party->is_resting)
				ss << "You suddenly find your camp surrounded by ";
			else
				ss << "Your stalwart party is brought to a halt by ";
			ss << _foes.to_string() << ".\n";
			ss << "Do you wish to ";
			ss << (_foes.closest_range() > 10 ? "(a)dvance, " : "");
			ss << "(f)ight or attempt to (r)un?";
			printcon(ss.str());

			switch (_em->get_key(_foes.closest_range() > 10 ? "afr" : "fr")) {
			case 'a':
				if (_foes.closest_range() > 10) {
					advance_party();
					foes_fight();
					return initiate();
				}
				break;
			case 'f':
				fight(attack_options());

				if (_foes.size() <= 0) {
					victory();
					return VICTORY;
				}
				else
					return initiate();
				break;
			default:
				printcon("You got away this time!");
				mwin.display_last();
				return FLED;
			}
		}
	}

	return OTHER;
}

// group refers to the n-th group of attackers.  There are always >= 1 groups.
// Returns a random first foe of the n-th group - this is always the default to be hit during an attack.

//int get_attacked_monster_from_group(int group)
//{
//	if (group == 1)
//		return 1;
//
//	std::string attacked_single_name = ""; // Store it just for fun, is not needed!  Maybe later?!
//	int j = 1;
//	for (auto foe : *(foes.count())) {
//		if (group == j)
//			attacked_single_name = foe.first;
//		j++;
//	}
//	printcon(player->name() + " will attack " + (Util::vowel(attacked_name[0]) ? "an " : "a ") + attacked_name + " in the next round.");
//	options[i]->set_target(attacked);
//}

std::vector<AttackOption*> Combat::attack_options()
{
	std::vector<AttackOption*> attackOptions;
	attackOptions.reserve(_party->size());
	attackOptions.resize(_party->size());

	for (int player_no = 0; player_no < _party->size(); player_no++) {
		PlayerCharacter* player = _party->get_player(player_no);

		if (player->is_npc())
			continue;

		std::string key_inputs = "adr";
		attackOptions[player_no] = NULL; // Initialise to NULL, in case player is dead and nothing else is set.

		if (player->condition() == DEAD)
			continue;

		ZtatsWin::Instance().highlight_lines(player_no * 2, player_no * 2 + 2);

		std::stringstream ss;
		ss << player->name() << " has these options this battle round:\n";
		ss << "(A)ttack foes with ";
		if (player->weapon() != NULL)
			ss << (Util::vowel(player->weapon()->name()[0])? "an " : "a ") << player->weapon()->name() << "\n";
		else
			ss << "bare hands.\n";
		if (player->is_spell_caster()) {
			ss << "(C)ast a spell.\n";
			key_inputs += 'c';
		}
		ss << "(D)efend.\n(R)eady item.";
		printcon(ss.str());
		ss.str("");

		char input = _em->get_key(key_inputs.c_str());

		if (input == 'a') {
			attackOptions[player_no] = new AttackOption(player_no, global_lua_state);

			if (_foes.amount()->size() == 1) {
				attackOptions[player_no]->set_target(1);
			}
			else {
				int attacked = select_enemy();

				string attacked_name;
				int j = 1;
				for (auto foe : *(_foes.amount())) {
					if (attacked == j)
						attacked_name = foe.first;
					j++;
				}
				printcon(player->name() + " will attack " + (Util::vowel(attacked_name[0]) ? "an " : "a ") + attacked_name + " in the next round.");
				attackOptions[player_no]->set_target(attacked);
			}
		}
		else if (input == 'c') {
			ZtatsWin::Instance().save_texture();
			std::string spell_file_path = GameControl::Instance().select_spell(player_no);

			if (spell_file_path == "") {
				printcon("Changed our mind in the last minute, didn't we?");
				attackOptions[player_no] = new DefendOption(player_no);
			}
			else {
				std::string defend_message = "";
				LuaWrapper lua(global_lua_state);
				Spell tmp_spell = Spell::spell_from_file_path(spell_file_path, global_lua_state);
				SpellCastHelper* spellCastHelper = new SpellCastHelper(player_no, global_lua_state);
				spellCastHelper->set_spell_path(spell_file_path);

				// Convert the this-pointer to string and push it to Lua-land
				// along with i, such that Lua knows which monster is referred
				// to.  (I know, this is very crazy code, but life is crazy.)
				// Then, the Lua spell code can refer to foes in the combat object,
				// for example.
				//
				// The workflow is as follows: first, we send the pointer as string to Lua,
				// then Lua sends it back to luaapi.cc, so that within luaapi.cc the according
				// combat actions can be triggered.

				std::ostringstream thiss;
				thiss << (void const *)this;
				lua.push_fn_arg((std::string)thiss.str());
				lua.call_void_fn("set_combat_ptr");

				try {
					spellCastHelper->init();

					if (spellCastHelper->choose() >= 0) {
						printcon(player->name() + " will cast '" + tmp_spell.name + "' in the next round.");
						attackOptions[player_no] = spellCastHelper;
					}
					else {
						printcon("Changed our mind in the last minute, didn't we?");
						attackOptions[player_no] = new DefendOption(player_no);
						delete spellCastHelper;
					}
				}
				// This exception is thrown, e.g., when a light spell is chosen, where no target or party member needs to be selected.
				catch (NoChooseFunctionException &ex) {
					printcon(player->name() + " will cast '" + tmp_spell.name + "' in the next round.");
					attackOptions[player_no] = spellCastHelper;
				}
				// This will be thrown, e.g., when the caster does not have enough spell points
				catch (SpellNotEnabledException &ex) {
					DefendOption* tmp_defend_option = new DefendOption(player_no);
					printcon("Not enough magic points. " + player->name() + " will instead defend in the next round.");
					attackOptions[player_no] = tmp_defend_option;
				}
			}
		}
		else if (input == 'd') {
			printcon(player->name() + " will defend in the next round.");
			attackOptions[player_no] = new DefendOption(player_no);
		}
		else { // (R)eady item
			std::string new_weapon = GameControl::Instance().keypress_ready_item(player_no);

			if (new_weapon != "") {
				ss << player->name() + " will ready a" << (Util::vowel(new_weapon[0])? "an " : "a ") << new_weapon << " in the next round.";
				printcon(ss.str());
			}
			else
				printcon(player->name() + " will defend in the next round.");

			attackOptions[player_no] = new DefendOption(player_no);
		}
		// This is a terrible hack: I quickly put the players back so that the "hacked" unhighlight lines
		// puts back the correct texture, the players list, not the spells list.
		// I need to do this so long as I can't get unhighlight/transparency to work properly.
		ZtatsWin::Instance().update_player_list();
		ZtatsWin::Instance().highlight_lines(player_no * 2, player_no * 2 + 2);
		ZtatsWin::Instance().unhighlight_lines(player_no * 2, player_no * 2 + 2);
	}

	return attackOptions;
}

int Combat::fight(std::vector<AttackOption*> attack_commands)
{
	party_fight(attack_commands);
	foes_fight();

	for (auto ac: attack_commands) {
		if (ac != NULL) { // Could be NULL in the array, if a party member is dead and has no attack_command set.
			std::cout << "INFO: combat:cc: Deleting party attack/defend command for player " << ac->attacking_player()->name() << ".\n";
			delete ac;
		}
	}

	return 0;
}

void Combat::add_to_bounty(Item* i)
{
	if (i != NULL)
		_bounty_items.add(i);
	else
		std::cout << "INFO: combat.cc: I just tried to add NULL to bounty. Did the attacker have a weapon? If so, this is an error. Otherwise it's OK.\n";
}

// Returns number of monsters left after a round of melee has taken
// place, also handles the actual melee itself after the characters
// have chosen their respective actions for this round.

int Combat::party_fight(std::vector<AttackOption*> attacks)
{
	if ((int)attacks.size() < _party->size())
		std::cerr << "ERROR: combat.cc: Attack choices < party size. This is serious.\n";

	// The party's moves...
	int i = 0;
	for (auto player = _party->begin(); player != _party->end(); i++, player++) {
		if (player->condition() == DEAD)
			continue;

		if (player->is_npc() && player->condition() != PlayerCondition::DEAD) {
			AttackOption ao(i, global_lua_state);
			ao.set_target(1);
			ao.execute(this);
		}
		else
			attacks[i]->execute(this);
	}

	return 0;
}

void Combat::victory()
{
	MiniWin& mwin = MiniWin::Instance();

	printcon("Your party emerged victorious!", true);
	mwin.display_last();

	if (_bounty_items.size() > 0) {
		ZtatsWin& zwin = ZtatsWin::Instance();
		printcon("As the dust of battle lifts, some items are left over. Dost thou wish to pick them up? (y/n)", true);

		switch (_em->get_key("yn")) {
		case 'y': {
			mwin.save_texture();
			mwin.clear();
			mwin.println(0, "Pick up items", CENTERALIGN);
			mwin.println(1, "(Press space to select an item, q when done)", CENTERALIGN);

			std::shared_ptr<ZtatsWinContentSelectionProvider<Item*>> content_selection_provider = _bounty_items.create_content_selection_provider(InventoryType::Anything);
			std::vector<Item*> selected_items = zwin.execute(content_selection_provider.get(), SelectionMode::MultipleItems);
			std::vector<Item*> not_selected_items;

			for (auto item: selected_items) {
				// Determine how many there are in the bounty of the current item...
				for (unsigned i = 0; i < _bounty_items.how_many_of(item->name(), item->description()); i++) {
					_party->inventory()->add(item);
				}
			}

			GameControl::Instance().redraw_graphics_status();
			mwin.display_last();

			// Finally, delete those items from bounty, which have NOT been picked up, otherwise, they will linger around in the heap somewhere...
			// This is somewhat tricky, as we first must build the difference btw. bounty and the inventory and then delete the memory of said difference.
			std::vector<Item*> tmp_bounty_items = _bounty_items.raw_items();

			std::sort(tmp_bounty_items.begin(), tmp_bounty_items.end());
			std::sort(selected_items.begin(), selected_items.end());

			// This will append to difference those elements found in tmp_bounty_items that are not found in selected_items::
			std::vector<Item*> difference;
			std::set_difference(
			    tmp_bounty_items.begin(), tmp_bounty_items.end(),
			    selected_items.begin(), selected_items.end(),
			    std::back_inserter(difference)
			);

			// Now delete the set difference, i.e., the left-behind items from combat...  The ones taken must not be deleted, as they are now in the inventory.
			for (unsigned i = 0; i < difference.size(); i++) {
				Item* to_delete_item = difference[i];
				std::cout << "INFO: combat.cc: Deleting left-behind item " << to_delete_item->name() << ".\n";
				delete to_delete_item;
			}

			break;
		}
		case 'n':
			// Delete memory for items from bounty
			for (unsigned i = 0; i <_bounty_items.size(); i++)
				delete _bounty_items.get_item(i);

			return;
		}
	}
}

/// Foes fight now against the party...

int Combat::foes_fight()
{
	LuaWrapper lua(global_lua_state);
	boost::unordered_set<std::string> moved;

	for (int i = 0; i < _foes.size(); i++) {
		boost::filesystem::path beast_path(conf_world_path);
		beast_path /= "bestiary";
		moved.clear();

		try {
			Creature* foe = _foes.get()->at(i).get();
			// This can throw an exception (see below!) If it doesn't, it means
			// we're dealing with a random outdoor monster from bestiary/.
			// Otherwise, we're dealing with town folk.
			beast_path /= World::Instance().get_monster_filename(foe->name());

			// Convert the this-pointer to string and push it to Lua-Land
			// along with i, such that Lua knows which monster is referred
			// to.  (I know, this is very crazy code, but life is crazy.)
			std::ostringstream thiss;
			thiss << (void const *)this;

			lua.push_fn_arg((double)i);
			lua.push_fn_arg((std::string)thiss.str());
			lua.call_void_fn(std::vector<std::string> { "Bestiary", foe->name(), "set_combat_ptr" });

			lua.push_fn_arg((double)foe->gold());
			lua.set_item_prop(std::vector<std::string> { "Bestiary", foe->name(), "gold" });

			// There are monsters, such as spiders, that don't have a weapon!
			if (foe->weapon() != NULL) {
				lua.push_fn_arg((std::string)foe->weapon()->name());
				lua.set_item_prop(std::vector<std::string> { "Bestiary", foe->name(), "weapon_name" });
			}

			lua.push_fn_arg((double)foe->luck());
			lua.set_item_prop(std::vector<std::string> { "Bestiary", foe->name(), "luck" });

			lua.push_fn_arg((double)foe->dxt());
			lua.set_item_prop(std::vector<std::string> { "Bestiary", foe->name(), "dxt" });

			lua.push_fn_arg((double)foe->hp());
			lua.set_item_prop(std::vector<std::string> { "Bestiary", foe->name(), "hp" });

			lua.push_fn_arg((double)foe->hpm());
			lua.set_item_prop(std::vector<std::string> { "Bestiary", foe->name(), "hp_max" });

			lua.push_fn_arg((double)foe->distance());
			lua.set_item_prop(std::vector<std::string> { "Bestiary", foe->name(), "distance" });

			if (lua.get_item_prop<bool>(std::vector<std::string> { "Bestiary", foe->name(), "advance" }) &&                // Foes want to advance? - as opposed to, say, fight from the distance
					moved.find(foe->name()) == moved.end())    // Foes haven't yet advanced in this round?
				moved = advance_foes();                        // Attempt to movej

			// If not moved and not fled, attack!
			if (moved.find(foe->name()) == moved.end() && !_fled) {
				lua.call_void_fn(std::vector<std::string> { "Bestiary", foe->name(), "attack" });
			}

			// All of the party died of the attack?
			if (_party->party_alive() == 0) {
				GameControl::Instance().game_over();
				return 0;
			}

			if (_fled)
				i--;
			_fled = false;
		} catch (const runtime_error& err) {
			std::cout << "INFO: combat.cc::foes_fight(): Couldn't execute Lua fight-file, "
					  << "assuming instead that we're fighting with some town folk in an indoors map: '"
					  << err.what() << "'\n";

			if (lua.call_fn<bool>("attack") && !_fled)
				lua.call_void_fn("fight");

			// All of the party died of the attack?
			if (_party->party_alive() == 0) {
				GameControl::Instance().game_over();
				return 0;
			}
		} catch (...) {
			std::cerr << "ERROR: combat.cc: foes_fight(): unforeseen exception.  In all likelyhood, "
					  << "Lua attack() function is buggered somewhere...\n";
			// TODO: When Boost > 1.65, this will be useful! std::cout << boost::stacktrace::stacktrace();
			// Alternatively, comment out entire catch clause and get stacktrace via gdb.
			return 0;
		}

		ZtatsWin::Instance().update_player_list();
	}

	return 0;
}

/// Returns which monster is going to be attacked in the next round by player player_no.
///
/// TODO: Make this as nice as select_player in ztats window!

int Combat::select_enemy()
{
	std::stringstream options;
	int i = 0;

	for (auto foe : *(_foes.amount())) {
		options << ++i;
		std::stringstream ss;
		ss << i << ") " << foe.second << " ";
		if (foe.second > 1)
			ss << _foes.get_plural_name(foe.first);
		else
			ss << foe.first;
		ss << " (" << _foes.get_distance(foe.first) << "')";
		printcon(ss.str());
	}

	char pressed = _em->get_key(options.str().c_str());
	return atoi(&pressed);
}

/// Makes the n-th foe in vector foes try to leave combat without a fight.
/// If it was the last foe of a group, the group disappears (e.g., group consisting of 1 Orc only).
/// If it was the last foe in combat, the combad ends.

void Combat::flee_foe(int n)
{
  Creature* foe = _foes.get()->at(n).get();
  std::stringstream ss;
  if (_foes.amount()->at(foe->name()) == 1)
    ss << "The " << foe->name() << " flees.";
  else
    ss << "A" << (Util::vowel(foe->name()[0])? "n " : " ")  << foe->name() << " flees.";
  printcon(ss.str(), true);

  std::cout << "INFO: combat.cc: About to remove " << n << "-th foe\n";
  _foes.remove(n);
  _fled = true;
}

/// Called advance party, but really decreases monsters' distances.

void Combat::advance_party()
{
  for (auto itr = _foes.begin(); itr != _foes.end(); itr++) {
    if ((*itr)->distance() > 10)
      (*itr)->set_distance((*itr)->distance() - 10);
  }
  printcon("Your party advances.", true);
}

Attackers& Combat::get_foes()
{
	return _foes;
}

void Combat::set_foes(Attackers new_foes)
{
	_foes = new_foes;
}

/// Advances the foes by 10' and returns the name of the foes that have moved.

boost::unordered_set<std::string> Combat::advance_foes()
{
  boost::unordered_set<std::string> moved;
  boost::unordered_map<int, std::string> distances = _foes.distances();
  for (auto foe = _foes.begin(); foe != _foes.end(); foe++) {
    if ((*foe)->distance() >= 20 && moved.find((*foe)->name()) == moved.end()) {
      try {
        // See if foe can move backwards or if this slot is blocked by other foes.
        // If there is no one there, then move (i.e., execute catch block).
        distances.at((*foe)->distance() - 10);
      }
      catch (std::out_of_range& oor) {
        _foes.move((*foe)->name().c_str(), -10);
        moved.insert((*foe)->name());
        distances = _foes.distances();
        if (_foes.amount()->at((*foe)->name()) == 1)
          printcon("The " + (*foe)->name() + " advances.", true);
        else
          printcon("The " + (*foe)->plural_name() + " advance.", true);
      }
    }
  }
  return moved;
}

/// Used for town folk, when the player attacks, e.g. a guard or runs amok otherwise.
/// Then we need to initiate battle, too, but create the "monsters", so to speak, rather
/// differently.  Script_path is the path to the Lua script defining the person in the town.

bool Combat::create_monsters_from_init_path(std::string script_file)
{
	LuaWrapper lua(global_lua_state);

	// Load corresponding Lua conversation file
	if (luaL_dofile(global_lua_state, (conf_world_path / boost::algorithm::to_lower_copy(script_file)).c_str())) {
		std::cerr << "ERROR: combat.cc::create_monsters_from_init_path(): Couldn't execute Lua file: " << lua_tostring(global_lua_state, -1) << std::endl;
		exit(EXIT_FAILURE);
	}

	// Load generic Lua script for fighting which is referenced by every town folk
	// Town folk don't have dedicated combat functions as it's not the norm that the party will attack town people.
	lua.push_fn_arg((std::string)((conf_world_path / "people" / "generic_fight.lua").c_str()));
	lua.call_void_fn(std::vector<std::string> { "load_generic_fight_file" });
    std::shared_ptr<GameCharacter> foe = std::static_pointer_cast<GameCharacter>(create_character_values_from_lua(global_lua_state));

    // Turn GameCharacter, foe, into Creature, creature; that is, enrich foe with the missing informations.
    Creature creature(*(foe.get()));
    creature.set_img(lua.call_fn<std::string>("img_path"));
	creature.set_distance(10);

    _foes.amount()->insert(std::make_pair(foe->name(), 1));
    _foes.add(std::make_shared<Creature>(creature));

	return true;
}

/// Usually used for monsters inside dungeons...

bool Combat::create_monsters_from_combat_path(std::string script_file)
{
	// Load corresponding Lua combat file (i.e., a monster definition)
	std::string monster_file_path = (conf_world_path / boost::algorithm::to_lower_copy(script_file)).string();
	if (luaL_dofile(global_lua_state, monster_file_path.c_str())) {
		std::cerr << "ERROR: combat.cc::create_monsters_from_combat_path(): Couldn't execute Lua file: " << lua_tostring(global_lua_state, -1) << std::endl;
		exit(EXIT_FAILURE);
	}

	// TODO: here is some code missing to create a proper set of foes,
	// consisting of different kinds of enemies, rather than just n of
	// the same!

	int number_of_enemies = random(1, 6);
	int distance = random(1,3) * 10;

	// Determine from PATH/bestiary/monster.lua the monster.lua bit and use it to get monster name property later.
	boost::regex expr {"[^\\/]+\\.lua$"};
	boost::smatch what;
	std::string monster_file_name;

	if (boost::regex_search(monster_file_path, what, expr)) {
		monster_file_name = what[0];
	}
	else {
		std::cerr << "ERROR: combat.cc: Monster file name for '" << monster_file_path << "' could not be determined.\n";
		return false;
	}

	std::string name = World::Instance().get_monster_name(monster_file_name);
	LuaWrapper lua(global_lua_state);

	// Create lua instance of monster and set further params
	lua.call_void_fn(std::vector<std::string> { "Bestiary", name, "create_instance" });

	for (int i = 0; i < number_of_enemies; i++) {
		std::shared_ptr<Creature> monster(new Creature());

		// Set distance
		monster->set_distance(distance);
		lua.push_fn_arg((double)distance);
		lua.set_item_prop(std::vector<std::string> { "Bestiary", name, "distance" });

		// Set rest of params
		monster->set_name(name);
		monster->set_img(lua.get_item_prop<std::string>(std::vector<std::string> { "Bestiary", name, "img_path" }));
		monster->set_plural_name(lua.get_item_prop<std::string>(std::vector<std::string> { "Bestiary", name, "plural_name" }));
		monster->set_hp(lua.get_item_prop<double>(std::vector<std::string> { "Bestiary", name, "hp" }));
		monster->set_hpm(lua.get_item_prop<double>(std::vector<std::string> { "Bestiary", name, "hp_max" }));
		monster->set_str(lua.get_item_prop<double>(std::vector<std::string> { "Bestiary", name, "strength" }));
		monster->set_luck(lua.get_item_prop<double>(std::vector<std::string> { "Bestiary", name, "luck" }));
		monster->set_gold(lua.get_item_prop<double>(std::vector<std::string> { "Bestiary", name, "gold" }));

		if (!lua.check_item_prop_is_nilornone(std::vector<std::string> { "Bestiary", name, "weapon" })) {
			std::string weapon_name = lua.get_item_prop<std::string>(std::vector<std::string> { "Bestiary", name, "weapon", "name" });
			Weapon* wep = WeaponHelper::createFromLua(weapon_name, global_lua_state);
			monster->set_weapon(wep);
		}

		// Add monster to rooster of attackers
		_foes.add(monster);
	}

    _foes.amount()->insert(std::make_pair(name, number_of_enemies));

	return true;
}

bool Combat::create_random_monsters()
{
	LuaWrapper lua(global_lua_state);
	std::pair<int,int> coords = _party->get_coords();
	int icon_no = GameControl::Instance().get_arena()->get_map()->get_tile(coords.first, coords.second);
	std::string icon_descr = "plain"; // Useless default icon description/name

	if (GameControl::Instance().get_arena()->get_map()->is_outdoors())
		icon_descr = OutdoorsIcons::Instance().get_props(icon_no)->get_name();
	else
		icon_descr = IndoorsIcons::Instance().get_props(icon_no)->get_name();

	int stack_size_before_result = lua_gettop(global_lua_state);

	lua.push_fn_arg(icon_descr); // The type of terrain the party is on...
	lua.call_fn_leave_ret_alone(std::vector<std::string> { "rand_encounter" }); // leave_ret_alone because the return type is complex and needs manual dissection

	int monsterDistance = -1;

	// Iterate through result table
	lua_pushnil(global_lua_state);
	while (lua_next(global_lua_state, -2) != 0) {
		std::string __name = "";
		int __number = -1;

		lua_pushnil(global_lua_state);
		while (lua_next(global_lua_state, -2) != 0) {
			std::string __key = lua_tostring(global_lua_state, -2);

			// Name of monster
			if (__key == "__name") {
				__name = lua_tostring(global_lua_state, -1);
			}
			// Distance, we only determine one and then derive the others below
			else if (__key == "__distance") {
				if (monsterDistance < 0) {
					if (lua_tonumber(global_lua_state, -1) > 0)
						monsterDistance = lua_tonumber(global_lua_state, -1);
					else
						std::cerr << "ERROR: combat.cc: Lua error: " << lua_tostring(global_lua_state, -1) << std::endl;
				}
			} // How many of the monster attack?
			else if (__key == "__number") {
				__number = lua_tonumber(global_lua_state, -1);
			}
			else {
				std::cerr << "ERROR: combat.cc: Did you fiddle with the bestiary/defs.lua file?\n";
				std::exit(EXIT_FAILURE);
			}

			// If everything was set, add monster to datastructure...
			if (__name.length() > 0 && __number >= 0) {
				_foes.amount()->insert(std::make_pair(Util::capitalise_first_letter(__name), __number));
				// std::cout << "Inserting: " << __name << ": " << __number << "\n";
			}

			lua_pop(global_lua_state, 1);
		}

		lua_pop(global_lua_state, 1);
	}
	lua_pop(global_lua_state, 1);

	// TODO: In production code, such Lua stack size checks should be unnecessary. For now, keep!  (See also, below!)
	if (lua_gettop(global_lua_state) != stack_size_before_result) {
		int stack_difference = std::abs(std::abs(stack_size_before_result) - std::abs(lua_gettop(global_lua_state)));
		std::cout << "WARNING: combat.cc: create_random_monsters(): Due to incorrect Lua stack size, popping "
				  << stack_difference << " off the Lua stack.\n";
		lua_pop(global_lua_state, stack_difference);
	}

	// Remove all occurrences of monsters with count 0.  These may be returned by the Lua
	// defs file for bookkeeping reasons, but we should eliminate them before we create
	// monster instances.
	for (auto foe = _foes.amount()->begin(); foe != _foes.amount()->end(); foe++) {
		std::string monsterName = foe->first;
		int count = foe->second;

		if (count == 0) {
			foe = _foes.amount()->erase(foe);
		}
	}

	// Now populate foe data structure with some further monster stats
	// according to the individual lua monster definitions

	// TODO: The way we iterate through the monsters means they always
	// appear in the same order when attacking in groups
	for (auto foe = _foes.amount()->begin(); foe != _foes.amount()->end(); foe++, monsterDistance += 10) {
		std::string monsterName = foe->first;
		int count = foe->second;

		// Create count many monsters named name, respectively
		for (int i = 0; i < count; i++) {
			std::shared_ptr<Creature> monster(new Creature());

			// Load corresponding Lua monster definition
			boost::filesystem::path beast_path(conf_world_path / "bestiary");
			try {
				beast_path /= World::Instance().get_monster_filename(monsterName);
			} catch (const runtime_error& error) {
				// TODO: The following is an ugly work around for
				//   terminate called after throwing an instance of 'std::runtime_error'
				//   what():  : no such monster name known.
				// But because this bug is difficult to reproduce, will keep this for now.
				// Could be that this happens, when called with an already corrupted Lua state above...
				// Try out if we can't use a fresh Lua state for this method instead.
				std::cerr << "combat.cc: create_random_monsters: get_monster_filename() for '" << monsterName
						<< "' failed. (This is serious and could mean the Lua stack is corrupted. "
						<< "You probably want to save the game state and reload it as the game could crash soon.)\n";
				return false;
			}

			int stack_size_before_monster_def = lua_gettop(global_lua_state);

			if (luaL_dofile(global_lua_state, beast_path.c_str())) {
				std::cerr << "ERROR: combat.cc::create_random_monsters(): Couldn't execute Lua file "
						  << beast_path << ": "
						  << lua_tostring(global_lua_state, -1) << std::endl;
				std::exit(EXIT_FAILURE);
			}
			else
				std::cout << "INFO: combat.cc: loaded " << beast_path.c_str() << ".\n";

			int stack_size_after_monster_def = lua_gettop(global_lua_state);

			// Set distance
			monster->set_distance(monsterDistance);
			lua.push_fn_arg((double)monsterDistance);
			lua.set_item_prop(std::vector<std::string> { "Bestiary", monsterName, "distance" });

			// Set rest of params
			monster->set_name(lua.get_item_prop<std::string>(std::vector<std::string> { "Bestiary", monsterName, "name" }));
			monster->set_plural_name(lua.get_item_prop<std::string>(std::vector<std::string> { "Bestiary", monsterName, "plural_name" }));

			// Create lua instance of monster and set further params
			lua.call_void_fn(std::vector<std::string> { "Bestiary", monsterName, "create_instance" });
			monster->set_hp(lua.get_item_prop<double>(std::vector<std::string> { "Bestiary", monsterName, "hp" }));
			monster->set_hpm(lua.get_item_prop<double>(std::vector<std::string> { "Bestiary", monsterName, "hp_max" }));
			monster->set_str(lua.get_item_prop<double>(std::vector<std::string> { "Bestiary", monsterName, "strength" }));
			monster->set_luck(lua.get_item_prop<double>(std::vector<std::string> { "Bestiary", monsterName, "luck" }));
			monster->set_gold(lua.get_item_prop<double>(std::vector<std::string> { "Bestiary", monsterName, "gold" }));
			monster->set_img(lua.get_item_prop<std::string>(std::vector<std::string> { "Bestiary", monsterName, "img_path" }));

			// Check if monster has weapon
			if (!lua.check_item_prop_is_nilornone(std::vector<std::string> { "Bestiary", monsterName, "weapon" })) {
				Weapon* wep = WeaponHelper::createFromLua(lua.get_item_prop<std::string>(std::vector<std::string> { "Bestiary", monsterName, "weapon", "name" }), global_lua_state);
				monster->set_weapon(wep);
			}

			// Now, for sanity, we do some Lua stack size checking...
			// TODO: This should not be necessary and can later be removed.
			if (lua_gettop(global_lua_state) != stack_size_after_monster_def)
				std::cerr << "WARNING: combat.cc: create_random_monsters() changed Lua stack by "
						  << std::abs(std::abs(lua_gettop(global_lua_state)) - std::abs(stack_size_after_monster_def)) << ".\n";

			int stack_difference = std::abs(std::abs(stack_size_after_monster_def) - std::abs(stack_size_before_monster_def));
			if (stack_difference != 0) {
				std::cout << "INFO: combat.cc: create_random_monsters(): popping " << stack_difference << " off the Lua stack.\n";
				lua_pop(global_lua_state, stack_difference);
			}

			// Add monster to rooster of attackers
			_foes.add(monster);
		}
	}

	return _foes.amount()->size() > 0;
}

/// Returns the name of a player, if the player noticed nearby monsters
/// (before they noticed him).  Otherwise an empty string is returned.

std::string Combat::noticed_monsters()
{
	// Determine who sees the other first, try each player, individually.
	std::string name = "";

	// If a dog is in the party, it will ALWAYS hear the enemy first - obviously.
	PlayerCharacter* npc = _party->get_npc_or_null();
	if (npc != NULL && npc->race() == RACE::DOG && npc->condition() != PlayerCondition::DEAD)
		return "Your good dog " + npc->name();

	// When resting only that player may notice monsters...
	if (_party->is_resting) {
		PlayerCharacter* p = _party->get_guard();

		if (p == NULL)
			return name;

		if (p->condition() != DEAD && GameRules::bonus(p->luck()) + random(1, 12) >= 9)
			name = p->name();

		return name;
	}
	// When not resting, we can try whole party...
	else {
		for (auto curr_player = _party->begin(); curr_player != _party->end(); curr_player++) {
			if (curr_player->condition() == DEAD)
				continue;

			if (GameRules::bonus(curr_player->luck()) + random(1, 12) >= 9) {
				name = curr_player->name();
				break;
			}
		}
	}
	return name;
}

int Combat::random(int min, int max)
{
  return GameControl::Instance().random(min, max);
}

void Combat::printcon(const std::string s, bool wait)
{
	Console::Instance().print(&Charset::Instance(), s, wait);
}
