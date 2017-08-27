// This source file is part of eureka
//
// Copyright (c) 2007-2017  Andreas Bauer <baueran@gmail.com>
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

#ifndef __SDLTRICKS_HH
#define __SDLTRICKS_HH

#include <SDL2/SDL.h>

using namespace std;

class SDLTricks
{
public:
	static SDLTricks& Instance();
	bool replace_color(SDL_Renderer*, SDL_Texture*, SDL_Color, SDL_Color, SDL_Rect* = NULL);
	void putpixel(SDL_Texture* surface, int x, int y, Uint32 pixel);
	unsigned int getpixel(SDL_Texture* s, int x, int y);

private:
	SDLTricks();
	~SDLTricks();
};

#endif

