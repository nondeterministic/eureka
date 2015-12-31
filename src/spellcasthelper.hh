/*
 *
 *  Created on: Jun 12, 2015
 *      Author: baueran
 */

#ifndef SRC_SPELLCASTHELPER_HH_
#define SRC_SPELLCASTHELPER_HH_

#include "spell.hh"
#include "attackoption.hh"
#include "combat.hh"

#include <string>

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lualib.h>
#include <lauxlib.h>
}

class Combat;

class SpellCastHelper : public AttackOption
{
private:
	Spell spell;

	bool enabled(); // True if the player is a magic user, prints on console an error otherwise.  INTERNAL USE ONLY!

public:
	SpellCastHelper(int, lua_State*);
	~SpellCastHelper();
	void set_spell_path(std::string);
	void execute(Combat* = NULL);
	int choose();
	void init();
	// finish() is called from spellsbearer.cc
	bool is_attack_spell_only();
};

#endif
