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

#include "mapobj.hh"
#include "attackers.hh"
#include "action.hh"

#include <sstream>
#include <string>
#include <iostream>

MapObj::MapObj()
{
	is_random_monster = false;
	id = "";
	_descr = "";
	_layer = 0;
	_x = 0;
	_y = 0;
	_ox = 0;
	_oy = 0;
	_icon = 0;
	lua_name = "";
	how_many = 1;
	move_mode = STATIC;
	personality = NEUTRAL;
	_combat_script = "";
	_type = MAPOBJ_ITEM;
	lock_type = UNLOCKED;
	openable = false;
}

MapObj::MapObj(const MapObj& m)
{
	is_random_monster = m.is_random_monster;
	_layer = m._layer;
	id = m.id;
	_x = m._x;
	_y = m._y;
	_ox = m._ox;
	_oy = m._oy;
	_icon = m._icon;
	lua_name = m.lua_name;
	how_many = m.how_many;
	move_mode = m.move_mode;
	personality = m.personality;
	_type = m._type;
	lock_type = m.lock_type;
	openable = m.openable;
	_descr = m._descr;
	_init_script = m._init_script;
	_combat_script = m._combat_script;

	// TODO: This is a bit useless, I think, as Attackers itself has no deep-copy
	// (but there are other places, where I also pass Attackers on the stack, and it seems to work)
	// Not sure, if this will be a problem later...
	_foes = m._foes;

	for (std::shared_ptr<Action> act: m._actions) {
		_actions.push_back(act);
	}
}

bool MapObj::operator==(const MapObj& rhs) const
{
	return
			_x == rhs._x &&
			_y == rhs._y &&
			_ox == rhs._ox &&
			_oy == rhs._oy &&
			_icon == rhs._icon &&
			how_many == rhs.how_many &&
			_type == rhs._type &&
			lua_name == rhs.lua_name &&
			openable == rhs.openable &&
			move_mode == rhs.move_mode &&
			personality == rhs.personality &&
			lock_type == rhs.lock_type &&
			is_random_monster == rhs.is_random_monster &&
			_init_script == rhs._init_script &&
			_combat_script == rhs._combat_script &&
			// TODO: Leave attackers out, because otherwise I need to include it here and therefore have a new dependency in the editor!
			// _foes.size() == rhs._foes.size() && // TODO: I believe it's ok to not directly compare all foes, but BEWARE!
			_actions.size() == rhs._actions.size() && // TODO: I believe it's ok to not directly compare all actions, but BEWARE!
			_descr == rhs._descr &&
			id == rhs.id
	;
}

MapObj::~MapObj()
{
	_actions.clear();
}

std::string MapObj::to_string()
{
	stringstream ss;

	ss << "MapObj: (x, y): (" << _x << ", " << _y << "), (ox, oy): (" << _ox << ", " << _oy <<
			", icon: " << _icon << ", how_many: << " << how_many << ", type: " << _type <<
			", lua_name: " << lua_name << ", openable: " << openable <<
			", move_mode: " << move_mode << ", personality: " << personality << ", lock_type: " << lock_type <<
			", is_random_monster: " << is_random_monster << ", init_script: " << _init_script << ", combat_script: " << _combat_script <<
			", actions.size(): " << _actions.size() << ", descr: " << _descr;

	return ss.str();
}

// TODO: This seems buggy, as there is no Attackers deep-copy. (Why doesn't this cause trouble?!)

void MapObj::set_foes(Attackers attackers)
{
	_foes = attackers;
}

Attackers MapObj::get_foes()
{
	return _foes;
}

void MapObj::add_action(std::shared_ptr<Action> new_act)
{
	try {
		// std::cout << "ADDING ACTION: " << new_act->name() << std::endl;
		_actions.push_back(std::shared_ptr<Action>(new_act));
	}
	catch(const std::exception& ex) {
		std::cerr << "Exception upon adding action in MapObj: " << ex.what() << std::endl;
	}
}

std::vector<std::shared_ptr<Action>>* MapObj::actions()
{
	return &_actions;
}

void MapObj::set_type(MAPOBJ_TYPES mt)
{
  _type = mt;
}

bool MapObj::is_animate()
{
	return _type == MAPOBJ_ANIMAL || _type == MAPOBJ_MONSTER || _type == MAPOBJ_PERSON;
}

MAPOBJ_TYPES MapObj::get_type()
{
  return _type;
}

void MapObj::set_init_script_path(std::string np)
{
  _init_script = np;
}

std::string MapObj::get_init_script_path()
{
  return _init_script;
}

void MapObj::set_combat_script_path(std::string np)
{
  _combat_script = np;
}

std::string MapObj::get_combat_script_path()
{
  return _combat_script;
}

void MapObj::set_coords(unsigned x, unsigned y)
{
  _x = x; _y = y;
}

void MapObj::get_coords(unsigned& x, unsigned& y)
{
  x = _x;
  y = _y;
}

std::pair<unsigned, unsigned> MapObj::get_coords()
{
	return std::pair<unsigned,unsigned>(_x, _y);
}

void MapObj::set_origin(unsigned x, unsigned y)
{
  _ox = x; _oy = y;
}

void MapObj::get_origin(unsigned& x, unsigned& y)
{
  x = _ox;
  y = _oy;
}

void MapObj::set_icon(unsigned no)
{
  _icon = no;
}

unsigned MapObj::get_icon(void)
{
  return _icon;
}

void MapObj::set_layer(int Slayer)
{
  _layer = Slayer;
}

int MapObj::get_layer(void)
{
  return _layer;
}

std::string MapObj::description()
{
	return _descr;
}

void MapObj::set_description(std::string d)
{
	_descr = d;
}

bool MapObj::is_removeable()
{
	return lua_name.size() > 0;
}
