// This source file is part of eureka
//
// Copyright (c) 2007-2018 Andreas Bauer <baueran@gmail.com>
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

#include <SDL2/SDL.h>

#include "miniwin.hh"
#include "sdlwindow.hh"
#include "sdltricks.hh"
#include "ztatswin.hh"

MiniWin::MiniWin()
{
	SDLWindow& win = SDLWindow::Instance();

	set_texture(SDLWindow::Instance().get_texture_mini_win());

	int miniwin_w, miniwin_h;
	if (SDL_QueryTexture(win.get_texture_mini_win(), NULL, NULL, &miniwin_w, &miniwin_h) < 0) {
		std::cerr << "WARNING: miniwin.cc: cannot set texture: " << IMG_GetError() << "\n";
		exit(EXIT_FAILURE);
	}

	SDL_Rect rect;
	rect.x = win.get_size().first - win.frame_icon_size() - miniwin_w + 2;
	rect.y = ZtatsWin::Instance().get_dimensions().h + 2 * win.frame_icon_size() - 6;
	rect.w = miniwin_w;
	rect.h = miniwin_h;
	set_dimensions(rect);

	// Alter position of text relative to SDL surface
	_y_frame_offset = 1;
}

MiniWin& MiniWin::Instance()
{
	static MiniWin inst;
	return inst;
}

// Briefly highlights the window in red as a kind of alarm.
// Used for noting the fact that the enemy has been hit.

void MiniWin::alarm()
{
	save_texture();

	SDL_SetRenderTarget(_renderer, _texture);
	SDL_SetRenderDrawColor(_renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderFillRect(_renderer, NULL);

	// Show for some time.
	blit();
	SDLWindow::Instance().blit_entire_window_texture();
	SDL_Delay(100);

	display_last();
	SDLWindow::Instance().blit_entire_window_texture();
}
