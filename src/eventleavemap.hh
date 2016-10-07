#ifndef __EVENT_LEAVE_MAP_HH
#define __EVENT_LEAVE_MAP_HH

#include <string>
#include "gameevent.hh"

class EventLeaveMap : public GameEvent
{
public:
  EventLeaveMap();
  ~EventLeaveMap();
  void set_x(unsigned);
  void set_y(unsigned);
  void set_map_name(const char*);
  void set_old_map_name(const char*);
  unsigned get_x();
  unsigned get_y();
  std::string get_map_name();
  std::string get_old_map_name();
  
protected:
  unsigned _x, _y;
  std::string _map_name;
  std::string _old_map_name;
};

#endif
