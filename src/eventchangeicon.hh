#ifndef __EVENT_CHANGE_ICON_HH
#define __EVENT_CHANGE_ICON_HH

#include <string>
#include "gameevent.hh"

class EventChangeIcon : public GameEvent
{
public:
  EventChangeIcon();
  ~EventChangeIcon();
  int x, y, icon_now, icon_new;
};

#endif
