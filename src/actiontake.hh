#ifndef __ACTIONTAKE_HH
#define __ACTIONTAKE_HH

#include "action.hh"
#include <string>

class ActionOnTake : public Action
{
public:
  ActionOnTake(const char*);
  ~ActionOnTake();
  std::string name();
};

#endif
