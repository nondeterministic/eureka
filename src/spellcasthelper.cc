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
	// std::cout << "~SpellCastHelper.\n";
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

bool SpellCastHelper::is_attack_spell_only()
{
	LuaWrapper lua(L);

	return lua.call_fn<bool>("is_attack_spell");
}

// Execute choose (monsters) function in Lua spell file

int SpellCastHelper::choose()
{
	NoChooseFunctionException noChooseFunction;
	LuaWrapper lua(L);

	if (!lua.call_fn<bool>("is_choose_function_defined")) {
		std::cout << "CHOOSE FUNCTRION NOT DEFINED!!\n";
		throw noChooseFunction;
	}
	std::cout << "CHOOSE FUNCTRION IS DEFINED!!\n";

	lua.call_void_fn("choose");
	int targets = lua.call_fn<double>("get_targets");

	return targets;
}

// Execute cast function in Lua spell file

void SpellCastHelper::execute(Combat* combat)
{
	LuaWrapper lua(L);
	lua.call_void_fn("cast");
	player->set_sp(player->sp() - spell.sp);
	ZtatsWin::Instance().update_player_list();
}

// Note that finish() is called from spellsbearer.cc

void SpellCastHelper::init()
{
	SpellNotEnabledException notEnabled;
	LuaWrapper lua(L);

	if (!enabled())
		throw notEnabled;

	if (player == NULL)
		std::cerr << "ERROR: spellcasthelper.cc: player == NULL. This is serious.\n";

	lua.push_fn_arg(player->name());
	lua.call_void_fn("set_caster");

	lua.call_void_fn("init");
}
