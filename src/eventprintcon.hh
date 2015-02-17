#ifndef __EVENT_PRINTCON_HH
#define __EVENT_PRINTCON_HH

#include <string>
#include "gameevent.hh"

class EventPrintcon : public GameEvent
{
public:
  EventPrintcon();
  ~EventPrintcon();
  std::string text;
};

#endif
