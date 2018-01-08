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

#ifndef SRC_SPELLCASTHELPER_HH_
#define SRC_SPELLCASTHELPER_HH_

#include "spell.hh"
#include "attackoption.hh"
#include "combat.hh"

#include <string>
#include <exception>

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lualib.h>
#include <lauxlib.h>
}

class Combat;

class SpellNotEnabledException: public exception
{
	virtual const char* what() const throw() {
		return "Spell cannot be cast";
	}
};

class NoChooseFunctionException: public exception
{
	virtual const char* what() const throw() {
		return "No choose function";
	}
};

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
