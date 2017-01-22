/*
 * weaponhelper.hh
 *
 *  Created on: Jun 22, 2014
 *      Author: baueran
 */

#ifndef WEAPONHELPER_HH_
#define WEAPONHELPER_HH_

#include "weapon.hh"

#include <lua.h>
#include <lualib.h>

#include <string>

class WeaponHelper
{
public:
	WeaponHelper();
	virtual ~WeaponHelper();
	static Weapon* createFromLua(std::string, lua_State*);
	static bool existsInLua(std::string, lua_State*);
};

#endif /* WEAPONHELPER_HH_ */
