// This source file is part of eureka
//
// Copyright (c) 2007-2018 Andreas Bauer <baueran@gmail.com>
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
	MapObj(const MapObj&);

	bool operator==(const MapObj&) const;
	std::string description();
	void set_description(std::string);
	void set_origin(unsigned, unsigned);
	void get_origin(unsigned&, unsigned&);
	void set_coords(unsigned, unsigned);
	void get_coords(unsigned&, unsigned&);
	std::pair<unsigned, unsigned> get_coords();
	void set_icon(unsigned);
	unsigned get_icon();
	void set_layer(int);
	int get_layer();
	bool is_removeable();
	void set_type(MAPOBJ_TYPES);
	MAPOBJ_TYPES get_type();
	bool is_animate();
	void set_init_script_path(std::string);
	std::string get_init_script_path();
	void set_combat_script_path(std::string);
	std::string get_combat_script_path();
	void add_action(std::shared_ptr<Action>);
	std::vector<std::shared_ptr<Action>>* actions();
	void set_foes(Attackers);
	Attackers get_foes();
	std::string to_string();

	/// Can be used by the game designer to specify a specific item to be dropped or picked up with push/pop methods defined in map.cc.  Can be empty.
	std::string id;
	/// If it's empty, it means, the player will never pick up this item, add it to the inventory, etc.  Also, if it's empty, the MapObj is automatically non-removable!
	std::string lua_name;
	int how_many;
	MOVE_MODE move_mode;
	PERSONALITY personality;
	LOCK_TYPE lock_type;
	bool openable;
	bool is_random_monster;

protected:
	MAPOBJ_TYPES _type;
	int _layer;
	int _icon;
	unsigned _x, _y;     // current pos
	unsigned _ox, _oy;   // origin
	std::string _init_script;
	std::string _combat_script;
	std::vector<std::shared_ptr<Action>> _actions;
	/// Can be left empty.  Will further specify an item, e.g., to differentiate different types of scrolls.
	/// Otherwise all scrolls are lumped together as one in the inventory.
	std::string _descr;  // ATTENTION: Right now, this field only is used for MiscItem items!!

	// If map_obj is a monster, we can add the attackers information to it, to keep track
	// of them, say, inside a dungeon or a city.
	Attackers _foes;
};

#endif
