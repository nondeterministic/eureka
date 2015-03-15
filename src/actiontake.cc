#include <iostream>
#include "action.hh"
#include "actiontake.hh"

ActionOnTake::ActionOnTake(const char* name)
{
  _name = name;
}

ActionOnTake::~ActionOnTake()
{
}

std::string ActionOnTake::name()
{
	return _name;
}
