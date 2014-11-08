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

#ifndef __ITEM_HH
#define __ITEM_HH

#include <string>

class Item
{
protected:
  std::string _name;
  std::string _plural_name;
  unsigned _weight;
  int _gold;

public:
  Item();
  Item(const Item&);
  virtual ~Item() {};

  bool removable;
  int  icon;

  virtual std::string luaName();

  Item& operator=(const Item&);
  std::string name() const;
  Item& name(const std::string);
  const std::string& plural_name();
  Item& plural_name(const std::string);
  unsigned weight();
  void weight(unsigned);
  int gold();
  void gold(int);
};

#endif
