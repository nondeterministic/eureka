// This source file is part of eureka
//
// Copyright (c) 2007-2016  Andreas Bauer <baueran@gmail.com>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
// USA.

#include "charset.hh"
#include "eureka.hh"

#include <SDL.h>
#include <SDL_image.h>

#include <string>
#include <iostream>
#include <map>
#include <utility>

#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>

using namespace std;

Charset::Charset() {
	load_charset();
}

Charset::~Charset() {
	// std::cout << "~Charset()\n";
}

int Charset::load_charset() {
	boost::filesystem::path icon_path(conf_data_path);
	icon_path /= "charset.png";

	Uint32 rmask, gmask, bmask, amask;
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

	// SDL_Surface* temp = IMG_Load(icon_path.c_str());
	// SDL_SetAlpha(temp, SDL_SRCALPHA | SDL_RLEACCEL, SDL_ALPHA_OPAQUE);
	// _ptr_charset_surf = SDL_DisplayFormatAlpha(temp);

	if (!(_ptr_charset_surf = IMG_Load(icon_path.string().c_str()))) {
		std::cerr << "ERROR: Couldn't load charset: " << IMG_GetError() << std::endl;
		return -1;
	}

	// Fill character std::map.
	int last_ascii = 0;
	for (int y = 0; y < _ptr_charset_surf->h; y += _h) {
		for (int x = 0; x < _ptr_charset_surf->w - 1; x += _w) {
			SDL_Rect srcRect;
			srcRect.x = x;
			srcRect.y = y;
			srcRect.w = _w;
			srcRect.h = _h;

			SDL_Surface *surface = NULL;

			if (!(surface = SDL_CreateRGBSurface(SDL_HWSURFACE, _w, _h, 32, rmask, gmask, bmask, amask))) {
				std::cerr << "ERROR: charset.cc surface == NULL\n";
				return -2;
			}

			// It is not f*cking obvious from the docs that you have to
			// clear the SDL_SRCALPHA flag on the source surface in order to
			// preserve its alpha mask in a subsequent blit.
			//
			// http://forums.libsdl.org/viewtopic.php?t=850&sid=50447b2dc7f77f03c86bed666ce568c2
			SDL_SetAlpha(_ptr_charset_surf, 0, SDL_ALPHA_OPAQUE);
			SDL_BlitSurface(_ptr_charset_surf, &srcRect, surface, NULL);

			// Now enable transparency again to preserve for later blits.
			SDL_SetAlpha(surface, SDL_SRCALPHA, SDL_ALPHA_TRANSPARENT);

			int x2 = x / _w;
			int y2 = y / _h;

			int ascii = 0;
			// Ankh
			if (y2 == 0 && x2 == 0)
				ascii = 0;
			// Shield
			else if (y2 == 0 && x2 == 1)
				ascii = 1;
			// Wall
			else if (y2 == 0 && x2 == 2)
				ascii = 2;
			// Wall 2
			else if (y2 == 0 && x2 == 3)
				ascii = 3;
			// Arrow Up-Down
			else if (y2 == 0 && x2 == 4)
				ascii = 4;
			// Arrow Down
			else if (y2 == 0 && x2 == 5)
				ascii = 5;
			// Arrow Up
			else if (y2 == 0 && x2 == 6)
				ascii = 6;
			// Weird Arrow Up
			else if (y2 == 0 && x2 == 7)
				ascii = 7;
			// Stone
			else if (y2 == 0 && x2 == 8)
				ascii = 8;
			// (C)
			else if (y2 == 0 && x2 == 9)
				ascii = 9;
			// (R)
			else if (y2 == 0 && x2 == 10)
				ascii = 15;

			// Reserve 10 - 15 for new line/feed/etc.

			// Male
			else if (y2 == 0 && x2 == 11)
				ascii = 16;
			// Female
			else if (y2 == 0 && x2 == 12)
				ascii = 17;
			// Blue Bar
			else if (y2 == 0 && x2 == 13)
				ascii = 18;
			// Frame
			else if (y2 == 0 && x2 == 14)
				ascii = 19;

			// Jump 400 up

			// Blue Stone
			else if (y2 == 0 && x2 == 15)
				ascii = 420;
			// Right Blue Bar End
			else if (y2 == 1 && x2 == 0)
				ascii = 421;
			// Right Blue Bar End
			else if (y2 == 1 && x2 == 1)
				ascii = 422;
			// Blank
			else if (y2 == 1 && x2 == 2)
				ascii = 423;
			// ...
			else if (y2 == 1 && x2 == 3)
				ascii = 424;
			// Moon 1
			else if (y2 == 1 && x2 == 4)
				ascii = 425;
			// Moon 2
			else if (y2 == 1 && x2 == 5)
				ascii = 426;
			// Moon 3
			else if (y2 == 1 && x2 == 6)
				ascii = 427;
			// Moon 4
			else if (y2 == 1 && x2 == 7)
				ascii = 428;
			// Moon 5
			else if (y2 == 1 && x2 == 8)
				ascii = 429;
			// Moon 6
			else if (y2 == 1 && x2 == 9)
				ascii = 430;
			// Moon 7
			else if (y2 == 1 && x2 == 10)
				ascii = 431;
			// Moon 8
			else if (y2 == 1 && x2 == 11)
				ascii = 432;
			// Whirl 1
			else if (y2 == 1 && x2 == 12)
				ascii = 433;
			// Whirl 2
			else if (y2 == 1 && x2 == 13)
				ascii = 434;
			// Whirl 3
			else if (y2 == 1 && x2 == 14)
				ascii = 435;
			// Whirl 4
			else if (y2 == 1 && x2 == 15)
				ascii = 436;
			// Delete
			else if (y2 == 7 && x2 == 15)
				ascii = 437;

			// Calculate the rest
			else {
				if (last_ascii > 0)
					ascii = last_ascii + 1;
				else
					ascii = 32;
				last_ascii = ascii;
			}

			// Insert in maps.
			_map_chars.insert(make_pair(ascii, surface));
		}
	}

	return 0;
};
