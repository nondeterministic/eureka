#include <iostream>
#include "action.hh"
#include "actiononenter.hh"

ActionOnEnter::ActionOnEnter(int x, int y, const char* name)
{
  _x = x;
  _y = y;
  _name = name;
}

ActionOnEnter::~ActionOnEnter()
{
}

std::string ActionOnEnter::name()
{
	return _name;
}
