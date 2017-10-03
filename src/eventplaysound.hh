#ifndef __EVENT_PLAYSOUND_HH
#define __EVENT_PLAYSOUND_HH

#include <string>
#include "gameevent.hh"

class EventPlaySound : public GameEvent
{
public:
  EventPlaySound(std::string, int, int);
  ~EventPlaySound();
  std::string filename;
  int loop;
  int volume;
};

// TODO: Make this own class!

class EventPlayMusic: public GameEvent
{
public:
  EventPlayMusic(std::string, int, int);
  ~EventPlayMusic();
  std::string filename;
  int loop;
  int volume;
};

#endif
