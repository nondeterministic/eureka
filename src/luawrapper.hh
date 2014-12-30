#ifndef __LUAWRAPPER_HH
#define __LUAWRAPPER_HH

#include <string>
#include <list>
#include <algorithm>
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
	void make_fn_call(int arguments, int return_values, int error = 0);
	void handle_error();

public:
	LuaWrapper(lua_State* state);
	~LuaWrapper();
	bool hasEntry(std::string, std::string);
	void push_fn_arg(LuaT arg);
	template <class T> T call_fn(std::string fn_name, unsigned ret_vals = 1, bool handle_return = true);
	template <class T> T get_item_prop(std::string item_array, std::string item_id, std::string prop);
	void call_void_fn(std::string fn_name);
	void call_fn_leave_ret_alone(std::string fn_name, int ret_vals = 1);
};

#endif
