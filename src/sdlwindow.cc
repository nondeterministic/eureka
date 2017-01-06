/* *********************************************************************
 * This file is part of eureka/Leibniz.
 *
 * Copyright (c) Andreas Bauer <baueran@gmail.com>
 *
 * eureka/Leibniz is free software: you can redistribute it
 * and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * eureka/Leibniz is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with eureka/Leibniz.  If not, see 
 * <http://www.gnu.org/licenses/>. 
 * ********************************************************************* */

#include "sdlwindow.hh"
#include "util.hh"
#include "config.h"

// Testing only
#include "ztatswin.hh"
#include "miniwin.hh"
#include "tinywin.hh"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <string>
#include <iostream>

#include "boost/filesystem.hpp"
#include "boost/filesystem/path.hpp"

SDLWindow::SDLWindow()
{
	_interior = NULL;
	_win      = NULL;
	_console  = NULL;
	_ztats    = NULL;
	_mini_win = NULL;

	/* SDL interprets each pixel as a 32-bit number, so our
     masks must depend on the endianness (byte order) of the
     machine */
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif
}

SDLWindow::~SDLWindow()
{
	// Iterate through frame vector and SDL_FreeSurface on all entries!
	for (std::vector<SDL_Surface*>::iterator curr_icon = _vec_frameicons.begin();
			curr_icon != _vec_frameicons.end();
			curr_icon++)
	{
		SDL_FreeSurface(*curr_icon);
	}
	_vec_frameicons.clear();

	if (_interior)
		SDL_FreeSurface(_interior);
	if (_console)
		SDL_FreeSurface(_console);
	if (_ztats)
		SDL_FreeSurface(_ztats);
	if (_mini_win)
		SDL_FreeSurface(_mini_win);
	if (_tiny_win)
		SDL_FreeSurface(_tiny_win);
}

SDLWindow& SDLWindow::Instance()
{
	static SDLWindow _inst;
	return _inst;
}

SDL_Surface* SDLWindow::get_SDL_surface()
{
	if (!_win)
		throw SurfaceNULL("SDLWindow::get_SDL_surface: _win is NULL");
	return _win;
}

int SDLWindow::init(int width, int height, int bpp, Uint32 flags)
{
	_w = width; _h = height;

	// _win must be freed via SDL_Quit(), not by caller; see man page.
	if (SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) != 0
			|| ((_win = SDL_SetVideoMode(_w, _h, bpp, flags)) == NULL))
		return -1;

	if (Mix_OpenAudio(22050,AUDIO_S16SYS,2,640) != 0)
		std::cerr << "ERROR: sdlwindow.cc: Could not initialize audio.\n";

	if (!SDL_JoystickOpen(0))
		std::cout << "INFO: sdlwindow.cc: No joystick found!\n";
	else
		std::cout << "INFO: sdlwindow.cc: Joystick detected.\n";

	return 0;
}

void SDLWindow::close()
{
	SDL_Quit();
}

int SDLWindow::create_console_surface()
{
	// We need to determine the size of the console relative to the
	// interior, hence when it is not initialised, we return with an
	// error.
	if (!_interior)
		return -2;

	// TODO: the width and height is probably garbage!
	if (! (_console = SDL_CreateRGBSurface(SDL_HWSURFACE,
			_w-_interior->w-2.5*(float)_frame_icon_size,
			((float)_interior->h)/2.0-_frame_icon_size+2,
			32, rmask, gmask, bmask,
			0))) // 0 = do not preserve/support transparency
		return -1;

	// Fill surface only temporarily to see where to move it to...
	// SDL_FillRect(_console, NULL, SDL_MapRGBA(_console->format, 50, 50, 50, amask));
	return 0;
}

int SDLWindow::create_ztats_surface()
{
	// We need to determine the size of the area relative to the
	// interior, hence when it is not initialised, we return with an
	// error.
	if (!_interior)
		return -2;

	if (! (_ztats = SDL_CreateRGBSurface(SDL_HWSURFACE,
			_w-_interior->w-2.5*(float)_frame_icon_size,
			((float)_interior->h)/2.0-(5*_frame_icon_size)+2,
			32, rmask, gmask, bmask,
			0)))
		return -1;

	// Fill surface only temporarily to see where to move it to...
	// SDL_FillRect(_ztats, NULL, SDL_MapRGBA(_ztats->format, 90, 90, 50, amask));

	return 0;
}

int SDLWindow::create_mini_win_surface()
{
	// We need to determine the size of the area relative to the
	// interior, hence when it is not initialised, we return with an
	// error.
	if (!_interior)
		return -2;

	if (! (_mini_win = SDL_CreateRGBSurface(SDL_HWSURFACE,
			_w-_interior->w-2.5*(float)_frame_icon_size,
			_frame_icon_size * 2 + 4,
			32, rmask, gmask, bmask,
			0)))
		// amask)))
		return -1;

	// SDL_FillRect(_mini_win, NULL, SDL_MapRGB(_mini_win->format, 50, 50, 50));

	return 0;
}

int SDLWindow::create_tiny_win_surface()
{
	// We need to determine the size of the area relative to the
	// interior, hence when it is not initialised, we return with an
	// error.
	if (!_interior)
		return -2;

	if (! (_tiny_win = SDL_CreateRGBSurface(SDL_HWSURFACE,
			_w-_interior->w-2.5*(float)_frame_icon_size,
			_frame_icon_size,
			32, rmask, gmask, bmask,
			0)))
	{
		return -1;
	}

	return 0;
}

SDL_Surface* SDLWindow::get_drawing_area_SDL_surface()
{
	if (!_interior)
		throw SurfaceNULL("SDLWindow::get_drawing_area_SDL_surface: _interior is NULL");
	return _interior;
}

SDL_Surface* SDLWindow::get_console_SDL_surface()
{
	if (!_console)
		throw SurfaceNULL("SDLWindow::get_console_SDL_surface: _console is NULL");
	return _console;
}

SDL_Surface* SDLWindow::get_ztats_SDL_surface()
{
	if (!_ztats)
		throw SurfaceNULL("SDLWindow::get_ztats_SDL_surface: _ztats is NULL");
	return _ztats;
}

SDL_Surface* SDLWindow::get_mini_win_SDL_surface()
{
	if (!_mini_win)
		throw SurfaceNULL("SDLWindow::get_mini_win_SDL_surface: _mini_win is NULL");
	return _mini_win;
}

SDL_Surface* SDLWindow::get_tiny_win_SDL_surface()
{
	if (!_tiny_win)
		throw SurfaceNULL("SDLWindow::get_tiny_win_SDL_surface: _tiny_win is NULL");
	return _tiny_win;
}

// If icon_size != 0, then the value is used as an offset to make a
// hex arena fill the screen properly.

int SDLWindow::blit_interior()
{
	SDL_Rect dstRect;
	dstRect.x = _frame_icon_size - 2;
	dstRect.y = _frame_icon_size - 2;
	dstRect.w = _interior->w + 1;
	dstRect.h = _interior->h + 1;

	if (SDL_BlitSurface(_interior, NULL, _win, &dstRect) == 0) {
		SDL_Flip(_win);
		return 0;
	}
	else
		return -1;
}

int SDLWindow::blit_ztats()
{
	ZtatsWin::Instance().blit();
	return 0;

	// SDL_Rect dstRect;
	// dstRect.x = _interior->w + 2 * _frame_icon_size - 6;
	// dstRect.y = _frame_icon_size - 2;
	// dstRect.w = _ztats->w;
	// dstRect.h = _ztats->h;

	// if (SDL_BlitSurface(_ztats, NULL, _win, &dstRect) == 0) {
	//   SDL_Flip(_win);
	//   return 0;
	// }
	// else
	//   return -1;
}

int SDLWindow::blit_console()
{
	SDL_Rect dstRect;
	dstRect.x = _interior->w + 2 * _frame_icon_size - 6;
	dstRect.y = _h / 2 + _frame_icon_size - 2;
	dstRect.w = _console->w;
	dstRect.h = _console->h;

	blit_mini_win();

	if (SDL_BlitSurface(_console, NULL, _win, &dstRect) == 0) {
		SDL_Flip(_win);
		return 0;
	}
	else
		return -1;
}

int SDLWindow::blit_tiny_win()
{
	TinyWin::Instance().blit();
	return 0;
}

int SDLWindow::blit_mini_win()
{
	MiniWin::Instance().blit();
	return 0;

	// SDL_Rect dstRect;
	// dstRect.x = _interior->w + 2 * _frame_icon_size - 6;
	// dstRect.y = _ztats->h + _frame_icon_size * 2 - 6;
	// dstRect.w = _mini_win->w;
	// dstRect.h = _mini_win->h;

	// if (SDL_BlitSurface(_mini_win, NULL, _win, &dstRect) == 0) {
	//   SDL_Flip(_win);
	//   return 0;
	// }
	// else
	//   return -1;
}

int SDLWindow::scroll_console(int amount, int delay)
{
	if (amount > 0) {
		SDL_Rect srcRect, fillRect;
		srcRect.x = 0;
		srcRect.y = 1;
		srcRect.w = _console->w;
		srcRect.h = _console->h;
		fillRect.x = 0;
		fillRect.y = _console->h - 1;
		fillRect.w = _console->w;
		fillRect.h = 1;

		if (SDL_BlitSurface(_console, &srcRect, _console, NULL) == 0) {
			SDL_FillRect(_console, &fillRect, SDL_MapRGB(_console->format, 0, 0, 0));

			// TODO: Ignoring delay for now.
			// SDL_Delay(delay);

			blit_console();
			scroll_console(amount - 1, delay);
		}
		else {
			std::cerr << "ERROR: sdlwindow.cc: sdlwindow::scroll_console() FAILED\n";
			return -1;
		}
	}

	return 0;
}

int SDLWindow::frame_icon_size()
{
	return _frame_icon_size;
}

// TODO: Add two args for x y offsets with default values 0.
int SDLWindow::draw_frame(int _arena_width, int _arena_height)
{
	SDL_Surface* _frame = NULL;

	boost::filesystem::path frame_path((std::string)DATADIR);
	frame_path = frame_path / (std::string)PACKAGE_NAME / "data" / "frame.png";

	// Load png
	if (!(_frame = IMG_Load(frame_path.c_str()))) {
		std::cerr << "ERROR: sdlwindow.cc: Couldn't load frame png: " << IMG_GetError() << std::endl;
		return -1;
	}

	// Fill frame vector
	for (int y = 0; y < _frame->h; y += _frame_icon_size) {
		for (int x = 0; x < _frame->w - 1; x += _frame_icon_size) {
			SDL_Rect srcRect;
			srcRect.x = x;
			srcRect.y = y;
			srcRect.w = _frame_icon_size;
			srcRect.h = _frame_icon_size;

			SDL_Surface* surface;

			if (!(surface = SDL_CreateRGBSurface(SDL_SWSURFACE, _frame_icon_size, _frame_icon_size, 32,
					rmask, gmask, bmask, amask)))
			{
				std::cerr << "ERROR: sdlwindow.cc: Couldn't create frame surface: " << IMG_GetError() << std::endl;
				return -1;
			}

			// Alpha, alternatively:
			// http://www.sdltutorials.com/sdl-image/
			// SDL_SetAlpha(_ptr_icon_surf, !SDL_SRCALPHA, 255);
			SDL_BlitSurface(_frame, &srcRect, surface, NULL);
			_vec_frameicons.push_back(surface);
		}
	}

	SDL_FreeSurface(_frame);

	// Blit left, big frame
	SDL_Rect tile_dst_rect;
	for (int y = 0; y < _arena_height*2; y++) {
		for (int x = 0; x < _arena_width*2; x++) {
			tile_dst_rect.x = x * _frame_icon_size;
			tile_dst_rect.y = y * _frame_icon_size;
			tile_dst_rect.w = _frame_icon_size;
			tile_dst_rect.h = _frame_icon_size;

			//  _
			// |
			if (tile_dst_rect.x == 0 && tile_dst_rect.y == 0)
				SDL_BlitSurface(_vec_frameicons[0], NULL, _win, &tile_dst_rect);
			// _
			//  |
			else if (tile_dst_rect.x ==
					_frame_icon_size * _arena_width * 2 - _frame_icon_size
					&& tile_dst_rect.y == 0)
				SDL_BlitSurface(_vec_frameicons[2], NULL, _win, &tile_dst_rect);
			// |_
			else if (tile_dst_rect.x == 0 && tile_dst_rect.y >=
					_frame_icon_size * _arena_height * 2 - _frame_icon_size)
				SDL_BlitSurface(_vec_frameicons[8], NULL, _win, &tile_dst_rect);
			// _|
			else if (tile_dst_rect.x ==
					_frame_icon_size * _arena_width * 2 - _frame_icon_size
					&& tile_dst_rect.y ==
							_frame_icon_size * _arena_height * 2 - _frame_icon_size)
				SDL_BlitSurface(_vec_frameicons[10], NULL, _win, &tile_dst_rect);
			// |
			else if (tile_dst_rect.x == 0 && tile_dst_rect.y > 0)
				SDL_BlitSurface(_vec_frameicons[7], NULL, _win, &tile_dst_rect);
			// _
			else if (tile_dst_rect.x >= 0 && tile_dst_rect.y == 0)
				SDL_BlitSurface(_vec_frameicons[13], NULL, _win, &tile_dst_rect);
			// _
			else if (tile_dst_rect.x >= 0 && tile_dst_rect.y ==
					_frame_icon_size * _arena_height * 2 - _frame_icon_size)
				SDL_BlitSurface(_vec_frameicons[13], NULL, _win, &tile_dst_rect);
			// |
			else if (tile_dst_rect.x ==
					_frame_icon_size * _arena_width * 2 - _frame_icon_size
					&& tile_dst_rect.y > 0)
				SDL_BlitSurface(_vec_frameicons[7], NULL, _win, &tile_dst_rect);
		}
	}

	if (! (_interior =
			SDL_CreateRGBSurface
			(SDL_SWSURFACE,
					// (SDL_HWSURFACE | SDL_SRCALPHA,
					4 + (2*_arena_width*_frame_icon_size) - (2*_frame_icon_size),
					4 + (2*_arena_height*_frame_icon_size) - (2*_frame_icon_size),
					32, rmask, gmask, bmask, amask)))
		return -1;

	// Determine what's left on the rhs of the screen, after the arena
	// was drawn.
	int rest_width = (_w - 2 * _arena_width * _frame_icon_size) / _frame_icon_size;

	// Blit two right frames for game messages
	for (int y = 0; y < _arena_height * 2; y++) {
		for (int x = 0; x <= rest_width; x++) {
			tile_dst_rect.x =
					(2 * _arena_width * _frame_icon_size) +
					(x * _frame_icon_size) - _frame_icon_size;
			tile_dst_rect.y = y * _frame_icon_size;
			tile_dst_rect.w = _frame_icon_size;
			tile_dst_rect.h = _frame_icon_size;

			// _ _
			//  |
			if (x == 0 && y == 0)
				SDL_BlitSurface(_vec_frameicons[1], NULL, _win, &tile_dst_rect);
			// _|_
			else if (x == 0 && y == _arena_height * 2 - 1)
				SDL_BlitSurface(_vec_frameicons[9], NULL, _win, &tile_dst_rect);
			// _|
			else if (x == rest_width && y == _arena_height * 2 - 1)
				SDL_BlitSurface(_vec_frameicons[10], NULL, _win, &tile_dst_rect);
			else if (x == rest_width && y == 0)
				SDL_BlitSurface(_vec_frameicons[2], NULL, _win, &tile_dst_rect);
			// _
			else if (x > 0 && y == 0)
				SDL_BlitSurface(_vec_frameicons[13], NULL, _win, &tile_dst_rect);

			// _|
			//  |
			else if (x == rest_width &&
					y == _arena_height)
				SDL_BlitSurface(_vec_frameicons[6], NULL, _win, &tile_dst_rect);
			// |_
			// |
			else if (x == 0 &&
					y == _arena_height)
				SDL_BlitSurface(_vec_frameicons[4], NULL, _win, &tile_dst_rect);
			// _
			else if (x > 0 && y == _arena_height)
				SDL_BlitSurface(_vec_frameicons[13], NULL, _win, &tile_dst_rect);

			// _|
			//  |
			else if (x == rest_width &&
					y == _arena_height - 2)
				SDL_BlitSurface(_vec_frameicons[6], NULL, _win, &tile_dst_rect);
			// |_
			// |
			else if (x == 0 &&
					y == _arena_height - 2)
				SDL_BlitSurface(_vec_frameicons[4], NULL, _win, &tile_dst_rect);
			// _
			else if (x > 0 && y == _arena_height - 2)
				SDL_BlitSurface(_vec_frameicons[13], NULL, _win, &tile_dst_rect);

			// _|
			//  |
			else if (x == rest_width &&
					y == _arena_height - 5)
				SDL_BlitSurface(_vec_frameicons[6], NULL, _win, &tile_dst_rect);
			// |_
			// |
			else if (x == 0 &&
					y == _arena_height - 5)
				SDL_BlitSurface(_vec_frameicons[4], NULL, _win, &tile_dst_rect);
			// _
			else if (x > 0 && y == _arena_height - 5)
				SDL_BlitSurface(_vec_frameicons[13], NULL, _win, &tile_dst_rect);

			// _
			else if (x > 0 && y == _arena_height * 2 - 1)
				SDL_BlitSurface(_vec_frameicons[13], NULL, _win, &tile_dst_rect);
			// (rhs) |
			else if (x == rest_width && y > 0)
				SDL_BlitSurface(_vec_frameicons[7], NULL, _win, &tile_dst_rect);
		}
	}

	// Update window
	return SDL_Flip(_win);
}
