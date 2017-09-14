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
