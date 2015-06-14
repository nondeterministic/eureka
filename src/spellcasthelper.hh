/*
 *
 *  Created on: Jun 12, 2015
 *      Author: baueran
 */

#ifndef SRC_SPELLCASTHELPER_HH_
#define SRC_SPELLCASTHELPER_HH_

#include "spell.hh"

#include <string>

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lualib.h>
#include <lauxlib.h>
}

class SpellCastHelper
{
public:
	static void cast(int, Spell, lua_State*);
};

#endif
