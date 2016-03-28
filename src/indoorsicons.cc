#include <iostream>
#include <string>
#include <vector>

#include <SDL.h>
#include <SDL_image.h>

#ifndef EDITOR_COMPILE
#include "simplicissimus.hh"
#else
#include "leibniz.hh"
#endif

#include "indoorsicons.hh"
#include "world.hh"
#include "config.h"

IndoorsIcons::IndoorsIcons()
{
  unsigned icon_size = World::Instance().get_indoors_tile_size();

  // Load indoors icon set
  _ptr_icon_surf = IMG_Load((conf_world_path / "images" / "icons_indoors.png").c_str());
  if (!_ptr_icon_surf)
    std::cerr << "ERROR: indooricons.cc: Couldn't load indoors icons: " << IMG_GetError() << ": " << (conf_world_path / "images" / "icons_indoors.png").c_str() << std::endl;
  
  // TODO: Why oh why do I have to disable alpha first?
  SDL_SetAlpha(_ptr_icon_surf, 
	       !SDL_SRCALPHA
	       | SDL_RLEACCEL, 255);

  // Fill icon vector
  for (int y = 0; y < _ptr_icon_surf->h; y += icon_size)
    {
      for (int x = 0; x < _ptr_icon_surf->w; x += icon_size)
	{
	  SDL_Rect srcRect;
	  srcRect.x = x;
	  srcRect.y = y;
	  srcRect.w = icon_size;
	  srcRect.h = icon_size;
	  	  
	  SDL_Surface *surface;
	  Uint32 rmask, gmask, bmask, amask;
	  
	  /* SDL interprets each pixel as a 32-bit number, so our masks must depend
	     on the endianness (byte order) of the machine */
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	  rmask = 0xff000000;
	  gmask = 0x00ff0000;
	  bmask = 0x0000ff00;
	  amask = 0x000000ff;
#else
	  rmask = 0x000000ff;
	  gmask = 0x0000ff00;
	  bmask = 0x00ff0000;
	  amask = 0xff000000;
#endif
	  
	  surface = SDL_CreateRGBSurface(SDL_SWSURFACE
					 // | SDL_HWSURFACE
					 | SDL_SRCALPHA,
					 icon_size, 
					 icon_size, 
					 32,  // What is this 32??
					 rmask, 
					 gmask, 
					 bmask, 
					 amask);
	  // Alpha, alternatively:
	  // http://www.sdltutorials.com/sdl-image/
	  // SDL_SetAlpha(_ptr_icon_surf, !SDL_SRCALPHA, 255);
	  SDL_BlitSurface(_ptr_icon_surf,
			  &srcRect, 
			  surface,
			  NULL);
	  _vec_sdlicons.push_back(surface);
	}
    }

  // Tell super class how many icons there are
  reserve(number_of_icons());
}

IndoorsIcons::~IndoorsIcons()
{
  SDL_FreeSurface(_ptr_icon_surf);

  // Iterate through icon vector and SDL_FreeSurface on all entries!
  for (std::vector<SDL_Surface*>::iterator curr_icon = _vec_sdlicons.begin();
       curr_icon != _vec_sdlicons.end();
       curr_icon++)
    SDL_FreeSurface(*curr_icon);
  _vec_sdlicons.clear();
}

IndoorsIcons& IndoorsIcons::Instance()
{
  static IndoorsIcons inst;
  return inst;
}

SDL_Surface* IndoorsIcons::get_sdl_icon(unsigned no)
{
  if (no < _vec_sdlicons.size())
    return _vec_sdlicons[no];
  return NULL;  // TODO: or throw?!
}

unsigned IndoorsIcons::number_of_icons(void)
{
  return _vec_sdlicons.size();
}
