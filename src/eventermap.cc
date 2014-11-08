#include "eventermap.hh"

EventEnterMap::EventEnterMap()
{
}

EventEnterMap::~EventEnterMap()
{
}

void EventEnterMap::set_x(unsigned x)
{
  _x = x;
}

void EventEnterMap::set_y(unsigned y)
{
  _y = y;
}

void EventEnterMap::set_map_name(const char* name)
{
  _map_name = name;
}

unsigned EventEnterMap::get_x(void)
{
  return _x;
}

unsigned EventEnterMap::get_y(void)
{
  return _y;
}

std::string EventEnterMap::get_map_name(void)
{
  return _map_name;
}
