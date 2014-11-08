#ifndef __ACTIONONENTER_HH
#define __ACTIONONENTER_HH

#include "action.hh"
#include <string>

class ActionOnEnter : public Action
{
public:
  ActionOnEnter(int, int, const char*);
  ~ActionOnEnter();
  std::string name();
};

#endif
