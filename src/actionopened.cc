#include <iostream>
#include "action.hh"
#include "actionopened.hh"

ActionOpened::ActionOpened(const char* name)
{
	_name = name;
}

ActionOpened::~ActionOpened()
{
}

std::string ActionOpened::name()
{
	return _name;
}
