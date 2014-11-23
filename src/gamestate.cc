/*
 * gamestate.cc
 *
 *  Created on: Nov 2, 2014
 *      Author: baueran
 */

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
			_maps.erase(b);
			break;
		}
	}

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

bool GameState::save_party(std::string fullFilePath)
{
	boost::filesystem::path the_file = fullFilePath + "party.xml";
	std::string party_xml = Party::Instance().to_xml();

	if (boost::filesystem::exists(the_file)) {
		std::cout << "gamestate.cc: Removing file " << the_file.c_str() << ".\n";
		boost::filesystem::remove(the_file);
	}

	std::cout << "Writring to " << the_file.string() << std::endl;
    std::ofstream out;
    out.open(the_file.string());
    out << party_xml;
    out.close();

	return true;
}

bool GameState::save(std::string fullFilePath)
{
	boost::filesystem::path dir; // (std::string(getenv("HOME")) + "/.simplicissimus/");  // TODO: Does this use of dir ensure platform-independence?

	if (fullFilePath == "")
		dir = std::string(getenv("HOME")) + "/.simplicissimus/";
	else
		dir = fullFilePath;

	// Create output file structure if need be
	if (!boost::filesystem::create_directory(dir))
		std::cout << "gamestate.cc: Could not create config directory: " << dir << std::endl;

	if (!boost::filesystem::create_directory(dir.string() + World::Instance().get_name()))
		std::cout << "gamestate.cc: Could not create config directory: " << (dir.string() + World::Instance().get_name()) << std::endl;

	if (!boost::filesystem::create_directory(dir.string() + World::Instance().get_name() + "/maps/"))
		std::cout << "gamestate.cc: Could not create config directory: " << (dir.string() + World::Instance().get_name() + "/maps/") << std::endl;

	// Store indoors maps
	for (auto map: _maps) {
		std::string file = dir.string() + "maps/" + map->get_name() + ".xml";

		if (boost::filesystem::exists(file)) {
			std::cout << "gamestate.cc: Removing file " << file << ".\n";
			boost::filesystem::remove(file);
		}

		std::cout << "gamestate.cc: Writing " << file << ".\n";
		map->xml_write_map_data(dir.string());
		std::cout << "gamestate.cc: Written.\n";
	}

	// Store party state
	save_party(dir.string());

	// Clear map state again
	_maps.clear();

	// Save world map
	std::string world_file = dir.string() + World::Instance().get_name() + ".xml";
	if (boost::filesystem::exists(world_file)) {
		std::cout << "gamestate.cc: Removing file " << world_file << ".\n";
		boost::filesystem::remove(world_file);
	}

	std::cout << "gamestate.cc: Writing " << world_file << ".\n";
	World::Instance().xml_write_world_data(dir.string());
	std::cout << "gamestate.cc: Written.\n";

	return true;
}

bool GameState::load(std::string fullFilePath)
{
	return true;
}
