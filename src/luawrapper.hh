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

#ifndef __LUAWRAPPER_HH
#define __LUAWRAPPER_HH

#include <string>
#include <list>
#include <algorithm>
#include <vector>
#include <string>

extern "C"
{
#include <lua.h>
}
#include <boost/variant.hpp>

typedef boost::variant<std::string, double, bool> LuaT;

class visitor : public boost::static_visitor<>
{
private:
	lua_State* l;

public:
	visitor(lua_State* ll);

	void operator()(std::string& s) const;
	void operator()(double& s) const;
	void operator()(bool& s) const;
};

class LuaWrapper
{
private:
	lua_State* l;
	std::list<LuaT> args;

	void get_item_prop_getter(std::string item_array, std::string item_id, std::string prop);
	void get_item_prop_cleaner();
	void make_fn_call(std::string fn_name, int arguments, int return_values, int error = 0);
	void handle_error(std::string);

public:
	LuaWrapper(lua_State* state);
	bool hasEntry(std::string, std::string);
	void push_fn_arg(LuaT arg);
	template <class T> T call_fn(std::string fn_name, unsigned ret_vals = 1, bool handle_return = true);
	template <class T> T get_item_prop(std::string item_array, std::string item_id, std::string prop);
	std::vector<std::string> get_strings_from_subtable(std::string item_array, std::string item_id, std::string prop);
	std::vector<std::string> get_itemids_from_itemarray(std::string item_array);
	void call_void_fn(std::string fn_name);
	void call_fn_leave_ret_alone(std::string fn_name, int ret_vals = 1);
	bool is_defined(std::string);

};

#endif
