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
// TODO SDL

//	SDL_Texture* s = SDLWindow::Instance().get_mini_win_SDL_surface();
//	// SDL_Surface *s = _surf; // SDLWindow::Instance().get_mini_win_SDL_surface();
//	SDL_Surface *copy = SDL_ConvertSurface(s, s->format, s->flags);
//
//	// cf. http://cboard.cprogramming.com/game-programming/99430-how-make-grayscale-filter.html
//	SDL_LockSurface(s);
//	for ( int x = 0; x != s->w; ++x ) {
//		for ( int y = 0; y != s->h; ++y ) {
//			Uint32 pixel = SDLTricks::Instance().getpixel(s,x,y);
//			Uint8 r = 0;
//			Uint8 g = 0;
//			Uint8 b = 0;
//			SDL_GetRGB(pixel, s->format, &r, &g, &b);
//			r = g = b = (( r+g+b )/3);
//			r = 0xFF;
//			SDLTricks::Instance().putpixel(s,x,y, SDL_MapRGB(s->format, r,g,b));
//		}
//	}
//	SDL_UnlockSurface(s);
//
//	SDLWindow::Instance().blit_mini_win();
//	SDL_Delay(300);
//	SDL_BlitSurface(copy, NULL, s, NULL);
//	SDL_FreeSurface(copy);
}

void MiniWin::surface_from_file(std::string filename)
{
	SDL_Surface* _tmp_surf = NULL;

	if ((_tmp_surf = IMG_Load(filename.c_str())) == NULL)
		std::cerr << "ERROR: miniwin.cc: miniwin could not load surface: '" << filename << "'.\n";

	SDL_Renderer* renderer = SDLWindow::Instance().getRenderer();
	SDL_Texture* tmp_txt = SDL_CreateTextureFromSurface(renderer, _tmp_surf);
	SDL_SetRenderTarget(renderer, _texture);
	SDL_RenderCopy(renderer, tmp_txt, NULL, NULL);
	SDLWindow::Instance().resetRenderer();

	SDL_FreeSurface(_tmp_surf);
	SDL_DestroyTexture(tmp_txt);

	// SDL_BlitSurface(_tmp_surf, NULL, _surf, NULL);
}
