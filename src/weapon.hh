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

#ifndef __WEAPON_HH
#define __WEAPON_HH

#include <string>
#include "item.hh"

class Weapon : public Item
{
protected:
  int _range;
  int _hands;
  int _dmg_min;
  int _dmg_max;
  int _dmg_bonus;
  int _light_radius;
  int _destroy_after;
  std::string _ammo;

public:
  Weapon();
  Weapon(const Weapon&);
  // Weapon& operator=(const Weapon&);
  virtual ~Weapon();

  int hands();
  void hands(int);

  int dmg_min();
  void dmg_min(int);

  int dmg_max();
  void dmg_max(int);

  int light_radius();
  void light_radius(int);

  int destroy_after();
  void destroy_after(int);

  int dmg_bonus();
  void dmg_bonus(int);

  int range();
  void range(int);

  std::string get_lua_name();

  std::string ammo() {
	  return _ammo;
  }

  void ammo(std::string a) {
	  _ammo = a;
  }
};

#endif
