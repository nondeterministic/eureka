/*
 * weaponhelper.hh
 *
 *  Created on: Jun 22, 2014
 *      Author: baueran
 */

#ifndef WEAPONHELPER_HH_
#define WEAPONHELPER_HH_

#include "weapon.hh"
#include <string>

class WeaponHelper
{
public:
	WeaponHelper();
	virtual ~WeaponHelper();
	static Weapon* createFromLua(std::string);
	static bool exists(std::string);
};

#endif /* WEAPONHELPER_HH_ */
