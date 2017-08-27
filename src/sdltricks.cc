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

// Replaces old_col in surf with new_col in the area of rect, or in
// the entire surface if rect == NULL.  Returns true on success, false
// in case surf == NULL.

bool SDLTricks::replace_color(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Color old_color, SDL_Color new_color, SDL_Rect* rect)
{
	if (old_color.r == new_color.r && old_color.g == new_color.g && old_color.b == new_color.b) {
		std::cout << "INFO: sdltricks: swapcolors not invoked as new color == old color.\n";
		return true;
	}

	SDL_Texture* tmp_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, rect->w, rect->h);

	if (SDL_SetRenderTarget(renderer, tmp_texture) < 0) {
		std::cerr << "ERROR: sdltricks.cc: Set render target failed..\n";
		return false;
	}

	// Fill temporary texture with new color.
	SDL_SetRenderDrawColor(renderer, new_color.r, new_color.g, new_color.b, 255); // Opaque
	if (SDL_RenderFillRect(renderer, NULL) < 0) {
		std::cerr << "ERROR: sdltricks.cc: set render draw color failed.\n";
		return false;
	}

	// Make old texture's old color transparent.
	SDL_SetRenderTarget(renderer, texture);
	if (SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND) < 0) {
		std::cerr << "ERROR: sdltricks.cc: set blend mode failed.\n";
		return false;
	}
	SDL_SetRenderDrawColor(renderer, old_color.r, old_color.g, old_color.b, 0); // Transparent
	if (SDL_RenderFillRect(renderer, rect) < 0) {
		std::cerr << "ERROR: sdltricks.cc: set render draw color failed.\n";
		return false;
	}

	// Copy texture onto temp texture, so that the old transparent color becomes the new color.
	SDL_SetRenderTarget(renderer, tmp_texture);
	SDL_RenderCopy(renderer, texture, rect, NULL);

	// Now remove alpha again and copy back.
	if (SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_NONE) < 0) {
		std::cerr << "ERROR: sdltricks.cc: reset blend mode failed.\n";
		return false;
	}
	SDL_SetRenderTarget(renderer, texture);
	SDL_RenderCopy(renderer, tmp_texture, NULL, rect);

	// Destroy temp texture and good bye.
	SDL_DestroyTexture(tmp_texture);

	return true;
}

unsigned int SDLTricks::getpixel(SDL_Texture* s, int x, int y)
{
	return 0; // TODO SDL
}

//unsigned int SDLTricks::getpixel(SDL_Texture* s, int x, int y)
//{
//	return ((unsigned int*)s->pixels)[y*(s->pitch/sizeof(unsigned int)) + x];
//}

void SDLTricks::putpixel(SDL_Texture* surface, int x, int y, Uint32 pixel)
{
	return; // TODO SDL

//    int bpp = surface->format->BytesPerPixel;
//    /* Here p is the address to the pixel we want to set */
//    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
//
//    switch(bpp) {
//    case 1:
//        *p = pixel;
//        break;
//
//    case 2:
//        *(Uint16 *)p = pixel;
//        break;
//
//    case 3:
//        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
//            p[0] = (pixel >> 16) & 0xff;
//            p[1] = (pixel >> 8) & 0xff;
//            p[2] = pixel & 0xff;
//        } else {
//            p[0] = pixel & 0xff;
//            p[1] = (pixel >> 8) & 0xff;
//            p[2] = (pixel >> 16) & 0xff;
//        }
//        break;
//
//    case 4:
//        *(Uint32 *)p = pixel;
//        break;
//    }
}
