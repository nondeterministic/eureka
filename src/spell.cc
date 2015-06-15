/*
 * spell.cc
 *
 *  Created on: Jun 12, 2015
 *      Author: baueran
 */

#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

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
		std::cerr << "ERROR: spell.cc: Couldn't execute Lua file: " << lua_tostring(L, -1) << endl;
		exit(1);
	}

	spell.name           = lua.call_fn<std::string>("get_name");
	spell.sound_path     = lua.call_fn<std::string>("get_sound_path");
	spell.sp             = lua.call_fn<double>("get_sp");
	spell.level          = lua.call_fn<double>("get_level");
	spell.aims_at        = lua.call_fn<double>("get_targets");

	return spell;
}
