#ifndef __EVENT_PLAYSOUND_HH
#define __EVENT_PLAYSOUND_HH

#include <string>
#include "gameevent.hh"

class EventPlaySound : public GameEvent
{
public:
  EventPlaySound(std::string);
  ~EventPlaySound();
  std::string filename;
};

#endif
