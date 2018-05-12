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

#include "edible.hh"
#include "item.hh"
#include <iostream>

Edible::Edible()
{
	food_up = 0;
	healing_power = NONE;
	poison_healing_power = NONE;
	poisonous = NONE;
	is_magic_herb = false;
	intoxicating = NONE;
}

Edible::~Edible()
{
}

Edible::Edible(const Edible& s): Item(s)
{
	food_up = s.food_up;
	healing_power = s.healing_power;
	poison_healing_power = s.poison_healing_power;
	poisonous = s.poisonous;
	is_magic_herb = s.is_magic_herb;
	intoxicating = s.intoxicating;
}

std::string Edible::get_lua_name()
{
	return "edibles::" + name();
}
