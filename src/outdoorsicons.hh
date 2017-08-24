#ifndef __OUTDOORSICONS_HH
#define __OUTDOORSICONS_HH

#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "icons.hh"

class OutdoorsIcons : public Icons
{
public:
  static OutdoorsIcons& Instance();
  int                   convert_icons_to_textures(SDL_Renderer*);

  OutdoorsIcons(OutdoorsIcons const&) = delete;
  void operator=(OutdoorsIcons const&) = delete;

protected:
  OutdoorsIcons();
};

#endif
