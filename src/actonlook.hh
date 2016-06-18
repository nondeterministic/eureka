#ifndef __ACTIONOLOOK_HH
#define __ACTIONOLOOK_HH

#include "action.hh"
#include <string>

class ActionOnLook : public Action
{
public:
  ActionOnLook(int, int, const char*);
  ActionOnLook(const char*);
  ~ActionOnLook();
  std::string name();
};

#endif
