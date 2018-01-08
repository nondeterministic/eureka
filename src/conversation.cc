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

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lualib.h>
#include <lauxlib.h>
}
#include <boost/algorithm/string.hpp>
#include <string>
#include <iostream>

#include "config.h"
#include "luawrapper.hh"
#include "luaapi.hh"
#include "charset.hh"
#include "console.hh"
#include "conversation.hh"
#include "eureka.hh"

Conversation::Conversation(MapObj& mo) : obj(mo)
{
}

/**
 * Initiate a conversation with the MapObj given by "obj", defined in the constructor.
 * Conversation, in turn, is defined by an external Lua script.  This, in turn, is
 * stated in the map file of the corresponding person in the init_script XML-tag.
 */

void Conversation::initiate()
{
	LuaWrapper lua(_lua_state);
	std::string lua_conversation_file = obj.get_init_script_path();

	// Load corresponding Lua conversation file
	if (luaL_dofile(_lua_state, (conf_world_path / boost::algorithm::to_lower_copy(lua_conversation_file)).c_str())) {
		std::cerr << "ERROR: conversation.cc: Couldn't execute Lua file: " << lua_tostring(_lua_state, -1) << std::endl;
		printcon("It seems the person can hear you, but cannot be bothered to respond...");
		return;
		// exit(EXIT_FAILURE);
	}

	lua.call_void_fn("description");

	std::string reply;
	do {
		reply = boost::algorithm::to_lower_copy(Console::Instance().gets());

		if (reply == "job") {
			printcon(reply + " ");
			lua.call_void_fn("job");
		}
		else if (reply == "name") {
			printcon(reply + " ");
			lua.call_void_fn("name");
		}
		else if (reply == "join") {
			printcon(reply + " ");
			if (lua.call_fn<bool>("join"))
				return;
		}
		else if (!(reply == "bye" || reply.length() == 0)) {
			printcon(reply + " ");
			lua.push_fn_arg(reply);
			lua.call_void_fn("otherwise");
		}

		if (lua.call_fn<bool>("conversation_over"))
			return;

	} while (!(reply == "bye" || reply.length() == 0));

	printcon("bye ");
	lua.call_void_fn("bye");
}

void Conversation::printcon(const std::string s, bool wait)
{
	Console::Instance().print(&Charset::Instance(), s, wait);
}
