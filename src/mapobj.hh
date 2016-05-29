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

#ifndef __MAPOBJ_HH
#define __MAPOBJ_HH

#include <string>
#include <memory>
#include <vector>
#include "attackers.hh"
#include "action.hh"

enum MAPOBJ_TYPES
{
	MAPOBJ_ITEM, MAPOBJ_MONSTER, MAPOBJ_PERSON, MAPOBJ_ANIMAL
};

enum MOVE_MODE
{
	STATIC, FLEE, FOLLOWING, ROAM
};

enum PERSONALITY
{
	HOSTILE, NEUTRAL, RIGHTEOUS
};

enum LOCK_TYPE
{
	NORMAL_LOCK, MAGIC_LOCK, UNLOCKED
};

class MapObj
{
public:
	MapObj();
	~MapObj();
	void set_origin(unsigned, unsigned);
	void get_origin(unsigned&, unsigned&);
	void set_coords(unsigned, unsigned);
	void get_coords(unsigned&, unsigned&);
	void set_icon(unsigned);
	unsigned get_icon();
	void set_layer(int);
	int get_layer();
	void set_type(MAPOBJ_TYPES);
	MAPOBJ_TYPES get_type();
	void set_init_script_path(std::string);
	std::string get_init_script_path();
	void set_combat_script_path(std::string);
	std::string get_combat_script_path();
	bool removable;
	std::string id;
	std::string lua_name;
	int how_many;
	void add_action(std::shared_ptr<Action>);
	std::vector<std::shared_ptr<Action>>* actions();
	void set_foes(Attackers);
	Attackers get_foes();
	MapObj copy();

	MOVE_MODE move_mode;
	PERSONALITY personality;
	LOCK_TYPE lock_type;
	bool openable;
	bool is_random_monster;

protected:
	MAPOBJ_TYPES _type;
	int _layer;
	int _icon;
	unsigned _x, _y;  // current pos
	unsigned _ox, _oy; // origin
	std::string _init_script;
	std::string _combat_script;
	std::vector<std::shared_ptr<Action>> _actions;

	// If map_obj is a monster, we can add the attackers information to it, to keep track
	// of them, say, inside a dungeon or a city.
	Attackers _foes;
};

#endif
