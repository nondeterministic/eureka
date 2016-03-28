// This source file is part of Simplicissimus
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

#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/path.hpp>

#include <string>
#include <iostream>
#include <vector>

#include "spell.hh"
#include "luawrapper.hh"
#include "simplicissimus.hh"
#include "party.hh"
#include "playercharacter.hh"
#include "gamecontrol.hh"

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lualib.h>
#include <lauxlib.h>
}

// Nice little static helper that returns a Spell object for a given filepath to a Lua
// spell definition file.  Needs a Lua state to evaluate the contents of the spell
// definition file.

Spell Spell::spell_from_file_path(std::string fp, lua_State* L)
{
	LuaWrapper lua(L);
	Spell spell;

	// Extract profession, assuming the directory is .../.../mage/spell.lua; whereas i points to spell.lua
	std::vector<std::string> strs;
	boost::split(strs, fp, boost::is_any_of("/\\"));
	spell.profession     = stringToProfession.at(boost::to_upper_copy<std::string>(strs.at(strs.size() - 2)));
	spell.full_file_path = fp;

	// Now execute spell to extract missing data that we want to store in spell object
	if (luaL_dofile(L, fp.c_str())) {
		std::cerr << "ERROR: spell.cc: Couldn't execute Lua file: " << lua_tostring(L, -1) << std::endl;
		exit(1);
	}

	spell.name           = lua.call_fn<std::string>("get_name");
	spell.sp             = lua.call_fn<double>("get_sp");
	spell.level          = lua.call_fn<double>("get_level");
	spell.aims_at        = lua.call_fn<double>("get_targets");

	boost::filesystem::path sound_path(lua.call_fn<std::string>("get_sound_path"));
	spell.sound_path     = sound_path.string();
	std::cout << "SOUNDPATH: " << spell.sound_path << std::endl;

	return spell;
}
