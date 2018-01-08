// This source file is part of eureka
//
// Copyright (c) 2007-2018  Andreas Bauer <baueran@gmail.com>
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

#ifndef _OBJ_HH
#define _OBJ_HH

#include <string>

// enum OBJ_TYPE
//   {
//     OBJ_TYPE_WEAPON,
//     OBJ_TYPE_ARMOUR,
//     OBJ_TYPE_PERSON,
//     OBJ_TYPE_MONSTER,
//     OBJ_TYPE_GROUND,
//     OBJ_TYPE_WALL,
//     OBJ_TYPE_DOOR,
//     OBJ_TYPE_RING,
//     OBJ_TYPE_FOOD,
//     OBJ_TYPE_SCROLL,
//   };

class Obj
{
public:
  Obj();
  ~Obj();
  void set_icon(unsigned);
  unsigned get_icon();
  void set_name(const char*);
  std::string get_name();

protected:
  unsigned _icon;
  std::string _name;
};

#endif
