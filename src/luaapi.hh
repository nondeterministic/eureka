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

#ifndef __LUAAPI_HH
#define __LUAAPI_HH

#include "playercharacter.hh"

#include <type_traits>
#include <string>

#include <boost/lexical_cast.hpp>

extern "C" 
{
#include <lua.h>
}

// int l_pushweapon(lua_State*);
// int l_pushshield(lua_State*);
// int l_printcon(lua_State*);
// int l_datapath(lua_State*);

void publicize_api(lua_State*);
std::shared_ptr<PlayerCharacter> create_character_values_from_lua(lua_State* L);


extern lua_State* _lua_state;

#endif
