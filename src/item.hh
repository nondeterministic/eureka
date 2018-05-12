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

#ifndef __ITEM_HH
#define __ITEM_HH

#include <string>

enum class ItemType
{
	MagicHerb,
	AttackOrDefenseTool,
	Edible,
	Other
};

class Item
{
protected:
  std::string _name;
  std::string _plural_name;
  std::string _descr;               // Currently only used by the derviced MiscItem
  unsigned _weight;
  int _gold;
  ItemType _type;

public:
  Item();
  Item(const Item&);
  virtual ~Item() {};

  int icon;

  virtual std::string get_lua_name();

  Item& operator=(const Item&);
  std::string name() const;
  Item& name(const std::string);
  const std::string& plural_name();
  Item& plural_name(const std::string);
  unsigned weight();
  void weight(unsigned);
  int gold();
  void gold(int);
  void description(std::string);
  std::string description();
  ItemType get_item_type();
  void set_item_type(ItemType);
};

#endif
