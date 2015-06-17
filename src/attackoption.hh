/*
 * attackoptions.hh
 *
 *  Created on: Jun 16, 2015
 *      Author: baueran
 */

#ifndef SRC_ATTACKOPTION_HH_
#define SRC_ATTACKOPTION_HH_

#include <string>

#include "spell.hh"
#include "creature.hh"

class AttackOption
{
public:
	AttackOption();

	bool is_cast_spell();
	bool is_attack();
	bool is_defend();

	void defend(bool = true);

	void attack(int);                        // Attack n-th opponent
	void attack(Creature*);				     // Attack this specific enemy
	int attacking_nr();
	Creature* attacking_who();

	void cast_spell(std::string);            // Cast spell with file path...
	void cast_spell(Spell);                  // Cast spell
	std::string get_spell_path();

private:
	std::string spell_fp;
	Spell spell;
	Creature* enemy;
	int enemy_nr;
	bool defending;
};

#endif /* SRC_ATTACKOPTIONS_HH_ */
