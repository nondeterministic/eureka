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

#ifndef __WORLD_HH
#define __WORLD_HH

// This is FUCKING ODD!!  If I don't undef None, then the inclusion of
// libxml++.h craps out.  It seems None is defined elsewhere, and
// simply undefining it leaves me with an uneasy feeling...
#undef None
#include <libxml++/libxml++.h>

#include <string>
#include <exception>
#include <vector>
#include <memory>

#include "map.hh"
#include "creature.hh"
#include "spell.hh"
#include "iconprops.hh"

#include "boost/filesystem.hpp"
#include "boost/filesystem/path.hpp"

extern "C" {
#include <lua5.1/lua.h>
}

class MapNotFound 
{
protected:
  std::string err;

public:
  MapNotFound(std::string s) { err = s; }
  std::string print() const { return err; }
};

class World
{
public:
  static World& Instance (void);

  std::string get_name(void);
  // This is the string to the world/ directory, not to the actual
  // world.  This would have to be appended to this e.g. via
  // get_name().
  boost::filesystem::path get_path(void);
  void set_name(const char*);
  void set_world_path(boost::filesystem::path);
  bool add_map(std::shared_ptr<Map>);
  bool delete_map(std::shared_ptr<Map>);
  bool exists_map(const char*);
  std::shared_ptr<Map> get_map(const char*);
  std::vector<std::shared_ptr<Map>>* get_maps();
  unsigned get_indoors_tile_size() const;
  unsigned get_outdoors_tile_size() const;
  void set_indoors_tile_size(unsigned);
  // Returns false if file was not found, or something else went
  // wrong.
  bool xml_load_world_data(const std::string);
  // bool xml_load_world_elements(const char*);
  void load_world_elements(lua_State* L);
  void xml_write_world_data();
  void xml_write_world_data(boost::filesystem::path);
  std::vector<Creature>* get_creatures();
  std::vector<Spell>* get_spells();

protected:
  World();
  ~World();

  // Override copy constructor
  World(const World&);

  std::vector<std::shared_ptr<Map>> _maps;
  std::vector<std::string> _map_names;
  std::vector<Creature> _creatures;
  std::vector<Spell> _spells;
  std::string _name;
  boost::filesystem::path _path;
  unsigned _indoors_tile_size, _outdoors_tile_size;
  xmlpp::Document* _main_world_xml_file;
  xmlpp::Node* _main_world_xml_root;
  void set_icon_attributes(xmlpp::Element*, ICON_TRANS, ICON_WALK);
};

#endif
