#ifndef __INDOORSICONS_HH
#define __INDOORSICONS_HH

#include <iostream>
#include <vector>
#include <SDL.h>
#include <SDL_image.h>
#include "icons.hh"

class IndoorsIcons : public Icons
{
public:
  static IndoorsIcons& Instance();
  SDL_Surface* get_sdl_icon(unsigned);
  unsigned number_of_icons();

protected:
  IndoorsIcons();
  ~IndoorsIcons();

  // Override copy constructor
  IndoorsIcons(const IndoorsIcons&);

  SDL_Surface* _ptr_icon_surf;
  std::vector<SDL_Surface*> _vec_sdlicons;
};

#endif
