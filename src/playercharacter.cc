#include <string>
#include "playercharacter.hh"
#include "profession.hh"

PlayerCharacter::PlayerCharacter()
{
	_ep = 0;  // Because it is the only class member variable that is not inherited!
}

PlayerCharacter::PlayerCharacter(const char* name, int hpm, int spm, 
				 int str, int luck, int dxt, int wis, 
				 int charr, int iq, int end, bool sex,
				 RACE race, PROFESSION profession) 
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
  _ep = 0;
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
