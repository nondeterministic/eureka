/*
 * attackoptions.cc
 *
 *  Created on: Jun 16, 2015
 *      Author: baueran
 */

#include "attackoption.hh"
#include "creature.hh"

#include <string>

AttackOption::AttackOption()
{
	spell_fp = "";
	enemy_nr = -1;
	enemy = NULL;
	defending = false;
}

bool AttackOption::is_cast_spell()
{
	return spell_fp.length() > 0;
}

void AttackOption::cast_spell(std::string fp)
{
	spell_fp = fp;
}

void AttackOption::cast_spell(Spell s)
{
	spell_fp = s.full_file_path;
	spell = s;
}

std::string AttackOption::get_spell_path()
{
	if (spell_fp.length() > 0)
		return spell_fp;
	else
		return spell.full_file_path;
}

bool AttackOption::is_attack()
{
	return enemy != NULL || enemy_nr >= 0;
}

int AttackOption::attacking_nr()
{
	return enemy_nr;
}

Creature* AttackOption::attacking_who()
{
	return enemy;
}

void AttackOption::attack(int enr)
{
	enemy_nr = enr;
}

void AttackOption::attack(Creature* c)
{
	enemy = c;
}

bool AttackOption::is_defend()
{
	return defending;
}


void AttackOption::defend(bool d)
{
	defending = d;
}
