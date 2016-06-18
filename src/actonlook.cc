#include <iostream>
#include "action.hh"
#include "actonlook.hh"

ActionOnLook::ActionOnLook(int x, int y, const char* name)
{
	this->x = x;
	this->y = y;
	_name = name;
}

ActionOnLook::ActionOnLook(const char* name)
{
	x = -1;
	y = -1;
	_name = name;
}

ActionOnLook::~ActionOnLook()
{
	x = -1;
	y = -1;
	_name = "";
}

std::string ActionOnLook::name()
{
	return _name;
}
