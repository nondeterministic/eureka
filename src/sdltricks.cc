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

#include "sdltricks.hh"
#include <iostream>

SDLTricks::SDLTricks() {}
SDLTricks::~SDLTricks() {}

SDLTricks& SDLTricks::Instance()
{
	static SDLTricks inst;
	return inst;
}

/// Replace standard black background color with a new one.

bool SDLTricks::replace_color(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Color new_colour, SDL_Color old_colour, SDL_Rect* rect)
{
	// Copy rect-area of texture into surface
	SDL_Surface* surface_of_rect_texture = SDL_CreateRGBSurface(0, rect->w, rect->h, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
	SDL_SetRenderTarget(renderer, texture);
	SDL_RenderReadPixels(renderer, rect, SDL_PIXELFORMAT_RGBA8888, surface_of_rect_texture->pixels, surface_of_rect_texture->pitch);

	// Convert new surface to color-modifiable surface
	SDL_Surface* tmp_surface = SDL_ConvertSurface(surface_of_rect_texture, surface_of_rect_texture->format, surface_of_rect_texture->flags);

	Uint32 amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	amask = 0x000000ff;
#else
	amask = 0xff000000;
#endif

	SDL_SetColorKey(tmp_surface, 1, SDL_MapRGBA(tmp_surface->format, old_colour.r, old_colour.g, old_colour.b, amask));
	SDL_FillRect(tmp_surface, NULL, SDL_MapRGB(tmp_surface->format, new_colour.r, new_colour.g, new_colour.b));
	SDL_BlitSurface(surface_of_rect_texture, NULL, tmp_surface, NULL);
	SDL_BlitSurface(tmp_surface, NULL, surface_of_rect_texture, NULL);

	// Remove transparency again
	SDL_SetColorKey(surface_of_rect_texture, 0, SDL_MapRGBA(surface_of_rect_texture->format, old_colour.r, old_colour.g, old_colour.b, amask));

	// Copy and quit
	SDL_Texture* tmp_texture = SDL_CreateTextureFromSurface(renderer, surface_of_rect_texture);
	SDL_SetRenderTarget(renderer, texture);
	SDL_RenderCopy(renderer, tmp_texture, NULL, rect);

	// Free memory
	SDL_FreeSurface(tmp_surface);
	SDL_FreeSurface(surface_of_rect_texture);
	SDL_DestroyTexture(tmp_texture);

	return true;
}
