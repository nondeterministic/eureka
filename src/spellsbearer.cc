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

#include <string>
#include <iostream>
#include <map>

#include "spell.hh"
#include "spellsbearer.hh"
#include "luawrapper.hh"

#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/unordered_map.hpp>

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
}

void SpellsBearer::add_active_spell(std::string spell_file_path, int duration)
{
	_active_spells[spell_file_path] = duration;
}

// player_name - the name of the player, if any, whose spell effects need to be reversed when the spell duration has run out.

void SpellsBearer::decrease_spells(lua_State* L, std::string player_name)
{
	if (_active_spells.size() < 0)
		return;

	active_spell tmp_spells;

	BOOST_FOREACH(active_spell::value_type spell, _active_spells) {
		// Decrease duration
		if (spell.second > 1) {
			tmp_spells[spell.first] = spell.second - 1;
		}
		// Or call "spell-destructors" if duration is about to be set to 0
		else {
			LuaWrapper lua(L);

			if (luaL_dofile(L, spell.first.c_str())) {
				std::cerr << "ERROR: spellsbearer.cc: Couldn't execute Lua file: " << lua_tostring(L, -1) << std::endl;
				return;
			}
			else {
				lua.push_fn_arg(player_name);
				lua.call_void_fn("finish");
			}
		}
	}

	// Now copy remaining and adjusted spells back to the spell directory.
	_active_spells.clear();
	BOOST_FOREACH(active_spell::value_type spell, tmp_spells) {
		_active_spells[spell.first] = spell.second;
	}
}

void SpellsBearer::add_icon_to_walkable(int icon)
{
	_additional_walkable_icons.push_back(icon);
}

void SpellsBearer::remove_icon_from_walkable(int icon)
{
	_additional_walkable_icons.erase(std::remove(_additional_walkable_icons.begin(), _additional_walkable_icons.end(), icon), _additional_walkable_icons.end());
}

std::vector<int> SpellsBearer::get_additional_walkable_icons()
{
	return _additional_walkable_icons;
}
