// This source file is part of Simplicissimus
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

#include "simplicissimus.hh"
#include "gamestate.hh"
#include "map.hh"
#include "world.hh"
#include "party.hh"

#include <vector>
#include <iostream>
#include <fstream>
#include <memory>

#include <boost/filesystem.hpp>

GameState::GameState()
{
	// TODO Auto-generated constructor stub
}

GameState::~GameState()
{
	// TODO Auto-generated destructor stub
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
			std::cerr << "Info: gamestate.cc: Erased old map " << (*b)->get_name() << " from current game state.\n";
			_maps.erase(b);
			break;
		}
	}

	std::cerr << "Info: gamestate.cc: Adding new map " << map->get_name() << " to current game state.\n";
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

	return true;
}

bool GameState::save()
{
	boost::filesystem::path dir = conf_savegame_path;

	// Create output file structure if need be
	if (!boost::filesystem::create_directory(dir))
		std::cerr << "ERROR: gamestate.cc: Could not create config directory: " << dir << std::endl;

	if (!boost::filesystem::create_directory(dir / World::Instance().get_name()))
		std::cerr << "ERROR: gamestate.cc: Could not create config directory: " << (dir / World::Instance().get_name()).string() << std::endl;

	if (!boost::filesystem::create_directory(dir / World::Instance().get_name() / "maps"))
		std::cerr << "ERROR: gamestate.cc: Could not create config directory: " << (dir / World::Instance().get_name() / "maps").string() << std::endl;

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

	// Clear map state again
	_maps.clear();

	// Save world map
	std::string world_file = dir.string() + World::Instance().get_name() + ".xml";
	if (boost::filesystem::exists(world_file)) {
		std::cout << "INFO: gamestate.cc: Removing file " << world_file << ".\n";
		boost::filesystem::remove(world_file);
	}

	std::cout << "INFO: gamestate.cc: Writing " << world_file << ".\n";
	World::Instance().xml_write_world_data(dir.string());
	std::cout << "INFO: gamestate.cc: Written.\n";

	return true;
}

bool GameState::load()
{
	return true;
}
