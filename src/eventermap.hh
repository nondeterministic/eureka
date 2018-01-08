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

#ifndef __EVENT_ENTER_MAP_HH
#define __EVENT_ENTER_MAP_HH

#include <string>
#include "gameevent.hh"

class EventEnterMap : public GameEvent
{
public:
  EventEnterMap();
  ~EventEnterMap();
  void set_x(unsigned);
  void set_y(unsigned);
  void set_map_name(const char*);
  unsigned get_x();
  unsigned get_y();
  std::string get_map_name();
  
protected:
  unsigned _x, _y;
  std::string _map_name;
};

#endif
