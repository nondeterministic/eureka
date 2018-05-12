// This source file is part of eureka
//
// Copyright (c) 2007-2018 Andreas Bauer <a@pspace.org>
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

#include <iostream>
#include "weapon.hh"
#include "item.hh"

using namespace std;

Weapon::Weapon()
{
	_destroy_after = 0;
	_light_radius = 0;
	_range = 10;
	_hands = 1;
	_dmg_bonus = 0;
	icon = 0;
	_dmg_max = 0;
	_dmg_min = 0;
	_ammo = "";
}

Weapon::Weapon(const Weapon& w): Item(w)
{
	_range = w._range;
	_hands = w._hands;
	_dmg_min = w._dmg_min;
	_dmg_max = w._dmg_max;
	_dmg_bonus = w._dmg_bonus;
	icon = w.icon;
	_light_radius = w._light_radius;
	_destroy_after = w._destroy_after;
	_ammo = w._ammo;
}

// Weapon& Weapon::operator=(const Weapon& w)
// {
//   std::cout << "Weapon::=operator: " << name() << "\n";
//   _range = w._range;
//   _hands = w._hands;
//   _dmg_min = w._dmg_min;
//   _dmg_max = w._dmg_max;
//   _dmg_bonus = w._dmg_bonus;
// }

Weapon::~Weapon()
{
}

std::string Weapon::get_lua_name()
{
	return "weapons::" + name();
}

int Weapon::range()
{
  return _range;
}

void Weapon::range(int h)
{
  _range = h;
}

int Weapon::hands()
{
  return _hands;
}

void Weapon::hands(int h)
{
  _hands = h;
}

int Weapon::dmg_min()
{
  return _dmg_min;
}

void Weapon::dmg_min(int d)
{
  _dmg_min = d;
}

int Weapon::dmg_max()
{
  return _dmg_max;
}

void Weapon::dmg_max(int d)
{
  _dmg_max = d;
}

int Weapon::dmg_bonus()
{
  return _dmg_bonus;
}

void Weapon::dmg_bonus(int d)
{
  _dmg_bonus = d;
}

int Weapon::light_radius()
{
  return _light_radius;
}

void Weapon::light_radius(int d)
{
  _light_radius = d;
}

int Weapon::destroy_after()
{
  return _destroy_after;
}

void Weapon::destroy_after(int d)
{
  _destroy_after = d;
}
