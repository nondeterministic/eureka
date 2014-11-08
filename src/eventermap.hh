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
