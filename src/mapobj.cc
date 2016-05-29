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

#include "mapobj.hh"

#include <string>
#include <iostream>

MapObj::MapObj()
{
	is_random_monster = false;
	_layer = 0;
	_x = 0;
	_y = 0;
	_ox = 0;
	_oy = 0;
	_icon = 0;
	removable = false;
	lua_name = "";
	how_many = 1;
	move_mode = STATIC;
	personality = NEUTRAL;
	_combat_script = "";

	_type = MAPOBJ_ITEM;

	lock_type = UNLOCKED;
	openable = false;
}

// TODO: Am not using copy constructor as it somehow breaks the game elsewhere.
// (To see why, add a std::cout in this method somewhere when used as copy constructor.)
//
// Deep-copy of MapObj

MapObj MapObj::copy()
{
	MapObj tmp;

	tmp.is_random_monster = is_random_monster;
	tmp._layer = _layer;
	tmp._x = _x;
	tmp._y = _y;
	tmp._ox = _ox;
	tmp._oy = _oy;
	tmp._icon = _icon;
	tmp.removable = removable;
	tmp.lua_name = lua_name;
	tmp.how_many = how_many;
	tmp.move_mode = move_mode;
	tmp.personality = personality;
	tmp._type = _type;
	tmp.lock_type = lock_type;
	tmp.openable = openable;

	for (std::shared_ptr<Action> act: _actions)
		tmp._actions.push_back(act);

	tmp._init_script = _init_script;
	tmp._combat_script = _combat_script;

	// TODO: This is a bit useless, I think, as Attackers itself has no deep-copy
	// (but there are other places, where I also pass Attackers on the stack, and it seems to work)
	// Not sure, if this will be a problem later...
	tmp._foes = _foes;

	return tmp;
}

MapObj::~MapObj()
{
	_actions.clear();
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
