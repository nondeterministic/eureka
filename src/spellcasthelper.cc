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

#include "spellcasthelper.hh"
#include "spell.hh"
#include "luawrapper.hh"
#include "simplicissimus.hh"
#include "party.hh"
#include "playercharacter.hh"
#include "gamecontrol.hh"
#include "ztatswin.hh"

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lualib.h>
#include <lauxlib.h>
}

SpellCastHelper::SpellCastHelper(int p, lua_State* ls) : AttackOption(p, ls)
{
}

SpellCastHelper::~SpellCastHelper()
{
	std::cout << "~SpellCastHelper.\n";
}

void SpellCastHelper::set_spell_path(std::string sp)
{
	spell = Spell::spell_from_file_path(sp, L);

	if (luaL_dofile(L, sp.c_str())) {
		std::cerr << "ERROR: spellcasthelper.cc: Couldn't execute Lua file: " << lua_tostring(L, -1) << endl;
		exit(1);
	}
}

bool SpellCastHelper::enabled()
{
	if (!player->is_spell_caster()) {
		GameControl::Instance().printcon(player->name() + " is not a magic user.");
		return false;
	}

	if (player->sp() < spell.sp) {
		GameControl::Instance().printcon(player->name() + " does not have enough spell points.");
		return false;
	}

	if (!player->condition() == DEAD) {
		GameControl::Instance().printcon("Try that with an alive party member next time.");
		return false;
	}

	return true;
}

// Execute choose function in Lua spell file

int SpellCastHelper::choose()
{
	LuaWrapper lua(L);

	if (!enabled())
		return -1;

	lua.call_void_fn("choose");
	int targets = lua.call_fn<double>("get_targets");

	return targets;
}

// Execute cast function in Lua spell file

void SpellCastHelper::execute(Combat* combat)
{
	std::cout << "Executing spell!\n";

	LuaWrapper lua(L);

	if (!enabled())
		return;

	lua.push_fn_arg(player->name());
	lua.call_void_fn("set_caster");

	player->set_sp(player->sp() - spell.sp);

	lua.call_void_fn("cast");
	ZtatsWin::Instance().update_player_list();
}
