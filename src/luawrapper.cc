#include "luawrapper.hh"
#include <string>
#include <list>
#include <algorithm>
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

// Checks if Lua table array_name has an entry with member name.
// E.g. if array_name is "Shields" and name is "small shield", then it will return true.
// It will return false if name is "small shields", because "small shields" is merely the
// index name within the table.

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
	std::cerr << "Error: Check luawrapper.hh\n";
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
