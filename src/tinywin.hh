#ifndef TINYWIN_HH
#define TINYWIN_HH

#include "sdlwindowregion.hh"

class TinyWin : public SDLWindowRegion
{
public:
  static TinyWin& Instance();

protected:
  TinyWin();
};

#endif
