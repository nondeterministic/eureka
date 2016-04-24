/*
 * attackoptions.hh
 *
 *  Created on: Jun 16, 2015
 *      Author: baueran
 */

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
	PlayerCharacter* player;
	lua_State* L;
	int target;

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
	DefendOption(); // int, lua_State*);
	~DefendOption();
	void execute(Combat* = NULL);
};

#endif
