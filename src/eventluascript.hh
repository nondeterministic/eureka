#ifndef __EVENT_LUA_SCRIPT_HH
#define __EVENT_LUA_SCRIPT_HH

#include <string>
#include "gameevent.hh"

class EventLuaScript : public GameEvent
{
public:
  EventLuaScript();
  ~EventLuaScript();
  std::string file_name;
};

#endif
