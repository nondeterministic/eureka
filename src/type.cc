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

#include <iostream>
#include <string>
#include <map>
#include <utility>
#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "type.hh"
#include "sdltricks.hh"
#include "sdlwindow.hh"

Type::Type() 
{
	// std::cout << "Type()\n";

	// Is true, when text on grey background is active, i.e., to display a cursor, etc.
	_select = false;

	_w = 8;   // Character width
	_h = 16;  // Character height
}

Type::~Type()
{
	// std::cout << "~Type()\n";

	for (std::map<int, SDL_Texture*>::iterator iter = _map_chars.begin(); iter != _map_chars.end(); iter++)
		SDL_DestroyTexture(iter->second);
	_map_chars.clear();
}

void Type::col_printch(SDL_Texture* surf, int c, int x, int y, SDL_Color bgcol, SDL_Color fgcol)
{
	printch(surf, c, x, y, &bgcol, &fgcol);
}

/// Print ASCIII c onto texture. Texture is normally the entire texture of the console
/// (miniwin, or wherever we want to print onto).

void Type::printch(SDL_Texture* texture, int c, int x, int y, SDL_Color* bgcol, SDL_Color* fgcol)
{
	if (c == 10)
		return; // Ignore newline.

	int w, h;
	if (SDL_QueryTexture(texture, NULL, NULL, &w, &h) < 0) {
		std::cerr << "ERROR: type.cc: printch: texture for ascii " << c << " onto is invalid: " << IMG_GetError() << "." << std::endl;
		return;
	}

	// Define default colours for printing, if none are supplied.
	SDL_Color std_bgcol, std_fgcol;
	SDL_Renderer* renderer = SDLWindow::Instance().getRenderer();

	if (bgcol == NULL) {
		std_bgcol.r = 0; std_bgcol.g = 0; std_bgcol.b = 0;
		bgcol = &std_bgcol;
	}

	if (fgcol == NULL) {
		std_fgcol.r = 253; std_fgcol.g = 253; std_fgcol.b = 253;
		fgcol = &std_fgcol;
	}

	SDL_Texture* charTexture = _map_chars[c];
	if (charTexture == NULL) {
		std::cerr << "ERROR: type.cc: printch: charSurf == NULL for ASCII: " << c << "\n";
		return;
	}

	if (SDL_SetRenderTarget(renderer, texture) < 0) {
		std::cerr << "ERROR: type.cc: printch: Cannot set texture as render target: " << IMG_GetError() << "." << std::endl;
		exit(EXIT_FAILURE);
	}

	// Set background colour of printch.
	SDL_SetRenderDrawColor(renderer, bgcol->r, bgcol->g, bgcol->b, 0);

	if (x == -1 && y == -1) {
		std::cerr << "WARNING: type.cc: printch() got bad coordinates.\n";
		SDL_RenderClear(renderer); // Perhaps, like in the else-case, one should construct sane coords instead of clearing entire texture...
		return;
	}
	else {
		SDL_Rect dstRect;
		dstRect.x = x;
		dstRect.y = y;
		dstRect.w = _w;
		dstRect.h = _h;

		SDL_RenderFillRect(renderer, &dstRect);
		if (SDL_RenderCopy(renderer, charTexture, NULL, &dstRect) < 0) {
			std::cerr << "ERROR: type.cc: printch: RenderCopy failed: " << IMG_GetError() << "." << std::endl;
			return;
		}
	}

//	// Set foreground colour, if non-standard one is required.
//	if (fgcol->r != 253 || fgcol->g != 253 || fgcol->b != 253) {
//		SDL_Color old_fgcol;
//		old_fgcol.r = 252; old_fgcol.g = 252; old_fgcol.b = 252;
//		SDLTricks::Instance().replace_col(texture, old_fgcol, *fgcol, NULL);
//	}
}

int Type::char_width()
{
	return _w;
}

int Type::char_height()
{
	return _h;
}

void Type::set_select(bool new_select)
{
	_select = new_select;
}

void Type::toggle_select()
{
	_select = !_select;
}
