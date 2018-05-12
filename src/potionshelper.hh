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

#ifndef POTIONSHELPER_HH_
#define POTIONSHELPER_HH_

#include "potion.hh"

#include <string>

#include <lua.h>
#include <lualib.h>

class PotionsHelper
{
private:
	static std::string _glob_array_name;

public:
	PotionsHelper();
	virtual ~PotionsHelper();
	static Potion* createFromLua(std::string, lua_State*);
	static bool existsInLua(std::string, lua_State*);
	static std::vector<Potion*> get_loaded_lua_potions(lua_State*);
	void drink(Potion*, lua_State*);
};

#endif
