#ifndef __GAMEEVENTHANDLER_HH
#define __GAMEEVENTHANDLER_HH

#include "gameevent.hh"
#include "eventermap.hh"
#include "eventprintcon.hh"
#include "eventchangeicon.hh"
#include "eventluascript.hh"
#include "map.hh"

#include <memory>

class GameEventHandler
{
public:
  GameEventHandler();
  bool handle(std::shared_ptr<GameEvent>, std::shared_ptr<Map>);
  bool handle_event_enter_map(std::shared_ptr<EventEnterMap>, std::shared_ptr<Map>);
  bool handle_event_change_icon(std::shared_ptr<EventChangeIcon>, std::shared_ptr<Map>);
  bool handle_event_printcon(std::shared_ptr<EventPrintcon>, std::shared_ptr<Map>);
  bool handle_event_lua_script(std::shared_ptr<EventLuaScript>, std::shared_ptr<Map>);
};

#endif
