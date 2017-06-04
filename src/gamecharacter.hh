//
//
// Copyright (c) 2005  Andreas Bauer <baueran@gmail.com>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
// USA.

#ifndef GAMECHAR_HH
#define GAMECHAR_HH

#include <string>

#include "item.hh"
#include "weapon.hh"
#include "shield.hh"
#include "armour.hh"
#include "race.hh"
#include "spellsbearer.hh"

using namespace std;

enum ATTITUDE {
  RIGHT, NEUTR, CHAOS
};

enum PlayerCondition {
  GOOD, POISONED, DEAD
};

class GameCharacter : public SpellsBearer
{
protected:
  int _hp, _hp_max;             // Hit points
  int _sp, _sp_max;             // Spell points

  int _str;                     // Strength
  int _luck;                    // Luck
  int _dxt;                     // Dexterity
  int _wis;                     // Wisdom
  int _char;                    // Charisma
  int _iq;                      // Intelligene
  int _end;                     // Endurance

  int _gold; // TODO: Do we still need this???

  ATTITUDE _att;                // Righteous, neutral, chaotic
  bool _sex;                    // F = Female, T = Male

  std::string _name;            // Name
  std::string _plural_name;     // Plural name

  RACE        _race;
  PlayerCondition _condition;

  Weapon* _rh_item;
  Shield* _lh_item;
  Armour* _armour;
  Armour* _head_armour;
  Armour* _feet_armour;
  Armour* _hands_armour;
  
public:
  GameCharacter();
  ~GameCharacter();
  GameCharacter(const GameCharacter&);

  void set_weapon(Weapon*);
  Weapon* weapon();
  void set_shield(Shield*);
  Shield* shield();
  Armour* armour();
  void set_armour(Armour*);
  Armour* armour_head();
  void set_armour_head(Armour*);
  Armour* armour_feet();
  void set_armour_feet(Armour*);
  Armour* armour_hands();
  void set_armour_hands(Armour*);
  void set_name(const char*);
  void set_name(std::string);
  std::string name();
  void set_plural_name(const char*);
  void set_plural_name(std::string);
  std::string plural_name();
  void set_hpm(int);
  int hpm();
  void set_spm(int);
  int spm();
  void set_gold(int);
  int gold();
  void add_hp(int);
  void set_hp(int);
  int hp();
  void set_sp(int);
  int sp();
  void set_str(int);
  int str();
  void set_luck(int);
  int luck();
  void set_dxt(int);
  int dxt();
  void set_wis(int);
  int wis();
  void set_char(int);
  int charr();
  void set_iq(int);
  int iq();
  void set_end(int);
  int end();
  bool sex();
  void set_sex(bool);
  PlayerCondition condition();
  void set_condition(PlayerCondition);
  RACE race();
  void set_race(RACE);
  int armour_class();
};

#endif
