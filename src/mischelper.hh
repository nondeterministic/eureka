// This source file is part of eureka
//
// Copyright (c) 2007-2016  Andreas Bauer <baueran@gmail.com>
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

#ifndef MISCHELPER_HH_
#define MISCHELPER_HH_

#include <string>

#include "miscitem.hh"
#include "mapobj.hh"

#include <lua.h>
#include <lualib.h>

class MiscHelper
{
public:
  MiscHelper();
  virtual ~MiscHelper();
  static MiscItem* createFromLua(std::string, lua_State*, MapObj* = NULL);
  static bool existsInLua(std::string, lua_State*);
  //  static void apply(MiscItem*, int);
};

#endif
