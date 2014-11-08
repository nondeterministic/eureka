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

#ifndef __ARMOUR_HH
#define __ARMOUR_HH

#include "item.hh"

class Armour : public Item
{
protected:
  int _protection;

public:
  Armour();
  ~Armour();

  const int hands();
  Weapon& hands(int);

  const int dmg_min();
  Weapon& dmg_min(int);

  const int dmg_max();
  Weapon& dmg_max(int);

  const int dmg_bonus();
  Weapon& dmg_bonus(int);

  const int range();
  Weapon& range(int);
};

#endif
