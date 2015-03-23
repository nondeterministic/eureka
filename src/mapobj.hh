// ----------------------------------------------------------------
// leibniz
//
// Copyright (c) 2009  Andreas Bauer <baueran@gmail.com>
//
// This is not free software.  See COPYING for further information.
// ----------------------------------------------------------------

#ifndef __MAPOBJ_HH
#define __MAPOBJ_HH

#include <string>
#include <memory>
#include <vector>
#include "attackers.hh"
#include "action.hh"

enum MAPOBJ_TYPES
{
	MAPOBJ_ITEM,
	MAPOBJ_MONSTER
};

class MapObj
{
public:
	MapObj();
	~MapObj();
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

protected:
	MAPOBJ_TYPES _type;
	int _layer;
	int _icon;
	unsigned _x, _y;
	std::string _init_script;
	std::string _combat_script;
	std::vector<std::shared_ptr<Action>> _actions;

	// If map_obj is a monster, we can add the attackers information to it, to keep track
	// of them, say, inside a dungeon or a city.
	Attackers _foes;
};

#endif
