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

#include "luawrapper.hh"
#include <string>
#include <list>
#include <algorithm>
#include <iostream>

extern "C"
{
#include <lua.h>
}
#include <boost/variant.hpp>

visitor::visitor(lua_State* ll)
{
	l = ll;
}

void visitor::operator()(std::string& s) const
{
	lua_pushstring(l, s.c_str());
}

void visitor::operator()(double& s) const
{
	lua_pushnumber(l, s);
}

void visitor::operator()(bool& s) const
{
	lua_pushboolean(l, s);
}

LuaWrapper::LuaWrapper(lua_State* state)
{
	l = state;
}

void LuaWrapper::push_fn_arg(LuaT arg)
{
	args.push_front(arg);
}

/**
 * Let's assume, you'd like to get from Lua-land the following: item_array[item_id].prop,
 * e.g., Weapons[axe].weight.  After calling the property, prop, is on top of the Lua stack.
 */

void LuaWrapper::get_item_prop_getter(std::string item_array, std::string item_id, std::string prop)
{
	// Let's assume we want to get Weapons[axe].name...

	lua_getglobal(l, item_array.c_str()); // Push "Weapons" on stack
    lua_pushstring(l, item_id.c_str());   // Push item_id on stack, e.g., "axe"
    lua_gettable(l, -2);                  // Remove item_id from stack and put Weapons[item_id] in its place
                                          // Stack from top to bot: *<item_id>, Weapons

    lua_pushstring(l, prop.c_str());      // Push weapon property name on stack, e.g., "name"
    lua_gettable(l, -2);                  // Remove weapon property name, and put actual property in its place on top of stack
    									  // Stack from top to bot: <name>, *<item_id>, Weapons
}

void LuaWrapper::get_item_prop_cleaner()
{
	lua_pop(l, 3);
}

template <>
std::string LuaWrapper::get_item_prop(std::string item_array, std::string item_id, std::string prop)
{
	// After this, the result lies on top of the lua stack, see get_item_prop_getter
	get_item_prop_getter(item_array, item_id, prop);

	// Now get string result and clear up stack again.
    std::string result = (std::string)lua_tostring(l, -1);

    // Now clean up stack again
    get_item_prop_cleaner();

    return result;
}

template <>
double LuaWrapper::get_item_prop(std::string item_array, std::string item_id, std::string prop)
{
	// After this, the result lies on top of the lua stack, see get_item_prop_getter
	get_item_prop_getter(item_array, item_id, prop);

	// Now get string result and clear up stack again.
    double result = (double)lua_tonumber(l, -1);

    // Now clean up stack again
    get_item_prop_cleaner();

    return result;
}

template <>
bool LuaWrapper::get_item_prop(std::string item_array, std::string item_id, std::string prop)
{
	// After this, the result lies on top of the lua stack, see get_item_prop_getter
	get_item_prop_getter(item_array, item_id, prop);

	// Now get string result and clear up stack again.
    bool result = (bool)lua_toboolean(l, -1);

    // Now clean up stack again
    get_item_prop_cleaner();

    return result;
}

/// Right now, really only called by Potion, which uses it to obtain a list of ingredient strings.

std::vector<std::string> LuaWrapper::get_strings_from_subtable(std::string item_array, std::string item_id, std::string prop)
{
	std::vector<std::string> ingredients;

	// After this, the result lies on top of the lua stack, see get_item_prop_getter
	get_item_prop_getter(item_array, item_id, prop);

    lua_pushnil(l); // Push nil, so lua_next removes it from stack and puts (k, v) on stack
	while (lua_next(l, -2) != 0) {
		// std::string key = lua_tostring(l, -2);
		//
		// It is super weird, that the above call fails.  I found this phenomenon described here:
		// http://stackoverflow.com/questions/22052579/how-to-validate-lua-table-keys-from-c
		// It is, because there isn't a string key, and this confuses the call to lua_next.
		//
		// If the table is like { "a", "b" } instead of { "key1" = "a", "key2" = "b" }, then
		// Lua automatically assigns a numeric key to "a", "b".  So, lua_tostring(l, -2)
		// returns a number for key 1, "1", and then the next call to lua_next crashes.
		std::string val = lua_tostring(l, -1);
		ingredients.push_back(val);

		lua_pop(l, 1); // Remove value, keep key for next iteration.
	}
    lua_pop(l, 2); // Pop table

    return ingredients;
}

/**
 * Let's say, you have in Lua an array Edibles = { sausage, beer, ... }, where sausage, beer, etc.
 * are further defined as all the items inside the data/ directory.  Then calling this function
 * will return a vector containing the strings "sausage", "beer", etc.
 */

std::vector<std::string> LuaWrapper::get_itemids_from_itemarray(std::string item_array)
{
	std::vector<std::string> itemids;

	lua_getglobal(l, item_array.c_str()); // Push, say, "Weapons" on stack.
	lua_pushnil(l);
	while (lua_next(l, -2) != 0) {
		std::string key = lua_tostring(l, -2); // Get key, ignore value at -1.
		itemids.push_back(key);
		lua_pop(l, 1);  // Pop value, keep key palceholder for next iteration.
	}
    lua_pop(l, 1); // Pop entire table. TODO: Above always pops 2 at the end.  I am not sure what is correct. Check!

	return itemids;
}


/// Checks if Lua table array_name has an entry with member name.
/// E.g. if array_name is "Shields" and name is "small shield", then it will return true.
/// It will return false if name is "small shields", because "small shields" is merely the
/// index name within the table.

bool LuaWrapper::hasEntry(std::string array_name, std::string name)
{
	bool found = false;
	lua_getglobal(l, array_name.c_str()); // Push array_name on stack

	// See example at http://www.lua.org/manual/5.1/manual.html#lua_next

    lua_pushnil(l);  // Reserve space for first key
    while (lua_next(l, -2) != 0) {
    	if (get_item_prop<std::string>(array_name, lua_tostring(l, -2), "name") == name) {
    		lua_pop(l, 1);
    		found = true;
    		break;
    	}
    	lua_pop(l, 1);
    }

    lua_pop(l, 2);
    return found;
}

template <class T>
T LuaWrapper::call_fn(std::string fn_name, unsigned ret_vals, bool handle_return)
{
	std::cerr << "ERROR: luawrapper.cc: Check luawrapper.hh!\n";
	exit(EXIT_FAILURE);
	return NULL;
}

template <>
std::string LuaWrapper::call_fn(std::string fn_name, unsigned ret_vals, bool handle_return)
{
	visitor vis(l);

	// Push function name on stack
	lua_getglobal(l, fn_name.c_str());

	// Push arguments, if any, on stack
	for (auto itr = args.begin(); itr != args.end(); itr++)
		boost::apply_visitor(vis, *itr);
	make_fn_call(args.size(), ret_vals);
	args.clear();

	if (ret_vals > 0 && handle_return) {
		std::string result = lua_tostring(l, -1);
		return result;
	}
	else {
		std::string dummy = "";
		return dummy; // Doesn't matter, as return value is ignored by caller
	}
}

template <>
double LuaWrapper::call_fn(std::string fn_name, unsigned ret_vals, bool handle_return)
{
	visitor vis(l);

	// Push function name on stack
	lua_getglobal(l, fn_name.c_str());

	// Push arguments, if any, on stack
	for (auto itr = args.begin(); itr != args.end(); itr++)
		boost::apply_visitor(vis, *itr);
	make_fn_call(args.size(), ret_vals);
	args.clear();

	if (ret_vals > 0 && handle_return) {
		double result = lua_tonumber(l, -1);
		return result;
	}
	else {
		return 0.0; // Doesn't matter, as return value is ignored by caller
	}
}

template <>
bool LuaWrapper::call_fn(std::string fn_name, unsigned ret_vals, bool handle_return)
{
	visitor vis(l);

	// Push function name on stack
	lua_getglobal(l, fn_name.c_str());

	// Push arguments, if any, on stack
	for (auto itr = args.begin(); itr != args.end(); itr++)
		boost::apply_visitor(vis, *itr);
	make_fn_call(args.size(), ret_vals);
	args.clear();

	if (ret_vals > 0 && handle_return) {
		bool result = lua_toboolean(l, -1);
		return result;
	}
	else {
		return false;  // Doesn't matter, as return value is ignored by caller
	}
}

void LuaWrapper::call_void_fn(std::string fn_name)
{
	call_fn<std::string>(fn_name, 0);
}

void LuaWrapper::call_fn_leave_ret_alone(std::string fn_name, int ret_vals)
{
	call_fn<std::string>(fn_name, ret_vals, false);
}

void LuaWrapper::make_fn_call(int arguments, int return_values, int error)
{
	if (lua_pcall(l, arguments, return_values, error) != 0)
		handle_error();
}

void LuaWrapper::handle_error()
{
	std::cerr << "ERROR: luawrapper.cc: Lua: '" << lua_tostring(l, -1) << "'. Perhaps a function was called that is not defined in the script?!" << std::endl;
	std::cerr << "ERROR: luawrapper.cc: Quitting program now as a consequence.\n";
	exit(0);
}

bool LuaWrapper::is_defined(std::string fn_name)
{
	bool global_function_is_defined = false;

	lua_getglobal(l, fn_name.c_str());

	if (lua_isfunction(l, lua_gettop(l)))
		global_function_is_defined = true;
	lua_pop(l, 1);

	return global_function_is_defined;
}
