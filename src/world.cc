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
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

#include <libxml++/libxml++.h>
#include <sstream>
#include <iostream>
#include <cstdlib>

#include "world.hh"
#include "config.h"
#include "simplicissimus.hh"
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
#include "luaapi.hh"

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lualib.h>
#include <lauxlib.h>
}

extern lua_State* _lua_state;

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

std::string World::get_path(void)
{
    return _path;
}

void World::set_name(const char* new_name)
{
    _name = new_name;
}

void World::set_world_path(const char* new_path)
{
    _path = new_path;
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
    throw MapNotFound("MapNotFound exception in World::get_open_map(" + (std::string)map_name + ")");
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

bool World::xml_load_world_data(const char* filename)
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
                std::shared_ptr<Map> new_map; //  = NULL;
                std::string new_map_name = reader.read_string();
                std::string new_map_path;

                reader.move_to_first_attribute();
                do {
                    if (reader.get_name().uppercase() == "HREF") {
                        new_map_path = reader.get_value();
                    }
                    else if (reader.get_name().uppercase() == "TYPE") {
                        if (reader.get_value().uppercase() == "INDOORS")
                            new_map = std::make_shared<IndoorsMap>(new_map_name.c_str(), new_map_path.c_str());
                        else // if (reader.get_value().uppercase() == "OUTDOORS")
                            new_map = std::make_shared<OutdoorsMap>(new_map_name.c_str(), new_map_path.c_str());
                    }
                }
                while (reader.move_to_next_attribute());
                reader.move_to_element();

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
                                    ; // do nothing as this is set by default in iconprops
                                else if (reader.get_value() == "semi") {
                                    new_props.add_flags(SEMI_TRANS);
                                    new_props.rm_flags(FULLY_TRANS);
                                }
                                else if (reader.get_value() == "not") {
                                    new_props.add_flags(NOT_TRANS);
                                    new_props.rm_flags(FULLY_TRANS);
                                }
                            }
                            else if (reader.get_name() == "walk") {
                                if (reader.get_value() == "full")
                                    ; // do nothing as this is set by default in iconprops
                                else if (reader.get_value() == "slow") {
                                    new_props.add_flags(WALK_SLOW);
                                    new_props.rm_flags(WALK_FULLSPEED);
                                }
                                else if (reader.get_value() == "not") {
                                    new_props.add_flags(WALK_NOT);
                                    new_props.rm_flags(WALK_FULLSPEED);
                                }
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

void World::xml_write_world_data(std::string path)
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

			if (IndoorsIcons::Instance().get_props(i)->light_radius() > 0)
				icon_node->set_attribute("light_radius", std::to_string(IndoorsIcons::Instance().get_props(i)->light_radius()));

			std::string sound_effect = IndoorsIcons::Instance().get_props(i)->sound_effect();
			int next_anim = IndoorsIcons::Instance().get_props(i)->next_anim();
			std::stringstream next_anim_str;
			next_anim_str << next_anim;

			icon_node->set_attribute("no", icon_no_str.str());

			set_icon_attributes(icon_node, IndoorsIcons::Instance().get_props(i)->flags());

			if (next_anim >= 0)
				icon_node->set_attribute("next_anim", next_anim_str.str());

			if (sound_effect.size() > 0)
				icon_node->set_attribute("sound_effect", sound_effect);

			icon_node->add_child_text(IndoorsIcons::Instance().get_props(i)->get_name().c_str());
		}

		// Outdoors
		xmlpp::Element* outdoorsicons_node = iconprops_node->add_child("outdoors");
		for (unsigned i = 0; i < OutdoorsIcons::Instance().number_of_icons(); i++) {
			xmlpp::Element* icon_node = outdoorsicons_node->add_child("icon");
			std::stringstream icon_no_str;
			icon_no_str << i;
			icon_node->set_attribute("no", icon_no_str.str());

			set_icon_attributes(icon_node, OutdoorsIcons::Instance().get_props(i)->flags());

			icon_node->add_child_text(OutdoorsIcons::Instance().get_props(i)->get_name().c_str());
		}

		if (path.length() == 0) // Default!
			_main_world_xml_file->write_to_file_formatted(get_path() + _name + ".xml");
		else
			_main_world_xml_file->write_to_file_formatted(path + _name + ".xml");
	}
	catch (...) {
		std::cerr << "world.cc: xml_write_world_data() failed." << std::endl;
	}

	if (_main_world_xml_file != NULL) {
		delete _main_world_xml_file;
		_main_world_xml_file = NULL;
	}
}

// This protected function is only called by xml_write_world_data.  It
// sets the flags to be written for the icon, represented by
// icon_node.

void World::set_icon_attributes(xmlpp::Element* icon_node, int flags)
{
    if (flags & SEMI_TRANS)
        icon_node->set_attribute("trans", "semi");
    else if (flags & NOT_TRANS)
        icon_node->set_attribute("trans", "not");
    else
        icon_node->set_attribute("trans", "full");

    if (flags & WALK_SLOW)
        icon_node->set_attribute("walk", "slow");
    else if (flags & WALK_NOT)
        icon_node->set_attribute("walk", "not");
    else
        icon_node->set_attribute("walk", "full");
}

/**
 * Loads all the world elements.
 */

void World::load_world_elements(lua_State* L)
{
	LuaWrapper lua(L);

	// First load those which are defined in terms of Lua tables and therefore defs.lua files inside their corresponding subdirectories inside data/

	int number_of_elems = 4;
	std::string* elems = new string[number_of_elems] { "weapons", "shields", "bestiary", "edibles" };

	for (int i = 0; i < number_of_elems; i++) {
		if (luaL_dofile(L, ((string)DATADIR + "/simplicissimus/data/" + (string)WORLD_NAME + "/" + elems[i] + "/defs.lua").c_str())) {
			cerr << "Couldn't execute Lua file: " << lua_tostring(L, -1) << endl;
			exit(1);
		}

		for (boost::filesystem::directory_iterator itr(((string)DATADIR + "/simplicissimus/data/" + (string) WORLD_NAME + "/" + elems[i]));
				itr != boost::filesystem::directory_iterator();
				++itr)
		{
			const string fname = itr->path().filename().string();

			if (fname.compare("defs.lua") != 0 && fname.find(".lua") != string::npos) {
				if (luaL_dofile(L, ((string) DATADIR + "/simplicissimus/data/" + (string)WORLD_NAME + "/" + elems[i] + "/" + fname).c_str())) {
					cerr << "Couldn't execute Lua file " << fname << ": " << lua_tostring(L, -1) << endl;
					exit(1);
				}
			}
		}
	}

	// Load spells, these are different, as those don't have a defs.lua and are separated into different directories.

	boost::filesystem::path targetDir((string)DATADIR + "/simplicissimus/data/" + (string)WORLD_NAME + "/spells/");
	boost::filesystem::recursive_directory_iterator iter(targetDir), eod;

	BOOST_FOREACH(boost::filesystem::path const& i, make_pair(iter, eod)) {
	    if (is_regular_file(i)) {
	    	Spell spell;
	    	std::vector<std::string> strs;
	    	boost::split(strs, i.string(), boost::is_any_of("/\\"));

	    	// Extract profession first, assuming the directory is .../.../mage/spell.lua; whereas i points to spell.lua
	    	spell.profession     = stringToProfession.at(boost::to_upper_copy<std::string>(strs.at(strs.size() - 2)));
	    	spell.full_file_path = i.string();

	    	// Now execute spell to extract missing data that we want to store in spell object
			if (luaL_dofile(L, i.string().c_str())) {
				std::cout << "ERROR: COULDNT EXECUTE SPELL FILE " << i.string() << endl;
				exit(1);
			}

			spell.name           = lua.call_fn<string>("get_name");
			spell.sound_path     = lua.call_fn<string>("get_sound_path");
			spell.sp             = lua.call_fn<double>("get_sp");
			spell.level          = lua.call_fn<double>("get_level");

	    	cout << "INFO: Loaded spell: " << i.string() << endl;
	    	_spells.push_back(spell);
	    }
	}
}
