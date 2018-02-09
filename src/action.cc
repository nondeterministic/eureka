// This source file is part of eureka
//
// Copyright (c) 2007-2018 Andreas Bauer <baueran@gmail.com>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
// USA.

#include <stdexcept>
#include <iostream>
#include <memory>
#include <algorithm>

#include "action.hh"
#include "gameevent.hh"
#include "eventdeleteobj.hh"

Action::Action()
{
  _x = -1; _y = -1;
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

	// We sort the events such that delete events are always last.
	// This is important when actions are executed, and the user puts delete of the object as first event of the action.
	// Then subsequent ones would fail as the object's already gone when it gets to them...
	std::sort(_events.begin(), _events.end(), [](const auto& lhs, const auto& rhs) {
		if (std::dynamic_pointer_cast<EventDeleteObject>(lhs))
			return false;
		return true;
	});
}

std::vector<std::shared_ptr<GameEvent>>::iterator Action::events_begin()
{
  return _events.begin();
}

std::vector<std::shared_ptr<GameEvent>>::iterator Action::events_end()
{
  return _events.end();
}

int Action::get_number_of_events()
{
	return _events.size();
}

const std::vector<std::shared_ptr<GameEvent>>& Action::get_events() const
{
	return _events;
}
