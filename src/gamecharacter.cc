#include "gamerules.hh"
#include "gamecharacter.hh"
#include "util.hh"
#include "race.hh"
#include "item.hh"
#include "weapon.hh"
#include "shield.hh"
#include "armour.hh"

#include <string>
#include <iostream>

GameCharacter::GameCharacter()
{
	_rh_item = NULL;
	_lh_item = NULL;
	_armour = NULL;
	_head_armour = NULL;
	_feet_armour = NULL;
	_hands_armour = NULL;
	_condition = GOOD;
	_att = RIGHT;
	_end = 0;
	_char = 0;
	_dxt = 0;
	_sp_max = 0;
	_sp = 0;
	_hp = -1;
	_hp_max = -1;
	_sex = true;
	_iq = 0;
	_wis = 0;
	_luck = 0;
	_str = 0;
	_race = HUMAN;
	_gold = 0;
}

GameCharacter::~GameCharacter()
{
	std::cout << "~GameCharacter\n";
	// if (_rh_item != NULL)
	//   delete _rh_item;
	// if (_lh_item != NULL)
	//   delete _lh_item;
}

GameCharacter::GameCharacter(const GameCharacter& p)
{
	// std::cout << "DEEP COPY OF GAMECHARACTER\n";

	_name = p._name;
	_plural_name = p._plural_name;

	_rh_item = p._rh_item;
	_lh_item = p._lh_item;
	_armour = p._armour;
	_head_armour = p._head_armour;
	_feet_armour = p._feet_armour;
	_hands_armour = p._hands_armour;
	_condition = p._condition;
	_att = p._att;
	_end = p._end;
	_char = p._char;
	_dxt = p._dxt;
	_sp_max = p._sp_max;
	_sp = p._sp;
	_hp = p._hp;
	_hp_max = p._hp_max;
	_sex = p._sex;
	_iq = p._iq;
	_wis = p._wis;
	_luck = p._luck;
	_str = p._str;
	_race = p._race;
	_gold = p._gold;
}

void GameCharacter::set_weapon(Weapon* w)
{
	_rh_item = w;
}

void GameCharacter::set_shield(Shield* s)
{
	_lh_item = s;
}

Shield* GameCharacter::shield()
{
	return _lh_item;
}

void GameCharacter::set_armour(Armour* s)
{
	_armour = s;
}

Armour* GameCharacter::armour()
{
	return _armour;
}

Armour* GameCharacter::armour_feet()
{
	return _feet_armour;
}

void GameCharacter::set_armour_feet(Armour* s)
{
	_feet_armour = s;
}

Armour* GameCharacter::armour_hands()
{
	return _hands_armour;
}

void GameCharacter::set_armour_hands(Armour* s)
{
	_hands_armour = s;
}

Armour* GameCharacter::armour_head()
{
	return _head_armour;
}

void GameCharacter::set_armour_head(Armour* s)
{
	_head_armour = s;
}

Weapon* GameCharacter::weapon()
{
	return _rh_item;
}

void GameCharacter::set_name(const char* name)
{
	_name = name;
}

void GameCharacter::set_name(std::string name)
{
	_name = name;
}

std::string GameCharacter::name()
{
	return _name;
}

void GameCharacter::set_plural_name(const char* nname)
{
	_plural_name = nname;
}

void GameCharacter::set_plural_name(std::string nname)
{
	_plural_name = nname;
}

std::string GameCharacter::plural_name()
{
	return _plural_name;
}

int GameCharacter::hpm()
{
	return _hp_max;
}

void GameCharacter::set_spm(int spm)
{
	_sp_max = spm;
}

int GameCharacter::spm()
{
	return _sp_max;
}

void GameCharacter::set_gold(int new_gold)
{
	_gold = std::max(0, new_gold);
}

int GameCharacter::gold()
{
	return _gold;
}

void GameCharacter::set_hpm(int hpm)
{
	_hp_max = hpm;
}

void GameCharacter::set_hp(int hp)
{
	_hp = std::max(0, hp);            // Needs to be >0

	if (hpm() >= 0)
		_hp = std::min(_hp, hpm());   // Needs to be <hmp(), if hpm() has been set.  This might be the case during loading of an old game.

	if (_hp == 0)
		_condition = DEAD;
}

void GameCharacter::add_hp(int hpoints)
{
	set_hp(hp() + hpoints);
}

int GameCharacter::hp()
{
	return _hp;
}

void GameCharacter::set_sp(int sp)
{
	_sp = sp;
}

int GameCharacter::sp()
{
	return _sp;
}

void GameCharacter::set_str(int str)
{
	_str = str;
}

int GameCharacter::str()
{
	return _str;
}

void GameCharacter::set_luck(int luck)
{
	_luck = luck;
}

int GameCharacter::luck()
{
	return _luck;
}

void GameCharacter::set_dxt(int dxt)
{
	_dxt = dxt;
}

int GameCharacter::dxt()
{
	return _dxt;
}

void GameCharacter::set_wis(int wis)
{
	_wis = wis;
}

int GameCharacter::wis()
{
	return _wis;
}

void GameCharacter::set_char(int charr)
{
	_char = charr;
}

int GameCharacter::charr()
{
	return _char;
}

void GameCharacter::set_iq(int iq)
{
	_iq = iq;
}

int GameCharacter::iq()
{
	return _iq;
}

void GameCharacter::set_end(int end)
{
	_end = end;
}

bool GameCharacter::sex()
{
	return _sex;
}

void GameCharacter::set_sex(bool ns)
{
	_sex = ns;
}

int GameCharacter::end()
{
	return _end;
}

RACE GameCharacter::race()
{
	return _race;
}

void GameCharacter::set_race(RACE nr)
{
	_race = nr;
}

PlayerCondition GameCharacter::condition()
{
	return _condition;
}

void GameCharacter::set_condition(PlayerCondition nc)
{
	_condition = nc;
}

int GameCharacter::base_ac()
{
	int ac = 0;

	switch (race()) {
	case HUMAN:
		ac = 16;
		break;
	case ELF:
		ac = 14;
		break;
	case HOBBIT:
		ac = 14;
		break;
	case HALF_ELF:
		ac = 14;
		break;
	case DWARF:
		ac = 17;
		break;
	default:
		std::cerr << "Error: No armour class defined for " << race() << std::endl;
	}

	return ac;
}

int GameCharacter::armour_class()
{
	return GameRules::armour_class(this);
}
