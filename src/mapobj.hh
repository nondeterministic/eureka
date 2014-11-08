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
	bool removable;
	std::string id;
	std::string lua_name;
	int how_many;

protected:
	MAPOBJ_TYPES _type;
	int _layer;
	int _icon;
	unsigned _x, _y;
	std::string _init_script;
};

#endif
