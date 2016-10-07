#include "eventleavemap.hh"

EventLeaveMap::EventLeaveMap()
{
	_x = -1; _y = -1;
	_map_name = ""; _old_map_name = "";
}

EventLeaveMap::~EventLeaveMap()
{
}

void EventLeaveMap::set_x(unsigned x)
{
  _x = x;
}

void EventLeaveMap::set_y(unsigned y)
{
  _y = y;
}

unsigned EventLeaveMap::get_x(void)
{
  return _x;
}

unsigned EventLeaveMap::get_y(void)
{
  return _y;
}

std::string EventLeaveMap::get_map_name(void)
{
  return _map_name;
}

void EventLeaveMap::set_map_name(const char* name)
{
  _map_name = name;
}

std::string EventLeaveMap::get_old_map_name(void)
{
  return _old_map_name;
}

void EventLeaveMap::set_old_map_name(const char* name)
{
  _old_map_name = name;
}
