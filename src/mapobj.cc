// ----------------------------------------------------------------
// leibniz
//
// Copyright (c) 2009  Andreas Bauer <baueran@gmail.com>
//
// This is not free software.  See COPYING for further information.
// ----------------------------------------------------------------

#include "mapobj.hh"

#include <string>
#include <iostream>

MapObj::MapObj()
{
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
}

MapObj::~MapObj()
{
	_actions.clear();
}

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
