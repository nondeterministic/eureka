// This source file is part of eureka
//
// Copyright (c) 2007-2018 Andreas Bauer <baueran@gmail.com>
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

#include "eureka.hh"
#include "gamestate.hh"
#include "map.hh"
#include "world.hh"
#include "party.hh"
#include "gamecharacter.hh"
#include "gamecontrol.hh"
#include "ztatswin.hh"
#include "itemfactory.hh"
#include "weaponhelper.hh"
#include "shieldhelper.hh"
#include "armourhelper.hh"

#include <vector>
#include <iostream>
#include <fstream>
#include <memory>
#include <cstdlib>

#include <lua.h>
#include <lualib.h>

#include <boost/filesystem.hpp>

GameState::GameState()
{
	_cur_outdoors = true;
	_cur_map_name = "";
	_jimmylocks = 0;
	_gold = 0;
	_food = 0;
	_x = 0;
	_y = 0;
}

GameState::~GameState()
{
}

GameState& GameState::Instance()
{
	static GameState _inst;
	return _inst;
}

void GameState::add_map(std::shared_ptr<IndoorsMap> map)
{
	// First delete map, if already exists in game state
	for (auto b = _maps.begin(); b != _maps.end(); b++) {
		if ((*b)->get_name() == map->get_name()) {
			std::cout << "INFO: gamestate.cc: Erased old map " << (*b)->get_name() << " from current game state.\n";
			_maps.erase(b);
			break;
		}
	}

	std::cout << "INFO: gamestate.cc: Adding new map " << map->get_name() << " to current game state.\n";
	_maps.push_back(map);
}

std::shared_ptr<IndoorsMap> GameState::get_map(std::string map_name)
{
	for (auto map: _maps) {
		if (map->get_name() == map_name)
			return map;
	}

	return NULL;
}

// protected, called by save()

bool GameState::save_party(boost::filesystem::path fullFilePath)
{
	boost::filesystem::path the_file = fullFilePath / "party.xml";
	std::string party_xml = Party::Instance().to_xml();

	if (boost::filesystem::exists(the_file)) {
		std::cout << "INFO: gamestate.cc: Removing file " << the_file.c_str() << ".\n";
		boost::filesystem::remove(the_file);
	}

	std::cout << "INFO: gamestate.cc: Writing party data to " << the_file.string() << std::endl;
    std::ofstream out;
    out.open(the_file.string());
    out << party_xml;
    out.close();
	std::cout << "INFO: gamestate.cc: Written.\n";

	return true;
}

// protected, called by save()

bool GameState::save_misc(boost::filesystem::path fullFilePath)
{
	boost::filesystem::path the_file = fullFilePath / "misc.xml";

	if (boost::filesystem::exists(the_file)) {
		std::cout << "INFO: gamestate.cc: Removing file " << the_file.c_str() << ".\n";
		boost::filesystem::remove(the_file);
	}

	// Create XML
	xmlpp::Document xml_doc;
	xmlpp::Element* partyNd = xml_doc.create_root_node("misc");
	xmlpp::Element* tod = partyNd->add_child("timeofday");
	tod->set_attribute("hour", std::to_string(GameControl::Instance().get_clock()->time().first));
	tod->set_attribute("minute", std::to_string(GameControl::Instance().get_clock()->time().second));

	// Write XML
	std::cout << "INFO: gamestate.cc: Writing misc information to " << the_file.string() << std::endl;
    std::ofstream out;
    out.open(the_file.string());
    out << xml_doc.write_to_string_formatted().c_str();
    out.close();
	std::cout << "INFO: gamestate.cc: Written.\n";

	return true;
}

bool GameState::save()
{
	boost::filesystem::path dir = conf_savegame_path;

	// Create output file structure if need be
	if (!boost::filesystem::create_directory(dir))
		std::cerr << "WARNING: gamestate.cc: Could not create config directory: " << dir << ". Might already exist." << std::endl;

	if (!boost::filesystem::create_directory(dir / World::Instance().get_name()))
		std::cerr << "WARNING: gamestate.cc: Could not create config directory: " << (dir / World::Instance().get_name()).string() << ". Might already exist." << std::endl;

	if (!boost::filesystem::create_directory(dir / World::Instance().get_name() / "maps"))
		std::cerr << "WARNING: gamestate.cc: Could not create config directory: " << (dir / World::Instance().get_name() / "maps").string() << ". Might already exist." << std::endl;

	// Store indoors maps
	for (auto map: _maps) {
		boost::filesystem::path file = dir / "maps" / (map->get_name() + ".xml");

		if (boost::filesystem::exists(file)) {
			std::cout << "INFO: gamestate.cc: gamestate.cc: Removing file " << file << ".\n";
			boost::filesystem::remove(file);
		}

		std::cout << "INFO: gamestate.cc: Writing " << file << ".\n";
		map->xml_write_map_data(dir);
		std::cout << "INFO: gamestate.cc: Written.\n";
	}

	// Store party state
	save_party(dir);

	// Save time of day and other misc. information, related to the current game state
	save_misc(dir);

	// Clear map state again
	_maps.clear();

	// Save world map
 	boost::filesystem::path world_file = dir / (World::Instance().get_name() + ".xml");
	if (boost::filesystem::exists(world_file)) {
		std::cout << "INFO: gamestate.cc: Removing file " << world_file << ".\n";
		boost::filesystem::remove(world_file);
	}

	std::cout << "INFO: gamestate.cc: Writing " << world_file << ".\n";
	World::Instance().xml_write_world_data(dir.string());
	std::cout << "INFO: gamestate.cc: Written.\n";

	return true;
}

// TODO: This was simply copied from eureka.cc. FIX!

bool GameState::load(lua_State* lua_state)
{
	// Check if there's a saved game to return to and load it, if there is.
	if (boost::filesystem::exists(conf_savegame_path)) {
		load_party(lua_state);
		load_misc(lua_state);
	}

	return true;
}

bool GameState::load_party(lua_State* lua_state)
{
	xmlpp::TextReader reader((conf_savegame_path / "party.xml").string());
	std::cout << "INFO: gamestate.cc: Loading game data from file " << (conf_savegame_path / "party.xml").string() << std::endl;

	while (reader.read()) {
		if (reader.get_node_type() != xmlpp::TextReader::xmlNodeType::EndElement && !reader.is_empty_element()) {
			if (reader.get_name() == "x")
				_x = std::stoi(reader.read_string());
			else if (reader.get_name() == "y")
				_y = std::stoi(reader.read_string());
			else if (reader.get_name() == "map")
				_cur_map_name = reader.read_string().c_str();
			else if (reader.get_name() == "indoors")
				_cur_outdoors = reader.read_string() == "0"? true : false;
			else if (reader.get_name() == "jimmylocks")
				_jimmylocks = std::stoi(reader.read_string().c_str());
			else if (reader.get_name() == "gold")
				_gold = std::stoi(reader.read_string().c_str());
			else if (reader.get_name() == "food")
				_food = std::stoi(reader.read_string().c_str());
			// Inventory
			else if (reader.get_name() == "inventory") {
				while (reader.read() && reader.get_name() != "inventory") {
					if (reader.get_node_type() != xmlpp::TextReader::xmlNodeType::EndElement && !reader.is_empty_element()) {
						if (reader.get_name() == "item") {
							reader.move_to_next_attribute();
							int how_many = std::atoi(reader.get_value().c_str());

							reader.move_to_element();
							std::string item_name = reader.read_string();
							if (item_name.length() == 0) {
								std::cerr << "WARNING: gamestate.cc: Skipping reading of inventory item due to empty description in XML-file.\n";
								continue;
							}
							std::string short_name = item_name.substr(item_name.find("::") + 2);

							for (int i = 0; i < how_many; i++)
								_inventory.add(ItemFactory::create(item_name));
						}
					}
				}
			}
			// Players
			else if (reader.get_name() == "players") {
				while (reader.read() && reader.get_name() != "players") {
					if (reader.get_name() == "player") {
						PlayerCharacter player;

						// Set player name from attribute
						reader.move_to_next_attribute();
						player.set_name(reader.get_value());

						// Parse properties of player until </player> tag is found
						while (reader.read() && reader.get_name() != "player") {
							if (reader.get_node_type() != xmlpp::TextReader::xmlNodeType::EndElement && !reader.is_empty_element()) {
								if (reader.get_name() == "profession") {
									std::string prof = reader.read_string();
									player.set_profession(stringToProfession.at(prof));
								}
								else if (reader.get_name() == "condition") {
									std::string cond = reader.read_string();
									player.set_condition(stringToPlayerCondition.at(cond));
								}
								else if (reader.get_name() == "ep")
									player.inc_ep(std::atoi(reader.read_string().c_str()));
								else if (reader.get_name() == "hp")
									player.set_hp(std::atoi(reader.read_string().c_str()));
								else if (reader.get_name() == "hpm")
									player.set_hpm(std::atoi(reader.read_string().c_str()));
								else if (reader.get_name() == "sp")
									player.set_sp(std::atoi(reader.read_string().c_str()));
								else if (reader.get_name() == "spm")
									player.set_spm(std::atoi(reader.read_string().c_str()));
								else if (reader.get_name() == "str")
									player.set_str(std::atoi(reader.read_string().c_str()));
								else if (reader.get_name() == "luck")
									player.set_luck(std::atoi(reader.read_string().c_str()));
								else if (reader.get_name() == "dxt")
									player.set_dxt(std::atoi(reader.read_string().c_str()));
								else if (reader.get_name() == "wis")
									player.set_wis(std::atoi(reader.read_string().c_str()));
								else if (reader.get_name() == "charr")
									player.set_char(std::atoi(reader.read_string().c_str()));
								else if (reader.get_name() == "iq")
									player.set_iq(std::atoi(reader.read_string().c_str()));
								else if (reader.get_name() == "end")
									player.set_end(std::atoi(reader.read_string().c_str()));
								else if (reader.get_name() == "sex") {
									int sex = std::atoi(reader.read_string().c_str())? 1 : 0;
									player.set_sex(sex);
								}
								else if (reader.get_name().lowercase() == "level")
									player.set_level_passively(std::atoi(reader.read_string().c_str()));
								else if (reader.get_name().lowercase() == "race")
									player.set_race(static_cast<RACE>(std::atoi(reader.read_string().c_str())));
								else if (reader.get_name().lowercase() == "weapon") {
									std::string weap_name = reader.read_string();
									std::string short_name = weap_name.substr(weap_name.find("::") + 2);
									player.set_weapon(WeaponHelper::createFromLua(short_name, lua_state));
								}
								else if (reader.get_name().lowercase() == "shield") {
									std::string shield_name = reader.read_string();
									std::string short_name = shield_name.substr(shield_name.find("::") + 2);
									player.set_shield(ShieldHelper::createFromLua(short_name, lua_state));
								}
								else if (reader.get_name().lowercase() == "armour") {
									std::string armour_name = reader.read_string();
									std::string short_name = armour_name.substr(armour_name.find("::") + 2);
									player.set_armour(ArmourHelper::createFromLua(short_name, lua_state));
								}
								else if (reader.get_name().lowercase() == "gloves") {
									std::string gloves_name = reader.read_string();
									std::string short_name = gloves_name.substr(gloves_name.find("::") + 2);
									player.set_armour_hands(ArmourHelper::createFromLua(short_name, lua_state));
								}
								else if (reader.get_name().lowercase() == "shoes") {
									std::string shoes_name = reader.read_string();
									std::string short_name = shoes_name.substr(shoes_name.find("::") + 2);
									player.set_armour_feet(ArmourHelper::createFromLua(short_name, lua_state));
								}
								else if (reader.get_name().lowercase() == "helmet") {
									std::string helmet_name = reader.read_string();
									std::string short_name = helmet_name.substr(helmet_name.find("::") + 2);
									player.set_armour_head(ArmourHelper::createFromLua(short_name, lua_state));
								}
							}
						} // player-while-end

						_players.push_back(player);
					}
				} // players-while-end
			} // players-else-if-end
		}
	}

	// ***************************************************************
	// TODO: Add maps to GameState object, update map in arena, etc.
	// ***************************************************************

	return true;
}

bool GameState::load_misc(lua_State* lua_state)
{
	xmlpp::TextReader reader((conf_savegame_path / "misc.xml").string());
	std::cout << "INFO: gamestate.cc: Loading misc. game data from file " << (conf_savegame_path / "misc.xml").string() << std::endl;

	while (reader.read()) {
		if (reader.get_node_type() != xmlpp::TextReader::xmlNodeType::EndElement) {
			if (reader.get_name() == "timeofday") {
				int hour   = std::atoi(reader.get_attribute("hour").c_str());
				int minute = std::atoi(reader.get_attribute("minute").c_str());
				GameControl::Instance().get_clock()->set(hour, minute);
			}
		}
	}

	return true;
}

std::string GameState::get_cur_map_name()
{
	return _cur_map_name;
}

bool GameState::is_cur_map_outdoors()
{
	return _cur_outdoors;
}

// This method should be called after loading a new game state from disk to make the loaded data effective.

bool GameState::apply()
{
	Party* party     = &Party::Instance();
	GameControl* gc  = &GameControl::Instance();
	ZtatsWin::Instance().update_player_list();

	for (auto const& player: _players)
		party->add_player(player);

	gc->set_map_name(_cur_map_name.c_str());
	gc->set_outdoors(_cur_outdoors);

	for (int l = 0; l < _jimmylocks; l++)
		party->add_jimmylock();

	party->set_gold(_gold);
	party->set_food(_food);
	party->inventory()->add_all(_inventory);
	party->set_coords(_x, _y);

	return true;
}

bool GameState::reset()
{
	_gold = 0;
	_food = 0;
	_x = 0;
	_y = 0;
	_jimmylocks = 0;
	_inventory.remove_all();
	_players.clear();
	_maps.clear(); // TODO: Do we need to destroy the shared_ptrs inside first?

	// Not sure if it matters to "reset" the following...
	// _cur_map_name = "";
	// _cur_outdoors = true;

	return true;
}
