#include <iostream>
#include "action.hh"
#include "actiononenter.hh"

ActionOnEnter::ActionOnEnter(int x, int y, const char* name)
{
  std::cout << "Added ActionOnEnter." << std::endl;
  _x = x;
  _y = y;
  _map_name = name;
}

ActionOnEnter::~ActionOnEnter()
{
}

std::string ActionOnEnter::name()
{
	return "ActionOnEnter";
}
