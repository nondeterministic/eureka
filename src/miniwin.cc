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
	const std::pair<int,int> size = std::make_pair(get_dimensions().w, get_dimensions().h);
	SDL_Renderer*  r = get_renderer();
	SDL_Texture*   t = get_texture();
	SDL_Texture* tmp = SDL_CreateTexture(r, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, size.first, size.second);

	// Back texture up.
	SDL_SetRenderTarget(r, tmp);
	SDL_RenderCopy(r, t, NULL, NULL);

	// Whiten texture.
	SDL_SetRenderTarget(r, t);
	SDL_SetRenderDrawColor(r, 255, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(r);

	// Show for some time.
	blit();
	SDL_Delay(100);

	// Show old texture again.
	SDL_SetRenderTarget(r, t);
	SDL_RenderCopy(r, tmp, NULL, NULL);
	blit();
	SDL_DestroyTexture(tmp);
}

void MiniWin::surface_from_file(std::string filename)
{
	SDL_Surface* _tmp_surf = NULL;

	if ((_tmp_surf = IMG_Load(filename.c_str())) == NULL)
		std::cerr << "ERROR: miniwin.cc: miniwin could not load surface: '" << filename << "'.\n";

	SDL_Renderer* renderer = SDLWindow::Instance().get_renderer();
	SDL_Texture* tmp_txt = SDL_CreateTextureFromSurface(renderer, _tmp_surf);
	SDL_SetRenderTarget(renderer, _texture);
	SDL_RenderCopy(renderer, tmp_txt, NULL, NULL);
	SDLWindow::Instance().resetRenderer();

	SDL_FreeSurface(_tmp_surf);
	SDL_DestroyTexture(tmp_txt);

	// SDL_BlitSurface(_tmp_surf, NULL, _surf, NULL);
}
