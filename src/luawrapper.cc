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

#include <string>
#include <sstream>
#include <list>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <lua.h>
#include "luawrapper.hh"

visitor::visitor(lua_State* ll)
{
	_ll = ll;
}

void visitor::operator()(std::string& s) const
{
	lua_pushstring(_ll, s.c_str());
}

void visitor::operator()(double& s) const
{
	lua_pushnumber(_ll, s);
}

void visitor::operator()(bool& s) const
{
	lua_pushboolean(_ll, s);
}

LuaWrapper::LuaWrapper(lua_State* state)
{
	_l = state;
	_initial_stack_size = lua_gettop(_l);
}

LuaWrapper::~LuaWrapper()
{
	int current_stack_size = lua_gettop(_l);
	int stack_difference = std::abs(std::abs(current_stack_size) - std::abs(_initial_stack_size));
	if (current_stack_size != _initial_stack_size) {
		std::cerr << "WARNING: ~LuaWrapper: Lua stack size mismatch by "
				  << stack_difference
				  << ".\n";
	}
}

/// Convenience function to check current stack situation.  How much stack leak was caused,
/// since the constructor was called?

int LuaWrapper::get_stack_difference()
{
	return std::abs(std::abs(_initial_stack_size) - std::abs(lua_gettop(_l)));
}

/// Return Lua stack size for subsequent lua_pop-call.

int LuaWrapper::get_item_prop_getter(std::vector<std::string> object_path)
{
	if (object_path.size() == 0) {
		std::cerr << "WARNING: luawrapper: get_item_prop_getter's object_path size is 0.\n";
		return 0;
	}

	lua_getglobal(_l, object_path[0].c_str());  // The first object must always be in global scope, or we have no chance

	unsigned i = 1;
	for (; i < object_path.size(); i++) {
		lua_pushstring(_l, object_path[i].c_str());   // Push next element on stack, e.g., "orc"
		lua_gettable(_l, -2);                         // Remove last element from stack and put e.g. Bestiary["orc"] in its place
	}

	return i;
}

void LuaWrapper::set_item_prop(std::vector<std::string> object_path)
{
	int stack = get_item_prop_getter(object_path);

	// Push argument on stack
	if (_args.size() != 1) {
		std::cerr << "ERROR: luawrapper.cc: set_item_prop expects exactly 1 argument on Lua stack before being called. Found: " <<
				_args.size() << ". Aborting call '" << object_path.back() << "'.\n";
		_args.clear();
		lua_pop(_l, stack);
		return;
	}

	lua_pushstring(_l, object_path.back().c_str());

	visitor vis(_l);
	boost::apply_visitor(vis, _args.front());

	lua_settable(_l, -4);
	_args.clear();
	lua_pop(_l, stack);
}

template <>
std::string LuaWrapper::get_item_prop(std::vector<std::string> object_path)
{
	int stack = get_item_prop_getter(object_path);
	std::string result = (std::string)lua_tostring(_l, -1);
	lua_pop(_l, stack);
	return result;
}

template <>
double LuaWrapper::get_item_prop(std::vector<std::string> object_path)
{
	int stack = get_item_prop_getter(object_path);
	double result = (double)lua_tonumber(_l, -1);
	lua_pop(_l, stack);
	return result;
}

template <>
bool LuaWrapper::get_item_prop(std::vector<std::string> object_path)
{
	int stack = get_item_prop_getter(object_path);
	bool result = (bool)lua_toboolean(_l, -1);
	lua_pop(_l, stack);
	return result;
}

bool LuaWrapper::check_item_prop_is_nilornone(std::vector<std::string> object_path)
{
	int stack = get_item_prop_getter(object_path);
	bool return_value = lua_isnoneornil(_l,-1) == 1;
	lua_pop(_l, stack);
	return return_value;
}

void LuaWrapper::handle_error(std::vector<std::string> object_path)
{
	std::cerr << "ERROR: luawrapper.cc: Lua: '" << lua_tostring(_l, -1) << "'. Perhaps the function '" + object_path.back() +
			"' that was called is not defined in the corresponding script?!" << std::endl;
	std::cerr << "ERROR: luawrapper.cc: Quitting program now as a consequence.\n";
	exit(EXIT_FAILURE);
}

void LuaWrapper::push_fn_arg(LuaT arg)
{
	_args.push_front(arg);
}

std::string LuaWrapper::object_path_to_string(std::vector<std::string> object_path)
{
	std::stringstream ss;

	for (auto object: object_path)
		ss << object << ".";

	return ss.str().substr(0, ss.str().size()-1);
}

int LuaWrapper::make_fn_call(std::vector<std::string> object_path, int ret_vals)
{
	visitor vis(_l);

	// Push function name on stack
	int stack_size = get_item_prop_getter(object_path);

	// Push arguments, if any, on stack
	for (auto itr = _args.begin(); itr != _args.end(); itr++)
		boost::apply_visitor(vis, *itr);

	// According to docs, lua_pcall returns function + arguments from Lua stack itself.  So no further popping necessary.
	if (lua_pcall(_l, _args.size(), ret_vals, 0) != 0)
		handle_error(object_path);
	_args.clear();

	return stack_size;
}

template <>
std::string LuaWrapper::call_fn(std::vector<std::string> object_path, unsigned ret_vals, bool handle_return)
{
	int stack = make_fn_call(object_path, ret_vals);

	if (ret_vals > 0 && handle_return) {
		std::string result = lua_tostring(_l, -1);
		lua_pop(_l, stack);
		return result;
	}
	else {
		std::string dummy = "";
		lua_pop(_l, stack);
		return dummy; // Doesn't matter, as return value is ignored by caller
	}
}

template <>
double LuaWrapper::call_fn(std::vector<std::string> object_path, unsigned ret_vals, bool handle_return)
{
	int stack = make_fn_call(object_path, ret_vals);

	if (ret_vals > 0 && handle_return) {
		double result = lua_tonumber(_l, -1);
		lua_pop(_l, stack);
		return result;
	}
	else {
		double dummy = 0.0;
		lua_pop(_l, stack);
		return dummy; // Doesn't matter, as return value is ignored by caller
	}
}

template <>
bool LuaWrapper::call_fn(std::vector<std::string> object_path, unsigned ret_vals, bool handle_return)
{
	int stack = make_fn_call(object_path, ret_vals);

	if (ret_vals > 0 && handle_return) {
		bool result = lua_toboolean(_l, -1);
		lua_pop(_l, stack);
		return result;
	}
	else {
		bool dummy = false;
		lua_pop(_l, stack);
		return dummy; // Doesn't matter, as return value is ignored by caller
	}
}

template <class T>
T LuaWrapper::call_fn(std::vector<std::string> object_path, unsigned ret_vals, bool handle_return)
{
	std::cout << "SHIT HAPPENED\n";
	exit(-1);
}

template <>
std::string LuaWrapper::call_fn(std::string object_path, unsigned ret_vals, bool handle_return)
{
	std::vector<std::string> v { object_path };
	return call_fn<std::string>(v, ret_vals, handle_return);
}

template <>
double LuaWrapper::call_fn(std::string object_path, unsigned ret_vals, bool handle_return)
{
	std::vector<std::string> v { object_path };
	return call_fn<double>(v, ret_vals, handle_return);
}

template <>
bool LuaWrapper::call_fn(std::string object_path, unsigned ret_vals, bool handle_return)
{
	std::vector<std::string> v { object_path };
	return call_fn<bool>(v, ret_vals, handle_return);
}

template <class T>
T LuaWrapper::call_fn(std::string object_path, unsigned ret_vals, bool handle_return)
{
	std::cout << "SHIT HAPPENED\n";
	exit(-1);
}

void LuaWrapper::call_void_fn(std::vector<std::string> object_path)
{
	int stack_before = lua_gettop(_l);

	int additional_stack = make_fn_call(object_path, 0);
	lua_pop(_l, additional_stack - 1);

	int stack_after = lua_gettop(_l);
	int difference = std::abs(std::abs(stack_before) - std::abs(stack_after));

	if (difference != 0) {
		std::cerr << "WARNING: luawarapper.cc: Lua stack size changed by "
				  << difference
				  << " after call_void_fn to "
				  << object_path_to_string(object_path) << ".\n";
		std::cerr << "Stack height: " << lua_gettop(_l) << "\n";
	}
}

void LuaWrapper::call_void_fn(std::string object_path)
{
	std::vector<std::string> v { object_path };
	call_void_fn(v);
}

int LuaWrapper::call_fn_leave_ret_alone(std::vector<std::string> object_path, unsigned ret_vals)
{
	int stack = make_fn_call(object_path, ret_vals);
	// Rather than lua_pop(_l, stack); we just return the stack size, as we leave the return value alone.
	// The caller can (and must) clean up the Lua stack!
	return stack;
}

/// Right now, really only called by Potion, which uses it to obtain a list of ingredient strings.

std::vector<std::string> LuaWrapper::get_strings_from_subtable(std::vector<std::string> object_path)
{
  std::vector<std::string> ingredients;

  // After this, the result lies on top of the lua stack, see get_item_prop_getter
  int stack = get_item_prop_getter(object_path);

  lua_pushnil(_l); // Push nil, so lua_next removes it from stack and puts (k, v) on stack
  while (lua_next(_l, -2) != 0) {
    // std::string key = lua_tostring(l, -2);
    //
    // It is super weird, that the above call fails.  I found this phenomenon described here:
    // http://stackoverflow.com/questions/22052579/how-to-validate-lua-table-keys-from-c
    // It is, because there isn't a string key, and this confuses the call to lua_next.
    //
    // If the table is like { "a", "b" } instead of { "key1" = "a", "key2" = "b" }, then
    // Lua automatically assigns a numeric key to "a", "b".  So, lua_tostring(l, -2)
    // returns a number for key 1, "1", and then the next call to lua_next crashes.
    std::string val = lua_tostring(_l, -1);
    ingredients.push_back(val);

    lua_pop(_l, 1); // Remove value, keep key for next iteration.
  }
  lua_pop(_l, stack); // Pop table

  return ingredients;
}
