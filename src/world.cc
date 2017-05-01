// leibniz
//
// Copyright (c) 2009  Andreas Bauer <baueran@gmail.com>
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

// See http://www.robertnitsch.de/notes/cpp/cpp11_boost_filesystem_undefined_reference_copy_file
#define BOOST_NO_SCOPED_ENUMS
#define BOOST_NO_CXX11_SCOPED_ENUMS

#include "boost/filesystem.hpp"
#include "boost/filesystem/path.hpp"
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

#include <libxml++/libxml++.h>
#include <string>
#include <sstream>
#include <iostream>
#include <cstdlib>

#include "world.hh"
#include "config.h"
#include "map.hh"
#include "indoorsmap.hh"
#include "outdoorsmap.hh"
#include "indoorsicons.hh"
#include "outdoorsicons.hh"
#include "creature.hh"
#include "luaapi.hh"
#include "weapon.hh"
#include "spell.hh"
#include "luawrapper.hh"

#ifndef EDITOR_COMPILE
#include "eureka.hh"
#else
#include "leibniz.hh"
#endif

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lualib.h>
#include <lauxlib.h>
}

// extern lua_State* _lua_state;

World::World()
{
    _indoors_tile_size = 32; // This is the length of a side of a
    // square.
    _outdoors_tile_size = 34; // This is the length of a side of the
    // hex.
    _main_world_xml_file = NULL;
    _main_world_xml_root = NULL;
}

World::~World()
{
    _map_names.clear();
}

World& World::Instance(void)
{
    static World inst;
    return inst;
}

std::string World::get_name(void)
{
    return _name;
}

boost::filesystem::path World::get_path(void)
{
    return _path;
}

void World::set_world_path(boost::filesystem::path new_path)
{
    _path = new_path;
}

void World::set_name(const char* new_name)
{
    _name = new_name;
}

bool World::add_map(std::shared_ptr<Map> new_map)
{
    try {
      _maps.push_back(new_map);
      //       // Only add name to vector if it ain't already in.
      //       bool is_new = true;
      //       for (std::vector<std::string>::iterator curr_map_n = _map_names.begin();
      // 	   curr_map_n != _map_names.end();
      // 	   curr_map_n++)
      // 	{
      // 	  if (*curr_map_n == new_map->get_name())
      // 	    {
      // 	      is_new = false;
      // 	      break;
      // 	    }
      // 	}
      //       if (is_new)
      //       	_map_names.push_back(new_map->get_name());
    }
    catch (const std::exception& e) {
      return false;
    }

    return true;
}

bool World::delete_map(std::shared_ptr<Map> the_map)
{
    if (the_map == NULL)
        return false;

    for (auto curr_map = _maps.begin(); curr_map != _maps.end(); curr_map++) {
        if (*curr_map == the_map) {
            _maps.erase(curr_map);
            // delete the_map;
            return true;
        }
    }

    std::cerr << "ERROR: world.cc:delete_map(): NOT DELETING! CHECK THIS!\n";
    return false;
}

std::shared_ptr<Map> World::get_map(const char* map_name)
{
    for (auto curr_map = _maps.begin(); curr_map != _maps.end(); curr_map++)
        if ((*curr_map)->get_name() == map_name)
            return *curr_map;
    throw MapNotFound("MapNotFound exception in World::get_map(" + (std::string)map_name + ")");
}

std::shared_ptr<Map> World::get_initial_map()
{
    for (auto curr_map = _maps.begin(); curr_map != _maps.end(); curr_map++)
        if ((*curr_map)->initial)
            return *curr_map;
    throw MapNotFound("MapNotFound exception in World::get_initial_map()");
}

std::vector<std::shared_ptr<Map>>* World::get_maps(void)
{
    return &_maps;
}

bool World::exists_map(const char* map_name)
{
    for (auto curr_map = _maps.begin(); curr_map != _maps.end(); curr_map++)
        if ((*curr_map)->get_name() == map_name)
            return true;
    return false;
}

unsigned World::get_indoors_tile_size(void) const
{
    return _indoors_tile_size;
}

unsigned World::get_outdoors_tile_size(void) const
{
    return _outdoors_tile_size;
}

void World::set_indoors_tile_size(unsigned new_ts)
{
    _indoors_tile_size = new_ts;
}

std::vector<Creature>* World::get_creatures()
{
    return &_creatures;
}

bool World::xml_load_world_data(const std::string filename)
{
	try {
    	xmlpp::TextReader reader(filename);

        // The reader.next() is necessary to skip the closing tags.
        // Don't know how else to do it.
        while (reader.read()) {
            if (reader.get_name() == "name") {
                _name = reader.read_string();
                boost::filesystem::path filepath(filename);
                _path = filepath.parent_path().string() + "/";
                reader.next();
            }
            else if (reader.get_name() == "map") {
                bool outdoors = false;
                bool initial = false;
            	std::string new_map_name = reader.read_string();
                std::string new_map_path;

                reader.move_to_first_attribute();
                do {
                    if (reader.get_name().uppercase() == "HREF") {
                        new_map_path = reader.get_value();
                    }
                    else if (reader.get_name().uppercase() == "TYPE") {
                    	if (reader.get_value().uppercase() == "OUTDOORS")
                    		outdoors = true;
                    }
                    else if (reader.get_name().uppercase() == "INITIAL") {
                        if (reader.get_value().uppercase() == "YES")
                        	initial = true;
                    }
                }
                while (reader.move_to_next_attribute());
                reader.move_to_element();

                // Create the actual map from read attributes
                std::shared_ptr<Map> new_map; //  = NULL;

                if (!outdoors)
                    new_map = std::make_shared<IndoorsMap>(new_map_name.c_str()); // , new_map_path.c_str());
                else
                    new_map = std::make_shared<OutdoorsMap>(new_map_name.c_str()); // , new_map_path.c_str());

                if (initial)
                	new_map->initial = true;

                new_map->set_notmodified();

                World::Instance().add_map(new_map);
                reader.next();
            }
            else if (reader.get_name() == "outdoors" || reader.get_name() == "indoors") {
                string parent_node = "indoors";
                if (reader.get_name() == "outdoors")
                    parent_node = "outdoors";

                // Read while </outdoors>/</indoors> isn't reached...
                while (reader.read() && reader.get_name() != parent_node) {
                    if (reader.get_name() == "icon") {
                        IconProps new_props;
                        new_props.set_name(reader.read_string().c_str());
                        do {
                            if (reader.get_name() == "no")
                                new_props.set_icon(atoi(reader.get_value().c_str()));
                            else if (reader.get_name() == "sound_effect") {
                                if (reader.get_value().length() > 0)
                                    new_props.set_sound_effect(reader.get_value().c_str());
                            }
                            else if (reader.get_name().uppercase() == "LIGHT_RADIUS") {
                                if (reader.get_value().length() > 0)
                                    new_props.set_light_radius(atoi(reader.get_value().c_str()));
                            }
                            else if (reader.get_name() == "next_anim") {
                                if (reader.get_value().length() > 0)
                                    new_props.set_next_anim(atoi(reader.get_value().c_str()));
                            }
                            else if (reader.get_name() == "trans") {
                                if (reader.get_value() == "full")
                                	new_props._trans = PropertyStrength::Full;
                                else if (reader.get_value() == "semi")
                                	new_props._trans = PropertyStrength::Some;
                                else if (reader.get_value() == "not")
                                	new_props._trans = PropertyStrength::None;
                            }
                            else if (reader.get_name().uppercase() == "DEFAULT_LUA_NAME") {
                            	new_props.set_default_lua_name(reader.get_value());
                            }
                            else if (reader.get_name() == "walk") {
                                if (reader.get_value() == "full")
                                	new_props._is_walkable = PropertyStrength::Full;
                                else if (reader.get_value() == "slow")
                                	new_props._is_walkable = PropertyStrength::Some;
                                else if (reader.get_value() == "not")
                                	new_props._is_walkable = PropertyStrength::None;
                            }
                            else if (reader.get_name() == "poison") {
                                if (reader.get_value() == "full")
                                	new_props._poisonous = PropertyStrength::Full;
                                else if (reader.get_value() == "semi")
                                	new_props._poisonous = PropertyStrength::Some;
                                else if (reader.get_value() == "not")
                                	new_props._poisonous = PropertyStrength::None;
                            }
                            else if (reader.get_name() == "magic_force_field") {
                                if (reader.get_value() == "full")
                                	new_props._magical_force_field = PropertyStrength::Full;
                                else if (reader.get_value() == "semi")
                                	new_props._magical_force_field = PropertyStrength::Some;
                                else if (reader.get_value() == "not")
                                	new_props._magical_force_field = PropertyStrength::None;
                            }
                        } while (reader.move_to_next_attribute());

                        if (parent_node == "outdoors")
                            OutdoorsIcons::Instance().add_props(new_props);
                        else
                            IndoorsIcons::Instance().add_props(new_props);

                        reader.move_to_element();
                        reader.next();
                    }
                }
            }
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Exception caught: " << ex.what() << std::endl;
        return false;
    }

    return true;
}

// Simulate default argument for xml_write_world_data.

void World::xml_write_world_data()
{
	boost::filesystem::path empty_path;
	xml_write_world_data(empty_path);
}

void World::xml_write_world_data(boost::filesystem::path path)
{
	try {
		_main_world_xml_file = new xmlpp::Document();

		// Add name
		_main_world_xml_root = _main_world_xml_file->create_root_node("world");
		xmlpp::Element* xml_world_name = _main_world_xml_root->add_child("name");
		xml_world_name->add_child_text(_name);

		// Add map information
		for (auto curr_map = _maps.begin(); curr_map != _maps.end(); curr_map++) {
			xmlpp::Element* map_node = _main_world_xml_root->add_child("map");
			map_node->set_attribute("href", "./" + _name + "/maps/" + (*curr_map)->get_name() + ".xml");
			map_node->set_attribute("type", ((*curr_map)->is_outdoors() ? "outdoors" : "indoors"));
			if ((*curr_map)->initial)
				map_node->set_attribute("initial", "yes");
			map_node->add_child_text((*curr_map)->get_name());
		}

		// Add icon data
		xmlpp::Element* iconprops_node = _main_world_xml_root->add_child("iconprops");

		// Indoors
		xmlpp::Element* indoorsicons_node = iconprops_node->add_child("indoors");
		for (unsigned i = 0; i < IndoorsIcons::Instance().number_of_icons(); i++) {
			xmlpp::Element* icon_node = indoorsicons_node->add_child("icon");
			std::stringstream icon_no_str;
			icon_no_str << i;

			IconProps* icon_props = IndoorsIcons::Instance().get_props(i);
			if (!icon_props) {
				std::cerr << "WARNING: world.cc: Skipping saving of icon " << i << " as I cannot get IconProps for it. Sort of serious.\n";
				continue;
			}

			if (icon_props->light_radius() > 0)
				icon_node->set_attribute("light_radius", std::to_string(icon_props->light_radius()));

			std::string sound_effect = icon_props->sound_effect();
			int next_anim = icon_props->next_anim();
			std::stringstream next_anim_str;
			next_anim_str << next_anim;

			icon_node->set_attribute("no", icon_no_str.str());

			set_icon_attributes(icon_node, icon_props);

			if (next_anim >= 0)
				icon_node->set_attribute("next_anim", next_anim_str.str());

			if (sound_effect.size() > 0)
				icon_node->set_attribute("sound_effect", sound_effect);

			if (icon_props->default_lua_name().length() > 0)
				icon_node->set_attribute("default_lua_name", icon_props->default_lua_name());

			icon_node->add_child_text(icon_props->get_name().c_str());
		}

		// Outdoors
		xmlpp::Element* outdoorsicons_node = iconprops_node->add_child("outdoors");
		for (unsigned i = 0; i < OutdoorsIcons::Instance().number_of_icons(); i++) {
			xmlpp::Element* icon_node = outdoorsicons_node->add_child("icon");
			std::stringstream icon_no_str;
			icon_no_str << i;
			icon_node->set_attribute("no", icon_no_str.str());

			set_icon_attributes(icon_node, OutdoorsIcons::Instance().get_props(i));

			icon_node->add_child_text(OutdoorsIcons::Instance().get_props(i)->get_name().c_str());
		}

		if (path.empty()) // Default!
			_main_world_xml_file->write_to_file_formatted((get_path() / (_name + ".xml")).c_str());
		else
			_main_world_xml_file->write_to_file_formatted((path / (_name + ".xml")).c_str());
	}
	catch (...) {
		std::cerr << "world.cc: xml_write_world_data() failed." << std::endl;
	}

	if (_main_world_xml_file != NULL) {
		delete _main_world_xml_file;
		_main_world_xml_file = NULL;
	}
}

/**
 * This protected function is only called by xml_write_world_data.  It
 * sets the flags to be written for the icon, represented by icon_node.
 */

void World::set_icon_attributes(xmlpp::Element* icon_node, IconProps* prop)
{
	/* Note, we sometimes omit the Not/Default-Property, as this will only bloat the XML file.
	 * Instead, use reasonable defaults on object creation.
	 */

	if (prop->_trans == PropertyStrength::Full)
		icon_node->set_attribute("trans", "full");
	else if (prop->_trans == PropertyStrength::Some)
		icon_node->set_attribute("trans", "semi");
	else
		icon_node->set_attribute("trans", "not");

	if (prop ->_is_walkable == PropertyStrength::Full)
        icon_node->set_attribute("walk", "full");
    else if (prop ->_is_walkable == PropertyStrength::Some)
        icon_node->set_attribute("walk", "slow");
    else
    	icon_node->set_attribute("walk", "not");

	if (prop ->_poisonous == PropertyStrength::Full)
        icon_node->set_attribute("poison", "full");
    else if (prop->_poisonous == PropertyStrength::Some)
        icon_node->set_attribute("poison", "semi");
    else
    	; // icon_node->set_attribute("poison", "not");

	if (prop ->_magical_force_field == PropertyStrength::Full)
        icon_node->set_attribute("magic_force_field", "full");
    else if (prop ->_magical_force_field== PropertyStrength::Some)
        icon_node->set_attribute("magic_force_field", "semi");
    else
    	; // icon_node->set_attribute("magic_force_field", "not");
}

/**
 * Initialises all of the Lua item arrays, with respect to the passed Lua state.
 */

void World::init_lua_arrays(lua_State* L)
{
	LuaWrapper lua(L);

	// First load those which are defined in terms of Lua tables and therefore defs.lua files inside their corresponding subdirectories inside data/

	std::string elems[] = { "weapons", "shields", "bestiary", "edibles", "services", "miscitems", "potions" };
	int number_of_elems = sizeof(elems) / sizeof(std::string);

	for (int i = 0; i < number_of_elems; i++) {
		if (luaL_dofile(L, (conf_world_path / elems[i] / "defs.lua").c_str())) {
			cerr << "WARNING: world.cc: Couldn't execute Lua file: " << lua_tostring(L, -1) << " Game not properly installed or incomplete?\n";
			continue;
		}

		for (boost::filesystem::directory_iterator itr((conf_world_path / elems[i]).string());
				itr != boost::filesystem::directory_iterator();
				++itr)
		{
			const string fname = itr->path().filename().string();

			if (fname.compare("defs.lua") != 0 && fname.find(".lua") != string::npos) {
				if (luaL_dofile(L, (conf_world_path / elems[i] / fname).c_str())) {
					cerr << "WARNING: world.cc: Couldn't execute Lua file " << fname << ": " << lua_tostring(L, -1) << " Game not properly installed or incomplete?\n";
					continue;
				}
			}
		}
	}
}

std::vector<Spell> World::load_lua_spells(lua_State* L)
{
	std::vector<Spell> spells;

	cout << "INFO: world.cc: Loading spells...\n";
	try {
		boost::filesystem::path targetDir(conf_world_path / "spells");
		boost::filesystem::recursive_directory_iterator iter(targetDir), eod;

		BOOST_FOREACH(boost::filesystem::path const& i, make_pair(iter, eod)) {
			if (is_regular_file(i)) {
				Spell spell = Spell::spell_from_file_path(i.string(), L);
				spells.push_back(spell);
			}
		}
	}
	catch (...) {
		cerr << "WARNING: world.cc: Loading of spells from: " << (conf_world_path / "spells").string() << " failed. Game not properly installed or incomplete?\n";
	}

	return spells;
}

std::vector<Spell>* World::get_spells()
{
	return &_spells;
}

void World::set_spells(std::vector<Spell> spells)
{
	if (_spells.size() != 0) {
		std::cerr << "WARNING: world.cc: Adding spells to already defined spells vector. Will overwrite vector instead.\n";
		_spells.clear();
	}

	_spells.insert(_spells.end(), spells.begin(), spells.end());
}
