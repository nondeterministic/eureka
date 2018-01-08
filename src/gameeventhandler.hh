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

#ifndef __GAMEEVENTHANDLER_HH
#define __GAMEEVENTHANDLER_HH

#include "gameevent.hh"
#include "eventermap.hh"
#include "eventleavemap.hh"
#include "eventprintcon.hh"
#include "eventchangeicon.hh"
#include "eventluascript.hh"
#include "eventplaysound.hh"
#include "map.hh"
#include "mapobj.hh"

#include <memory>

class GameEventHandler
{
public:
  GameEventHandler();
  bool handle(std::shared_ptr<GameEvent>, std::shared_ptr<Map>, MapObj* = NULL);
  bool handle_event_enter_map(std::shared_ptr<EventEnterMap>, std::shared_ptr<Map>);
  bool handle_event_leave_map(std::shared_ptr<EventLeaveMap>, std::shared_ptr<Map>);
  bool handle_event_change_icon(std::shared_ptr<EventChangeIcon>, std::shared_ptr<Map>);
  bool handle_event_printcon(std::shared_ptr<EventPrintcon>, std::shared_ptr<Map>);
  bool handle_event_playsound(std::shared_ptr<EventPlaySound>, std::shared_ptr<Map>);
  bool handle_event_playmusic(std::shared_ptr<EventPlayMusic>, std::shared_ptr<Map>);
  bool handle_event_lua_script(std::shared_ptr<EventLuaScript>, std::shared_ptr<Map>);
  bool handle_event_delete_object(std::shared_ptr<Map>, MapObj*);
  bool handle_event_add_object(std::shared_ptr<Map>, MapObj*);
  bool handle_event_add_object(std::shared_ptr<Map>, MapObj);
  // bool handle_event_play_sound(std::shared_ptr<Map>, MapObj*);
};

#endif
