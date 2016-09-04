#ifndef __ACTION_HH
#define __ACTION_HH

#include <string>
#include <vector>
#include <memory>
#include "gameevent.hh"

// An action triggers a set of events.  An action has a particular
// type, i.e., a type can be thought of as a trigger:

enum ACTION_TYPES
{
	ACT_ON_PULLPUSH,          // Triggers when player pulls/pushes icon
	ACT_ON_ENTER,             // Triggers when player enters on a certain icon
	ACT_ON_REACH,             // Triggers when player reaches a certain icon
	ACT_ON_LOOK,              // Triggers when player searches icon
	ACT_ON_SEARCH
};

class Action
{
public:
  Action();
  virtual ~Action();

  virtual std::string name();
  unsigned get_x();
  unsigned get_y();
  void set_x(unsigned);
  void set_y(unsigned);
  void add_event(std::shared_ptr<GameEvent>);
  std::vector<std::shared_ptr<GameEvent>>::iterator events_begin();
  std::vector<std::shared_ptr<GameEvent>>::iterator events_end();

protected:
  unsigned _x, _y;     // Map coordinates where the action sits
  std::string _name;
  // These are the events that are triggered by the action
  std::vector<std::shared_ptr<GameEvent>> _events;
};

#endif
