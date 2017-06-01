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
  bool handle_event_lua_script(std::shared_ptr<EventLuaScript>, std::shared_ptr<Map>);
  bool handle_event_delete_object(std::shared_ptr<Map>, MapObj*);
  bool handle_event_add_object(std::shared_ptr<Map>, MapObj*);
  bool handle_event_add_object(std::shared_ptr<Map>, MapObj);
  // bool handle_event_play_sound(std::shared_ptr<Map>, MapObj*);
};

#endif
