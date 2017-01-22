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

#ifndef GAMESTATE_HH_
#define GAMESTATE_HH_

#include "indoorsmap.hh"
#include "playercharacter.hh"
#include "inventory.hh"

#include <vector>
#include <memory>
#include <string>

#include <lua.h>
#include <lualib.h>

#include <boost/filesystem.hpp>

class GameState
{
protected:
	std::vector<std::shared_ptr<IndoorsMap>> _maps;
	GameState();
	bool save_party(boost::filesystem::path);
	bool _cur_outdoors;
	std::string _cur_map_name;
	std::vector<PlayerCharacter> _players;
	Inventory _inventory;
	int _jimmylocks;
	int _gold;
	int _food;
	int _x, _y;

public:
	static GameState& Instance();
	virtual ~GameState();
	bool save();
	bool load(lua_State*);
	bool apply();
	bool reset();
	void add_map(std::shared_ptr<IndoorsMap>);
	std::shared_ptr<IndoorsMap> get_map(std::string);
	std::string get_cur_map_name();
	bool is_cur_map_outdoors();
};

#endif /* GAMESTATE_HH_ */
