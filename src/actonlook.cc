#include <iostream>
#include "action.hh"
#include "actonlook.hh"

ActionOnLook::ActionOnLook(int xx, int yy, const char* name)
{
	set_x(xx);
	set_y(yy);
	_name = name;
}

ActionOnLook::ActionOnLook(const char* name)
{
	_name = name;
}

ActionOnLook::~ActionOnLook()
{
	_name = "";
}

std::string ActionOnLook::name()
{
	return _name;
}
