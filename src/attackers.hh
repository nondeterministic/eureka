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

#ifndef __ATTACKERS_HH
#define __ATTACKERS_HH

#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include <vector>

#include <boost/unordered_map.hpp>

#include <SDL2/SDL.h>

#include "creature.hh"

class Attackers
{
private:
  std::vector<std::shared_ptr<Creature>> _enemies;
  boost::unordered_map<std::string, int> _enemies_count;

public:
  Attackers();
  ~Attackers();
  Attackers(const Attackers&);
  void remove(int);
  void remove(std::vector<int>&);
  const std::vector<std::shared_ptr<Creature>>* get();
  Creature* get(int number);
  void add(std::shared_ptr<Creature>);
  std::vector<std::shared_ptr<Creature>>::iterator begin();
  std::vector<std::shared_ptr<Creature>>::iterator end();
  std::string to_string();
  std::string get_plural_name(const string&);
  std::shared_ptr<SDL_Texture> pic(SDL_Renderer*);
  int get_distance(const std::string&);
  Creature* get_attacker(int distance);
  boost::unordered_map<std::string, int>* count();
  int size() const;
  int closest_range();
  bool vowel(const char);
  boost::unordered_map<int, std::string> distances();
  std::string attacker_at(int);
  void move(const char*, int);
};

#endif
