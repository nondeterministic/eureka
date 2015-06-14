#include <string>
#include "playercharacter.hh"
#include "profession.hh"
#include "race.hh"

PlayerCharacter::PlayerCharacter()
{
	_level = 1;
	_ep = 0;
}

PlayerCharacter::PlayerCharacter(const char* name, int hpm, int spm, 
				 int str, int luck, int dxt, int wis, 
				 int charr, int iq, int end, bool sex,
				 RACE race, PROFESSION profession) : PlayerCharacter()
{
	_race = race;
	_prof = profession;
	_name = name;
	_hp_max = hpm;
	_hp = hpm;
	_sp = spm;
	_sp_max = spm;
	_str = str;
	_luck = luck;
	_dxt = dxt;
	_wis = wis;
	_char = charr;
	_iq = iq;
	_end = end;
	_sex = sex;
	_condition = GOOD;
}

bool PlayerCharacter::is_spell_caster()
{
	return _prof == MAGE || _prof == CLERIC || _prof == DRUID || _prof == NECROMANCER || _prof == ARCHMAGE || _prof == GEOMANCER;
}

int PlayerCharacter::ep()
{
  return _ep;
}

void PlayerCharacter::inc_ep(int ne)
{
  _ep += ne;
}

PROFESSION PlayerCharacter::profession()
{
  return _prof;
}

void PlayerCharacter::set_profession(PROFESSION np)
{
  _prof = np;
}

void PlayerCharacter::set_level(int l)
{
	_level = l;
}

int PlayerCharacter::level()
{
	_level = potential_level(); // TODO!!!
	return _level;
}

// Returns the level a character can be in potentially; that is, if the player then goes through the
// up-levelling ritual (whatever that will turn out to be in this game - TODO)
//
// Initially it probably makes sense to have actual level == potential level.

int PlayerCharacter::potential_level()
{
	int lev = 1;

	switch (_race) {
	case HUMAN:
		if (_ep >= 50)
			lev = 2;
		if (_ep >= 100)
			lev = 3;
		if (_ep >= 200)
			lev = 4;
		if (_ep >= 400)
			lev = 5;
		if (_ep >= 800)
			lev = 6;
		if (_ep >= 1600)
			lev = 7;
		if (_ep >= 3200)
			lev = 8;
		if (_ep >= 5000)
			lev = 9;
		if (_ep >= 10000)
			lev = 10;
		break;
	case ELF: // slower progress than human
		if (_ep >= 100)
			lev = 2;
		if (_ep >= 200)
			lev = 3;
		if (_ep >= 400)
			lev = 4;
		if (_ep >= 800)
			lev = 5;
		if (_ep >= 1600)
			lev = 6;
		if (_ep >= 3200)
			lev = 7;
		if (_ep >= 6000)
			lev = 8;
		if (_ep >= 10000)
			lev = 9;
		if (_ep >= 15000)
			lev = 10;
		break;
	case HOBBIT: // slightly quicker progress than human
		if (_ep >= 50)
			lev = 2;
		if (_ep >= 100)
			lev = 3;
		if (_ep >= 200)
			lev = 4;
		if (_ep >= 400)
			lev = 5;
		if (_ep >= 800)
			lev = 6;
		if (_ep >= 1200)
			lev = 7;
		if (_ep >= 2000)
			lev = 8;
		if (_ep >= 5000)
			lev = 9;
		if (_ep >= 8000)
			lev = 10;
		break;
	case HALF_ELF: // quick to progress at first but slow to finish
		if (_ep >= 50)
			lev = 2;
		if (_ep >= 100)
			lev = 3;
		if (_ep >= 200)
			lev = 4;
		if (_ep >= 400)
			lev = 5;
		if (_ep >= 800)
			lev = 6;
		if (_ep >= 1600)
			lev = 7;
		if (_ep >= 4000)
			lev = 8;
		if (_ep >= 8000)
			lev = 9;
		if (_ep >= 15000)
			lev = 10;
		break;
	case DWARF: // slow to progress at first but quick to finish, bit like hobbit really
		if (_ep >= 10)
			lev = 2;
		if (_ep >= 200)
			lev = 3;
		if (_ep >= 400)
			lev = 4;
		if (_ep >= 800)
			lev = 5;
		if (_ep >= 1200)
			lev = 6;
		if (_ep >= 2000)
			lev = 7;
		if (_ep >= 4000)
			lev = 8;
		if (_ep >= 8000)
			lev = 9;
		if (_ep >= 10000)
			lev = 10;
		break;
	}

	return lev;
}
