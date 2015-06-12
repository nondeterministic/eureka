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

class Spell
{
public:
	PROFESSION profession;
	int level;
	int sp;
	std::string name;
	std::string sound_path;
	std::string full_file_path;

};

#endif /* SRC_SPELL_HH_ */
