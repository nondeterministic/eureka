// This source file is part of eureka
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

#ifndef PARTY_HH
#define PARTY_HH

#include <utility>
#include <string>
#include <vector>
#include <memory>
#include <map>

#include "playercharacter.hh"
#include "weapon.hh"
#include "inventory.hh"
#include "spellsbearer.hh"

class Party : public SpellsBearer
{
protected:
  std::vector<PlayerCharacter> _players;
  int prev_x, prev_y;
  bool prev_indoors, _indoors;
  std::string prev_map_name, _map_name;
  Inventory _inv;
  int _gold, _food;
  int _guard;
  int _jlocks;
  int _magic_light_radius;

  Party();

public:
  // The absolute party coordinates on a map.  Use map->screen to get relative coords!
  int x, y;
  int rounds_intoxicated;
  bool is_resting;
  bool is_in_combat;

  // >0 if, e.g., torch is carried and lit.
  // int light_radius;
  
  static Party& Instance();
  int light_radius();
  void add_player(PlayerCharacter);
  PlayerCharacter* get_player(int);
  PlayerCharacter* get_player(std::string);
  std::vector<PlayerCharacter>::iterator party_begin();
  std::vector<PlayerCharacter>::iterator party_end();
  int party_size();
  unsigned party_alive();
  void set_guard(int);
  PlayerCharacter* get_guard();
  void set_guard(PlayerCharacter*);
  void unset_guard();
  void set_coords(int, int);
  void set_coords(std::pair<int, int>);
  std::pair<int, int> get_coords();
  void store_outside_coords();
  bool restore_outside_coords();
  bool indoors();
  void set_indoors(bool);
  void set_map_name(const char*);
  std::string map_name();
  Inventory* inventory();
  int gold();
  void set_gold(int);
  int food();
  void set_food(int);
  std::string to_xml();
  int max_carrying_capacity();
  void rm_jimmylock();
  void add_jimmylock();
  int jimmylock_count();
  void set_magic_light_radius(int);
  bool level_up();
};

#endif
