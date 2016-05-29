/* *********************************************************************
 * This file is part of eureka/Leibniz.
 *
 * Copyright (c) Andreas Bauer <baueran@gmail.com>
 *
 * eureka/Leibniz is free software: you can redistribute it
 * and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * eureka/Leibniz is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with eureka/Leibniz.  If not, see 
 * <http://www.gnu.org/licenses/>. 
 * ********************************************************************* */

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
