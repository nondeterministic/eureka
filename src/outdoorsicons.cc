#include <iostream>
#include <vector>
#include <string>

#include "boost/filesystem.hpp"
#include "boost/filesystem/path.hpp"

#include <SDL.h>
#include <SDL_image.h>

#ifndef EDITOR_COMPILE
#include "simplicissimus.hh"
#else
#include "leibniz.hh"
#endif

#include "outdoorsicons.hh"
#include "world.hh"
#include "config.h"

OutdoorsIcons::OutdoorsIcons()
{
	unsigned icon_size = World::Instance().get_outdoors_tile_size() - 1;

	// Load outdoors icon set
//	std::string icon_path =
//			(std::string)DATADIR + "/simplicissimus/data/" +
//			(std::string)WORLD_NAME + "/images/icons_outdoors.png";
	_ptr_icon_surf = IMG_Load((conf_world_path / "images" / "icons_outdoors.png").c_str());

	if (!_ptr_icon_surf)
		std::cerr << "ERROR: outdoorsicons.cc: Couldn't load outdoors icons: " << IMG_GetError() << std::endl;

	//  std::cout << "pic width: " << _ptr_icon_surf->w
	//	    << " icon size: " << icon_size
	//	    << std::endl;

	SDL_SetAlpha(_ptr_icon_surf,
			!SDL_SRCALPHA
			| SDL_RLEACCEL, 0);

	// Fill icon vector
	for (int y = 0; y < _ptr_icon_surf->h; y += icon_size) {
		for (int x = 0; x < _ptr_icon_surf->w - 1; x += icon_size) {
			SDL_Rect srcRect;
			srcRect.x = x;
			srcRect.y = y;
			srcRect.w = icon_size;
			srcRect.h = icon_size;


			SDL_Surface *surface;
			Uint32 rmask, gmask, bmask, amask;

			/* SDL interprets each pixel as a 32-bit number, so our
	     masks must depend on the endianness (byte order) of the
	     machine */
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

			surface = SDL_CreateRGBSurface(SDL_SWSURFACE,
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

OutdoorsIcons::~OutdoorsIcons()
{
	SDL_FreeSurface(_ptr_icon_surf);

	// Iterate through icon vector and SDL_FreeSurface on all entries!
	for (std::vector<SDL_Surface*>::iterator curr_icon = _vec_sdlicons.begin(); curr_icon != _vec_sdlicons.end(); curr_icon++)
		SDL_FreeSurface(*curr_icon);
	_vec_sdlicons.clear();
}

OutdoorsIcons& OutdoorsIcons::Instance()
{
	static OutdoorsIcons inst;
	return inst;
}

SDL_Surface* OutdoorsIcons::get_sdl_icon(unsigned no)
{
	if (no < _vec_sdlicons.size())
		return _vec_sdlicons[no];
	return NULL;  // or throw?!
}

unsigned OutdoorsIcons::number_of_icons(void)
{
	return _vec_sdlicons.size();
}
