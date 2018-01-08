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

#ifndef SRC_ATTACKOPTION_HH_
#define SRC_ATTACKOPTION_HH_

#include <string>

#include "playercharacter.hh"
#include "spell.hh"
#include "combat.hh"

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lualib.h>
#include <lauxlib.h>
}

class Combat;

class AttackOption
{
protected:
	PlayerCharacter* _player;
	lua_State* _L;
	int _target;

	int random(int,int);
	void printcon(std::string, bool);

public:
	AttackOption(int, lua_State*);
	virtual ~AttackOption();
	virtual void execute(Combat* = NULL);
	void set_target(int);
	PlayerCharacter* attacking_player();
};

class DefendOption : public AttackOption
{
public:
	DefendOption(int); // , lua_State*);
	~DefendOption();
	void execute(Combat* = NULL);
	std::string message;  // If non-empty, will be printed during defend action.
};

#endif
