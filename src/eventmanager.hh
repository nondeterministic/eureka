//
//
// Copyright (c) 2005  Andreas Bauer <baueran@gmail.com>
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

#ifndef EVENTMANAGER_HH
#define EVENTMANAGER_HH

#include "SDL.h"
#include <list>
#include <boost/signals.hpp>

// typedef boost::signal<void (SDL_Event*)> key_event;
// typedef boost::signal<void ()>           tick_event;
// typedef boost::signals::connection       connection_t;

enum CustomEvent 
{
  // whatever timed events you want
  TICK,
  DRAW,
};

class EventManager
{
protected:
  EventManager();

public:
  static EventManager& Instance();
  SDL_TimerID add_event(Uint32, SDL_NewTimerCallback, void*);
  char get_key(const char* = NULL);
  SDLKey get_generic_key(std::list<SDLKey>);
};

#endif
