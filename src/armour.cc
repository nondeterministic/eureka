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

#include <iostream>
#include <string>

#include "armour.hh"
#include "item.hh"

Armour::Armour()
{
	_protection = 0;
	_is_shoes = false;
	_is_helmet = false;
	_is_gloves = false;
}

Armour::~Armour()
{
}

Armour::Armour(const Armour& s): Item(s)
{
  _protection = s._protection;
  _is_gloves = s._is_gloves;
  _is_helmet = s._is_helmet;
  _is_shoes = s._is_shoes;
}

bool Armour::is_helmet()
{
	return _is_helmet;
}

bool Armour::is_shoes()
{
	return _is_shoes;
}

bool Armour::is_gloves()
{
	return _is_gloves;
}

void Armour::set_is_helmet()
{
	_is_helmet = true;
}

void Armour::set_is_gloves()
{
	_is_gloves = true;
}

void Armour::set_is_shoes()
{
	_is_shoes = true;
}

int Armour::protection()
{
  return _protection;
}

void Armour::protection(int p)
{
  _protection = p;
}

std::string Armour::get_lua_name()
{
	return "armour::" + name();
}
