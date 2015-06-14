/*
 * spell.hh
 *
 *  Created on: Jun 12, 2015
 *      Author: baueran
 */

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
