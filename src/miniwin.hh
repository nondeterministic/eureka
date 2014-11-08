#ifndef MINIWIN_HH
#define MINIWIN_HH

#include <SDL.h>
#include <string>

#include "type.hh"
#include "charset.hh"
#include "sdlwindowregion.hh"

class MiniWin : public SDLWindowRegion
{
public:
  static MiniWin& Instance();
  void alarm();
  void surface_from_file(std::string);

protected:
  SDL_Surface* _tmp_surf;

  MiniWin();
};

#endif
