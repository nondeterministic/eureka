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

#include "potion.hh"
#include "edible.hh"

#include <iostream>
#include <stdexcept>

Potion::Potion() : Edible()
{
	name_of_potion_drinker = "";
}

Potion::~Potion()
{
}

Potion::Potion(const Potion& s) : Edible(s)
{
	name_of_potion_drinker = s.name_of_potion_drinker;
	_ingredient_names.insert(_ingredient_names.end(), s._ingredient_names.begin(), s._ingredient_names.end());
}

std::string Potion::get_lua_name()
{
	return "potions::" + name();
}

void Potion::set_ingredient_names(std::vector<std::string> i)
{
	if (_ingredient_names.size() != 0)
		throw std::logic_error("Cannot add ingredients to a potion object that already has ingredients added to it.");

	_ingredient_names = i;
	std::sort(_ingredient_names.begin(), _ingredient_names.end()); // We sort now, so we can compare later. See below.
}

const std::vector<std::string>* Potion::get_ingredient_names()
{
	return &_ingredient_names;
}

bool Potion::consists_of(std::vector<std::string> ingredient_names)
{
	std::sort(ingredient_names.begin(), ingredient_names.end());
	return ingredient_names == _ingredient_names;
}
