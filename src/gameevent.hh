#ifndef __GAMEEVENT_HH
#define __GAMEEVENT_HH

enum EVENT_TYPES
{
	EVENT_CHANGE_ICON,     // E.g., when a lever is switched
    EVENT_CHANGE_MUSIC,
    EVENT_ENTER_MAP        // E.g., when a city or dungeon is entered
};

// TODO: Add field for precondition, which is a path to a lua program, returning true or false!

class GameEvent
{
public:
  GameEvent();
  virtual ~GameEvent();
};

#endif
