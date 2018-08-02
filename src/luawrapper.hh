#ifndef __LUAWRAPPER_HH
#define __LUAWRAPPER_HH

#include <string>
#include <list>
#include <algorithm>
#include <vector>
#include <string>
#include <lua.h>
#include <boost/variant.hpp>

typedef boost::variant<std::string, double, bool> LuaT;

class visitor : public boost::static_visitor<>
{
private:
	lua_State* _ll;

public:
	visitor(lua_State* ll);

	void operator()(std::string& s) const;
	void operator()(double& s) const;
	void operator()(bool& s) const;
};

class LuaWrapper
{
private:
	lua_State* _l;
	std::list<LuaT> _args;
	int _initial_stack_size;

	int get_item_prop_getter(std::vector<std::string>);
	int make_fn_call(std::vector<std::string>, int);
	void handle_error(std::vector<std::string>);
	std::string object_path_to_string(std::vector<std::string>);

public:
	LuaWrapper(lua_State* state);
	~LuaWrapper();

	int get_stack_difference();
	void push_fn_arg(LuaT arg);
	template <class T> T call_fn(std::string, unsigned ret_vals = 1, bool handle_return = true);
	template <class T> T call_fn(std::vector<std::string>, unsigned ret_vals = 1, bool handle_return = true);
	void call_void_fn(std::vector<std::string>);
	void call_void_fn(std::string);
	int call_fn_leave_ret_alone(std::vector<std::string>, unsigned ret_vals = 1);

	template <class T> T get_item_prop(std::vector<std::string>);

	void set_item_prop(std::vector<std::string>);

	bool check_item_prop_is_nilornone(std::vector<std::string>);

	std::vector<std::string> get_strings_from_subtable(std::vector<std::string>);

	// bool is_defined(std::vector<std::string>);
	// bool hasEntry(std::vector<std::string>); // TODO: Rename!
	// std::vector<std::string> get_itemids_from_itemarray(std::string item_array); // TODO: Fix later!
	// std::vector<std::string> get_strings_from_subtable(std::string item_array, std::string item_id, std::string prop);
};

#endif
