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
#include <memory>

#include "config.h"
#include "luawrapper.hh"
#include "luaapi.hh"
#include "charset.hh"
#include "console.hh"
#include "conversation.hh"
#include "world.hh"
#include "eureka.hh"
#include "party.hh"

Conversation::Conversation(MapObj& mo) : _map_obj(mo)
{
	// Make conversation self-contained, use a fresh Lua state!
	_lua_state = luaL_newstate();
	luaL_openlibs(_lua_state);
	publicize_api(_lua_state);
	World::Instance().init_lua_arrays(_lua_state);
}

Conversation::~Conversation()
{
    lua_close(_lua_state);
}

/**
 * Initiate a conversation with the MapObj given by "obj", defined in the constructor.
 * Conversation, in turn, is defined by an external Lua script.  This, in turn, is
 * stated in the map file of the corresponding person in the init_script XML-tag.
 */

void Conversation::initiate()
{
	if (!load_lua_conversation_file())
		return;

	_lua->call_void_fn("description");

	std::string reply;
	do {
		reply = boost::algorithm::to_lower_copy(Console::Instance().gets());

		if (reply == "job") {
			printcon(reply + " ");
			_lua->call_void_fn("job");
		}
		else if (reply == "name") {
			printcon(reply + " ");
			_lua->call_void_fn("name");
		}
		else if (reply == "join") {
			printcon(reply + " ");
			if (_lua->call_fn<bool>("join"))
				return;
		}
		else if (!(reply == "bye" || reply.length() == 0)) {
			printcon(reply + " ");
			_lua->push_fn_arg(reply);
			_lua->call_void_fn("otherwise");
		}

		if (_lua->call_fn<bool>("conversation_over"))
			return;

	} while (!(reply == "bye" || reply.length() == 0));

	printcon("bye ");
	_lua->call_void_fn("bye");
}

/// This lets dogs join the party, basically.

void Conversation::initiate_with_animal()
{
	if (!load_lua_conversation_file())
		return;

	// Create temporary character to get c_values from NPC...
	std::shared_ptr<PlayerCharacter> npc = create_character_values_from_lua(_lua_state);

	if (npc->race() == RACE::DOG && !Party::Instance().get_npc_or_null()) {
		if (npc->name().length() == 0) {
			printcon("The dog has taken a shine on you. Would you like him to join your party? (y/n)");
		}
		else {
			printcon(npc->name() + " has taken a shine on you. Would you like him to join your party? (y/n)");
		}
	}
	else {
		std::cerr << "ERROR: conversation.cc: It seems an animal conversation file is missing for " <<
				_map_obj.lua_name << "/ " << _map_obj.description() << "\n";
		printcon("Talking to animals can be so soothing...");
	}
}

/// Internal method to initialise the conversation files, lua stuff, etc.  Call first, converse later!

bool Conversation::load_lua_conversation_file()
{
	_lua = std::make_shared<LuaWrapper>(_lua_state);
	std::string lua_conversation_file = _map_obj.get_init_script_path();

	// Load corresponding Lua conversation file
	if (luaL_dofile(_lua_state, (conf_world_path / boost::algorithm::to_lower_copy(lua_conversation_file)).c_str())) {
		std::cerr << "ERROR: conversation.cc: Couldn't execute Lua file: " << lua_tostring(_lua_state, -1) << std::endl;
		printcon("You get no response.");
		return false;
	}

	return true;
}

void Conversation::printcon(const std::string s, bool wait)
{
	Console::Instance().print(&Charset::Instance(), s, wait);
}
