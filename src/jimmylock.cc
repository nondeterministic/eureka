#include "jimmylock.hh"
#include "item.hh"

JimmyLock::JimmyLock()
{
	_name = "jimmy lock";
	_plural_name = "jimmy locks";
	_weight = 0;
}

/*
std::string JimmyLock::luaName()
{
	return ""; // "shields::" + name();
}
*/


JimmyLock::JimmyLock(const JimmyLock& g): Item(g)
{
	_name = "jimmy lock";
	_plural_name = "jimmy locks";
	_weight = 0;
}
