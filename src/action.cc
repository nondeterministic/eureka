#include <stdexcept>
#include <iostream>
#include <memory>
#include "action.hh"
#include "gameevent.hh"

Action::Action()
{
  _x = 0; _y = 0;
}

Action::~Action()
{
  _events.clear();
}

std::string Action::name()
{
	return "Action";
}

unsigned Action::get_x(void)
{
  return _x;
}

unsigned Action::get_y(void)
{
  return _y;
}

void Action::set_x(unsigned val)
{
  _x = val;
}

void Action::set_y(unsigned val)
{
  _y = val;
}

void Action::add_event(std::shared_ptr<GameEvent> ev)
{
	_events.push_back(ev);
}

std::vector<std::shared_ptr<GameEvent>>::iterator Action::events_begin()
{
  return _events.begin();
}

std::vector<std::shared_ptr<GameEvent>>::iterator Action::events_end()
{
  return _events.end();
}
