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

#ifndef SRC_SPELL_HH_
#define SRC_SPELL_HH_

#include "profession.hh"

#include <string>

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lualib.h>
#include <lauxlib.h>
}

class Spell
{
public:
	PROFESSION profession;
	int level;
	int sp;
	std::string name;
	std::string sound_path;
	std::string full_file_path;
	int aims_at; // if this is 1, the spell is an attack spell aimed at 1 target. If it is 2, then it aims at two targets, or
				 // if it is a very large number, it may aim at all targets, e.g., 999.  The player can only choose a group of attackers though,
				 // not individual targets.

	static Spell spell_from_file_path(std::string, lua_State*);
};

#endif /* SRC_SPELL_HH_ */
