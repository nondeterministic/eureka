#ifndef __ACTIONOPENED_HH
#define __ACTIONOPENED_HH

#include "action.hh"
#include <string>

class ActionOpened : public Action
{
public:
  ActionOpened(const char*);
  ~ActionOpened();
  std::string name();
};

#endif
