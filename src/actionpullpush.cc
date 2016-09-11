#include <iostream>
#include "action.hh"
#include "actionpullpush.hh"

ActionPullPush::ActionPullPush(int x, int y, const char* name)
{
  _x = x;
  _y = y;
  _name = name;
}

ActionPullPush::ActionPullPush(const char* name)
{
  _name = name;
}

ActionPullPush::~ActionPullPush()
{
}

std::string ActionPullPush::name()
{
	return _name;
}
