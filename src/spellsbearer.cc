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
