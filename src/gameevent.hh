#ifndef __GAMEEVENT_HH
#define __GAMEEVENT_HH

enum EVENT_TYPES
  {
    EVENT_CHANGE_ICONS,    // E.g., when a lever is switched
    EVENT_CHANGE_MUSIC,
    EVENT_ENTER_MAP        // E.g., when a city or dungeon is entered
  };

class GameEvent
{
public:
  GameEvent();
  virtual ~GameEvent();
};

#endif
