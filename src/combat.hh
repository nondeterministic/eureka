//
//
// Copyright (c) 2005  Andreas Bauer <baueran@gmail.com>
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

#ifndef __GCCOMBAT_HH
#define __GCCOMBAT_HH

#include <SDL.h>

#include "attackoption.hh"
#include "gamecontrol.hh"
#include "attackers.hh"
#include "eventmanager.hh"
#include "playercharacter.hh"
#include "item.hh"
#include "inventory.hh"

#include <boost/unordered_set.hpp>

#include <string>
#include <vector>

class AttackOption;

class Combat
{
protected:
  Inventory _bounty_items;
  int random(int, int);
  Attackers foes;
  EventManager* em;
  Party* party;
  bool fled;

public:
  Combat();
  ~Combat();
  bool initiate();
  int select_enemy();
  int fight(std::vector<AttackOption*>);
  int party_fight(std::vector<AttackOption*>);
  void victory();
  void add_to_bounty(Item*);
  int foes_fight();
  std::vector<AttackOption*> attack_options();
  void advance_party();
  std::string noticed_monsters();
  bool create_random_monsters();
  bool create_monsters_from_init_path(std::string);
  bool create_monsters_from_combat_path(std::string);
  void printcon(const std::string, bool wait = false);
  bool vowel(const char);
  boost::unordered_set<std::string> advance_foes();
  void flee_foe(int);
  Attackers& get_foes();
  void set_foes(Attackers);
};

#endif
