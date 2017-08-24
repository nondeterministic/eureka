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

bool SDLTricks::replace_col(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Color old_col, SDL_Color new_col, SDL_Rect* rect)
{
	int w, h;
	Uint32 format;
	if (SDL_QueryTexture(texture, &format, NULL, &w, &h) < 0) {
		std::cerr << "ERROR: sdltricks.cc texture is NULL.\n";
		return false;
	}

	SDL_SetRenderTarget(renderer, texture);
	SDL_Surface* surf = SDL_CreateRGBSurface(0, w, h, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
	SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ABGR8888, surf->pixels, surf->pitch); // w * 4 /*SDL_PIXELFORMAT_ABGR8888*/ );

 	if (surf == NULL) {
 	    std::cerr << "ERROR: surf == NULL in SDLTricks::replace_col.\n";
 	    return false;
 	}

 	SDL_Surface *surf2 = SDL_ConvertSurface(surf, surf->format, surf->flags);
 	Uint32 amask;
 	#if SDL_BYTEORDER == SDL_BIG_ENDIAN
 	  amask = 0x000000ff;
 	#else
 	  amask = 0xff000000;
 	#endif

 	SDL_SetColorKey(surf, SDL_TRUE, SDL_MapRGBA(surf->format, old_col.r, old_col.g, old_col.b, amask));
 	SDL_FillRect(surf2, NULL, SDL_MapRGB(surf->format, new_col.r, new_col.g, new_col.b));
 	SDL_BlitSurface(surf, NULL, surf2, NULL);
 	SDL_BlitSurface(surf2, rect, surf, rect);
 	SDL_FreeSurface(surf2);

 	// Remove transparency again
 	SDL_SetColorKey(surf, 0, SDL_MapRGBA(surf->format, old_col.r, old_col.g, old_col.b, amask));

 	SDL_Texture* target = SDL_CreateTextureFromSurface(renderer, surf);
 	SDL_SetRenderTarget(renderer, texture);
 	SDL_RenderClear(renderer);
 	SDL_RenderCopy(renderer, target, NULL, NULL);

 	SDL_FreeSurface(surf);
 	SDL_DestroyTexture(target);

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
