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

#ifndef __GENERALMAP_HH
#define __GENERALMAP_HH

#include "boost/unordered_map.hpp"
#include <string>
#include <vector>
#include <utility>
#include <memory>
// See comments in world.hh for the weird 'undef None'
#undef None
#include <libxml++/libxml++.h>
#include "mapobj.hh"
#include "action.hh"
#include "gameevent.hh"

class Map
{
public:
  Map();
  Map(const Map&);
  virtual ~Map();

  void unload_map_data();
  std::string get_name();
  void set_name(const char*);
  // Loads the map from disk and overwrites any changes that may have
  // been made to the map stored in memory.  That is, the disk data
  // will shamelessly overwrite the currently used memory of the map.
  bool xml_load_map_data(std::string = "");
  bool xml_write_map_data(std::string = "");
  // Returns true if this map has been written on disk before, i.e.,
  // this can be indirectly used to check whether the map is new or
  // not.
  bool exists_on_disk();
  unsigned height() const;
  unsigned width() const;
  bool modified() const;
  // If called like set_notmodified(), it means that the map was saved
  // and has, since then, not been modified.  If called
  // set_notmodified(false), then it means the map was modified and
  // needs saving.
  void set_notmodified(bool = true);
  // Get and set_tile use absolute map coordinates, i.e., could be
  // absolute hex values
  virtual int get_tile(unsigned, unsigned) = 0;
  virtual int set_tile(unsigned, unsigned, unsigned) = 0;
  virtual bool is_outdoors() const = 0;
  // Shrink or expand map, depending on the values
  virtual void expand_map(int, int, int, int) = 0;
  void expand_map_data(int, int, int, int);
  void push_obj(MapObj);
  void pop_obj(int, int);
  void pop_obj(std::string);
  void push_icon(int, int, unsigned);
  std::vector<MapObj*> get_objs(unsigned x, unsigned y);
  std::vector<MapObj*> get_objs(std::pair<unsigned, unsigned> coords);
  std::shared_ptr<Action> get_action(unsigned, unsigned);
  void add_action(Action*);
  void add_action(std::shared_ptr<Action>);
  void add_event_to_action(unsigned, unsigned, std::shared_ptr<GameEvent>);
  void del_action(unsigned, unsigned);
  boost::unordered_multimap<std::pair<unsigned, unsigned>, MapObj>* objs();

  bool guarded_city;
  bool is_dungeon;

protected:
  // Variables
  std::string _name;
  // xmlpp::Document* _main_map_xml_file;
  // xmlpp::Node* _main_map_xml_root;
  bool _modified;

  // XML-helper functions
  void parse_node(const xmlpp::Node*);
  void parse_objects_node(const xmlpp::Node*);
  void parse_data_node(const xmlpp::Node*);
  std::vector<std::shared_ptr<Action>> parse_actions_node(const xmlpp::Node*);
  void write_action_node(xmlpp::Element*, Action*);

  // Map data and stuff...
  std::vector<std::vector<unsigned>>                               _data;
  boost::unordered_multimap<std::pair<unsigned, unsigned>, MapObj> _map_objects;
  std::vector<std::shared_ptr<Action>>                             _actions;
};

#endif
