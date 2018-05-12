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
#include <string>
#include "item.hh"

using namespace std;

Item::Item()
{
	_type = ItemType::Other;
	_descr = "";
	_name = "";
	_plural_name = "";
	_gold = 0;
	icon = 0;
	_weight = 0;
}

Item::Item(const Item& i)
{
	_type = i._type;
	_descr = i._descr;
	_name = i._name;
	_plural_name = i._plural_name;
	_weight = i._weight;
	icon = i.icon;
	_gold = i._gold;
	std::cout << "DEEP COPY I OF ITEM\n";
}

Item& Item::operator=(const Item& i)
{
	_type = i._type;
	_name = i._name;
	_plural_name = i._plural_name;
	_weight = i._weight;
	icon = i.icon;
	_gold = i._gold;
	std::cout << "DEEP COPY II OF ITEM\n";
	return *this;
}

ItemType Item::get_item_type()
{
	return _type;
}

void Item::set_item_type(ItemType type)
{
	_type = type;
}

int Item::gold()
{
	return _gold;
}

void Item::gold(int g)
{
	_gold = g;
}

std::string Item::name() const
{
	return _name;
}

Item& Item::name(const string n)
{
	_name = n;
	return *this;
}

const string& Item::plural_name()
{
	return _plural_name;
}

Item& Item::plural_name(const string n)
{
	_plural_name = n;
	return *this;
}

unsigned Item::weight()
{
	return _weight;
}

void Item::weight(unsigned new_weight)
{
	_weight = new_weight;
}

std::string Item::get_lua_name()
{
	return "";
}

std::string Item::description()
{
	return _descr;
}

void Item::description(std::string d)
{
	_descr = d;
}
