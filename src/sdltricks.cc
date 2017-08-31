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

/// NOW: Replace standard black background color with a new one.
///
/// WAS: Replaces old_col in surf with new_col in the area of rect, or in
/// the entire surface if rect == NULL.  Returns true on success, false
/// in case surf == NULL.
///
/// But, alas! could not get color swapping to work. :-(

bool SDLTricks::replace_bg_color(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Color new_color, SDL_Rect* rect)
{
	SDL_Texture* tmp_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, rect->w, rect->h);

	if (SDL_SetRenderTarget(renderer, tmp_texture) < 0) {
		std::cerr << "ERROR: sdltricks.cc: Set render target failed..\n";
		return false;
	}

	// Fill temporary texture with new color.
	SDL_SetRenderDrawColor(renderer, new_color.r, new_color.g, new_color.b, SDL_ALPHA_OPAQUE);
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
//	SDL_SetRenderDrawColor(renderer, old_color.r, old_color.g, old_color.b, SDL_ALPHA_TRANSPARENT);
//	if (SDL_RenderFillRect(renderer, rect) < 0) {
//		std::cerr << "ERROR: sdltricks.cc: set render draw color failed.\n";
//		return false;
//	}

	// Copy texture onto temp texture, so that the old but now transparent color becomes the new color.
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

//bool SDLTricks::replace_color(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Color old_color, SDL_Color new_color, SDL_Rect* rect)
//{
//	// https://gamedev.stackexchange.com/questions/98641/how-do-i-modify-textures-in-sdl-with-direct-pixel-access
//	// https://discourse.libsdl.org/t/change-texture-access-from-target-to-streaming/21957/8
//
//	int w, h;
//	Uint32 format;
//	SDL_QueryTexture(texture, &format, NULL, &w, &h);
//
//	SDL_Texture* tmp_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, w, h);
//	SDL_SetRenderTarget(renderer, texture);
//
//	// Now let's make our "pixels" pointer point to the texture data.
//	Uint32* pixels = nullptr;
//	int pitch = 0;
//	if (SDL_LockTexture(tmp_texture, nullptr, (void**)&pixels, &pitch) < 0) {
//		std::cout << "MOO\n";
//		return false; // If the locking fails, you might want to handle it somehow. SDL_GetError(); or something here.
//	}
//	std::cout << SDL_QueryTexture(tmp_texture, &format, NULL, &w, &h) << "\n";
//
//	SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_RGBA8888, pixels, pitch);
//
//	SDL_PixelFormat pixelFormat;
//	pixelFormat.format = format;
//	std::cout << "a\n";
//	// Now you want to format the color to a correct format that SDL can use.
//	// Basically we convert our RGB color to a hex-like BGR color.
//	Uint32 old_color_bgr = SDL_MapRGB(&pixelFormat, old_color.r, old_color.g, old_color.b);
//	Uint32 new_color_bgr = SDL_MapRGB(&pixelFormat, new_color.r, new_color.g, new_color.b);
//
//	std::cout << "b\n";
//
//	for(int x = 0; x < w; x++) {
//		for(int y = 0; y < h; y++) {
//			// Before setting the color, we need to know where we have to place it.
//			Uint32 pixelPosition = y * (pitch / sizeof(unsigned int)) + x;
//			if (pixels[pixelPosition] == old_color_bgr)
//				pixels[pixelPosition] = new_color_bgr;
//		}
//	}
//
//	std::cout << "c\n";
//
//	// Also don't forget to unlock your texture once you're done.
//	SDL_UnlockTexture(texture);
//
//	SDL_SetRenderTarget(renderer, texture);
//	SDL_RenderCopy(renderer, tmp_texture, NULL, NULL);
//
//	return true;
//
////	for(int x = 0; x < w; x++) {
////		for(int y = 0; y < h; y++) {
////			if (getpixel(x,y) == coltemp1)
////				putpixel(x,y,coltemp2);
////		}
////	}
//
//}

//bool SDLTricks::replace_color(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Color old_col, SDL_Color new_col, SDL_Rect* rect)
//{
//	Uint32 amask, rmask, bmask, gmask;
//
//#if SDL_BYTEORDER == SDL_BIG_ENDIAN
//    rmask = 0xff000000;
//    gmask = 0x00ff0000;
//    bmask = 0x0000ff00;
//    amask = 0x000000ff;
//#else
//    rmask = 0x000000ff;
//    gmask = 0x0000ff00;
//    bmask = 0x00ff0000;
//    amask = 0xff000000;
//#endif
//
//    int w, h;
//    SDL_QueryTexture(texture, NULL, NULL, &w, &h);
//
//	SDL_Surface* surf = SDL_CreateRGBSurface(0, w, h, 32, rmask, gmask, bmask, amask);
//	SDL_SetRenderTarget(renderer, texture);
//	SDL_RenderReadPixels(renderer, NULL, 0, surf->pixels, surf->pitch);
//	SDL_Surface* surf2 = SDL_ConvertSurface(surf, surf->format, surf->flags);
//
//	SDL_SetColorKey(surf, 1, SDL_MapRGBA(surf->format, old_col.r, old_col.g, old_col.b, amask));
//	SDL_FillRect(surf2, NULL, SDL_MapRGB(surf->format, new_col.r, new_col.g, new_col.b));
//	SDL_BlitSurface(surf, NULL, surf2, NULL);
//	SDL_BlitSurface(surf2, rect, surf, rect);
//	SDL_FreeSurface(surf2);
//
//	// Remove transparency again
//	SDL_SetColorKey(surf, 0, SDL_MapRGBA(surf->format, old_col.r, old_col.g, old_col.b, amask));
//
//	// Now back into textures...
//	SDL_Texture* new_texture = SDL_CreateTextureFromSurface(renderer, surf);
//	SDL_SetRenderTarget(renderer, texture);
//	SDL_RenderCopy(renderer, new_texture, NULL, NULL);
//
//	SDL_FreeSurface(surf);
//	SDL_DestroyTexture(new_texture);
//
//	return true;
//}

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
