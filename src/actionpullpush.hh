#ifndef __ACTIONPULLPUSH_HH
#define __ACTIONPULLPUSH_HH

#include "action.hh"
#include <string>

class ActionPullPush : public Action
{
public:
  ActionPullPush(int, int, const char*);
  ActionPullPush(const char*);
  ~ActionPullPush();
  std::string name();
};

#endif
