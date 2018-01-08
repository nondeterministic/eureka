// This source file is part of eureka
//
// Copyright (c) 2007-2018 Andreas Bauer <baueran@gmail.com>
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

#ifndef SERVICESHELPER_HH_
#define SERVICESHELPER_HH_

#include <string>

#include <lua.h>
#include <lualib.h>

#include "playercharacter.hh"
#include "service.hh"

class ServicesHelper
{
public:
  ServicesHelper();
  virtual ~ServicesHelper();
  static Service* createFromLua(std::string, lua_State*);
  static bool existsInLua(std::string, lua_State*);
  static void apply(Service*, int);
};

#endif
