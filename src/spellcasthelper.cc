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

void SpellCastHelper::cast(int player_no, Spell spell, lua_State* L)
{
	LuaWrapper lua(L);

	PlayerCharacter* player = Party::Instance().get_player(player_no);

	if (!player->is_spell_caster()) {
		GameControl::Instance().printcon(player->name() + " is not a magic user.");
		return;
	}

	if (player->sp() < spell.sp) {
		GameControl::Instance().printcon(player->name() + " does not have enough spell points.");
		return;
	}

	if (!player->condition() == DEAD) {
		GameControl::Instance().printcon("Try that with an alive party member next time.");
		return;
	}

	if (luaL_dofile(L, spell.full_file_path.c_str())) {
		std::cout << "ERROR: COULDNT EXECUTE SPELL FILE IN SPELL.CC: " << spell.full_file_path << std::endl;
		exit(1);
	}

	player->set_sp(player->sp() - spell.sp);

	lua.call_void_fn("cast");
	ZtatsWin::Instance().update_player_list();
}
