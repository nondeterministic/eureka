#include "gamerules.hh"
#include "gamecharacter.hh"
#include "util.hh"
#include "race.hh"
#include "item.hh"
#include "weapon.hh"
#include "shield.hh"

#include <string>
#include <iostream>

GameCharacter::GameCharacter()
{
  _rh_item = NULL;
  _lh_item = NULL;
  _gold = 0;
}

GameCharacter::~GameCharacter()
{
  // if (_rh_item != NULL)
  //   delete _rh_item;
  // if (_lh_item != NULL)
  //   delete _lh_item;
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

void GameCharacter::set_hpm(int hpm)
{
  _hp_max = hpm;
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
  _gold = new_gold;
}

int GameCharacter::gold()
{
  return _gold;
}

void GameCharacter::set_hp(int hp)
{
  _hp = hp;
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

int GameCharacter::armour_class()
{
  int ac = 0;

  switch (race()) {
  case HUMAN:
    ac = 13;
    break;
  case ELF:
    ac = 11;
    break;
  case HOBBIT:
    ac = 11;
    break;
  case HALF_ELF:
    ac = 11;
    break;
  case DWARF:
    ac = 14;
    break;
  default:
    std::cerr << "Error: No armour class defined for " << race() << std::endl;
  }

  return GameRules::armour_class(ac, dxt(), shield());
}
