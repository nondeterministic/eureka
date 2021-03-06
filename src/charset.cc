// This source file is part of eureka
//
// Copyright (c) 2007-2018 Andreas Bauer <a@pspace.org>
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

#include <string>
#include <iostream>
#include <map>
#include <utility>

#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "charset.hh"
#include "eureka.hh"
#include "sdlwindow.hh"

Charset::Charset()
{
	load_charset();
}

Charset& Charset::Instance()
{
	static Charset _inst;
	return _inst;
}

Charset::~Charset()
{
	// std::cout << "~Charset()\n";
}

int Charset::load_charset()
{
	boost::filesystem::path icon_path(conf_data_path);
	icon_path /= "charset.png";

	SDL_Texture* all_chars_texture = NULL;
	SDL_Surface* all_chars_surface = NULL;

	if (!(all_chars_surface = IMG_Load(icon_path.string().c_str()))) {
		std::cerr << "ERROR: charset.cc: Couldn't load charset: " << IMG_GetError() << std::endl;
		return -1;
	}

	SDL_Renderer* renderer = SDLWindow::Instance().get_renderer();
	SDLWindow::Instance().resetRenderer();

	if ((all_chars_texture = SDL_CreateTextureFromSurface(renderer, all_chars_surface)) == NULL) {
		std::cerr << "ERROR: charset.cc: create texture from surface not working: " << IMG_GetError() << "\n";
		return -1;
	}

	if (SDL_SetTextureBlendMode(all_chars_texture, SDL_BLENDMODE_BLEND) < 0) {
		std::cerr << "ERROR: charset.cc: setting blend mode (1) not working: " << IMG_GetError() << "\n";
		return -1;
	}

// see also https://forums.libsdl.org/viewtopic.php?p=40949
// SDL_SetRenderTarget(renderer, all_chars_texture);
// SDL_SetRenderDrawColor(renderer, 0,0,0,0);
// SDL_RenderClear(renderer);

	// Fill character std::map.
	int last_ascii = 0;
	for (int y = 0; y < all_chars_surface->h; y += _h) {
		for (int x = 0; x < all_chars_surface->w - 1; x += _w) {
			SDL_Rect srcRect;
			srcRect.x = x;
			srcRect.y = y;
			srcRect.w = _w;
			srcRect.h = _h;

			SDL_Texture* single_char_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, _w, _h);

			if (single_char_texture == NULL) {
				std::cerr << "ERROR: charset.cc: create texture not working: " << IMG_GetError() << "\n";
				exit(EXIT_FAILURE);
			}

			if (SDL_SetTextureBlendMode(single_char_texture, SDL_BLENDMODE_BLEND) < 0) {
				std::cerr << "ERROR: charset.cc: setting blend mode (2) not working: " << IMG_GetError() << "\n";
				continue;
			}

			if (SDL_SetRenderTarget(renderer, single_char_texture) < 0) {
				std::cerr << "ERROR: charset.cc: setting rendering target not working: " << IMG_GetError() << "\n";
				exit(EXIT_FAILURE);
			}

			// see above, and also link!
			SDL_SetRenderDrawColor(renderer, 0,0,0,0);
			SDL_RenderClear(renderer);

			if (SDL_RenderCopy(renderer, all_chars_texture, &srcRect, NULL) < 0) {
				std::cerr << "WARNING: charset.cc: render copy failed: " << IMG_GetError() << "\n";
				continue;
			}

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
			if (single_char_texture != NULL) {
				std::pair<int, SDL_Texture*> pair = std::make_pair(ascii, single_char_texture);
				_map_chars.insert(pair);
			}
			else
				std::cerr << "WARNING: charset.cc: single_char_texture == NULL.\n";
		}
	}

	SDL_DestroyTexture(all_chars_texture);
	SDL_FreeSurface(all_chars_surface);

	return 0;
};
