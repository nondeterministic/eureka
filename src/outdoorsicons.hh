#ifndef __OUTDOORSICONS_HH
#define __OUTDOORSICONS_HH

#include <vector>
#include <SDL.h>
#include <SDL_image.h>
#include "icons.hh"

class OutdoorsIcons : public Icons
{
public:
  static OutdoorsIcons& Instance();
  SDL_Surface*          get_sdl_icon(unsigned);
  unsigned              number_of_icons();

protected:
  OutdoorsIcons();
  ~OutdoorsIcons();

  SDL_Surface*                                 _ptr_icon_surf;
  std::vector<SDL_Surface*>                    _vec_sdlicons;
};

#endif
