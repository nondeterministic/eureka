// This source file is part of eureka
//
// Copyright (c) 2007-2018 Andreas Bauer <a@pspace.org>
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

#ifndef __GAMEEVENT_HH
#define __GAMEEVENT_HH

enum EVENT_TYPES
{
	EVENT_CHANGE_ICON,     // E.g., when a lever is switched
    EVENT_CHANGE_MUSIC,
    EVENT_ENTER_MAP        // E.g., when a city or dungeon is entered
};

// TODO: Add field for precondition, which is a path to a lua program, returning true or false!

class GameEvent
{
public:
  GameEvent();
  virtual ~GameEvent();
};

#endif
