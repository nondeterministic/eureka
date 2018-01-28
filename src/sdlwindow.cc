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

#include "sdlwindow.hh"
#include "util.hh"
#include "config.h"

#include "ztatswin.hh"
#include "miniwin.hh"
#include "tinywin.hh"
#include "console.hh"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <string>
#include <iostream>

#include "boost/filesystem.hpp"
#include "boost/filesystem/path.hpp"

SDLWindow::SDLWindow()
{
	_w = 0;
	_h = 0;
	_x = 0;
	_y = 0;

	_arena_w = -1;
	_arena_h = -1;
	_renderer = NULL;
	_win      = NULL;
	_texture_entire_window = NULL;
	_texture_arena = NULL;
	_texture_tiny_win = NULL;
	_texture_console  = NULL;
	_texture_ztats    = NULL;
	_texture_mini_win = NULL;

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
	// std::cout << "INFO: sdlwindow.cc: ~SDLWindow().\n";

	// Iterate through frame vector and SDL_FreeSurface on all entries!
	for (std::vector<SDL_Texture*>::iterator curr_icon = _vec_frameicons.begin(); curr_icon != _vec_frameicons.end(); curr_icon++)
		SDL_DestroyTexture(*curr_icon);
	_vec_frameicons.clear();

	if (_texture_arena)
		SDL_DestroyTexture(_texture_arena);
	if (_texture_console)
		SDL_DestroyTexture(_texture_console);
	if (_texture_ztats)
		SDL_DestroyTexture(_texture_ztats);
	if (_texture_mini_win)
		SDL_DestroyTexture(_texture_mini_win);
	if (_texture_tiny_win)
		SDL_DestroyTexture(_texture_tiny_win);
}

SDLWindow& SDLWindow::Instance()
{
	static SDLWindow _inst;
	return _inst;
}

int SDLWindow::init(int width, int height, int bpp, Uint32 flags)
{
	if (_win != NULL) {
		std::cerr << "WARNING: SDLWindow::init() called more than once.\n";
		return -1;
	}

	std::cout << "INFO: SDLWindow::init() called.\n";

	_w = width; _h = height;

	if (SDL_Init(flags) != 0) {
		std::cerr << "ERROR: sdlwindow.cc: Could not initialize SDL.\n";
		return -1;
	}

	if (!(_win = SDL_CreateWindow(
						PACKAGE_STRING,
						SDL_WINDOWPOS_UNDEFINED,
						SDL_WINDOWPOS_UNDEFINED,
						_w, _h, 0)))
	{
		std::cerr << "ERROR: sdlwindow.cc: Could not create initial SDL window.\n";
		return -1;
	}
	else
		std::cout << "INFO: sdlwindow.cc: Created SDL window " << _w << "x" << _h << "\n";

	if (!(_renderer = SDL_CreateRenderer(_win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE))) {
		std::cerr << "ERROR: sdlwindow.cc: Could not create initial SDL renderer.\n";
		return -1;
	}

	SDL_RendererInfo drinfo;
	SDL_GetRendererInfo(_renderer, &drinfo);
	if (drinfo.flags & SDL_RENDERER_ACCELERATED)
		std::cout << "INFO: sdlwindow.cc: SDL renderer supports accelerated rendering.\n";
	else {
		std::cerr << "ERROR: sdlwindow.cc: SDL renderer doesn't support accelerated rendering.\n";
		return -1;
	}

	// if (Mix_OpenAudio(22050,AUDIO_S16SYS,2,640) != 0)
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) != 0)
		std::cerr << "WARNING: sdlwindow.cc: Could not initialize audio.\n";
	Mix_AllocateChannels(16);

	return 0;
}

SDL_Window* SDLWindow::get_native_sdl_window()
{
	return _win;
}

void SDLWindow::clear_texture_arena()
{
	if ( (SDL_SetRenderTarget(_renderer, _texture_arena) == 0) &&
			(SDL_SetRenderDrawColor(_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE) == 0) &&
				(SDL_RenderClear(_renderer) == 0) )
	{
		resetRenderer();
	}
	else {
		std::cerr << "WARNING: sdlwindow.cc: clear_texture_arena() failed: " << IMG_GetError() << std::endl;
		return;
	}
}

/// Return dimensions of the window (not just of a texture within it, but of the window itself).

std::pair<int,int> SDLWindow::get_size()
{
	return std::make_pair(_w,_h);
}

void SDLWindow::close()
{
	SDL_Quit();
}

int SDLWindow::create_texture_entire_window()
{
	if (!(_texture_entire_window = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_TARGET, _w, _h))) {
		std::cerr << "ERROR: sdlwindow.cc: Could not create entire_window_texture.\n";
		return -1;
	}

	return 0;
}

int SDLWindow::create_texture_console()
{
	if (!(_texture_console = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_TARGET,
											   _w-_arena_w-2.5*(float)_frame_icon_size,
											   ((float)_arena_h)/2.0-_frame_icon_size+2)))
	{
		std::cerr << "ERROR: sdlwindow.cc: Could not create console texture.\n";
		return -1;
	}

	return 0;
}

int SDLWindow::create_texture_ztats()
{
	if (!(_texture_ztats = SDL_CreateTexture(_renderer,	SDL_PIXELFORMAT_RGB888,	SDL_TEXTUREACCESS_TARGET,
											 _w-_arena_w-2.5*(float)_frame_icon_size,
											 ((float)_arena_h)/2.0-(5*_frame_icon_size)+2)))
	{
		std::cerr << "ERROR: sdlwindow.cc: Could not create ztats texture.\n";
		return -1;
	}

	return 0;
}

int SDLWindow::create_texture_mini_win()
{
	if (!(_texture_mini_win = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_TARGET,
												_w-_arena_w-2.5*(float)_frame_icon_size,
												_frame_icon_size * 2 + 4)))
	{
		std::cerr << "ERROR: sdlwindow.cc: Could not create mini_win texture.\n";
		return -1;
	}

	return 0;
}

int SDLWindow::create_texture_tiny_win()
{
	if (!(_texture_tiny_win = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_TARGET,
												_w-_arena_w-2.5*(float)_frame_icon_size,
												_frame_icon_size)))
	{
		std::cerr << "ERROR: sdlwindow.cc: Could not create tiny_win texture.\n";
		return -1;
	}

	return 0;
}

SDL_Texture* SDLWindow::get_texture_entire_win()
{
	if (!_texture_entire_window)
		throw SurfaceNULL("SDLWindow::get_texture_entire_win(): texture is NULL");
	return _texture_entire_window;
}

SDL_Texture* SDLWindow::get_texture_arena()
{
	if (!_texture_arena)
		throw SurfaceNULL("SDLWindow::get_texture_arena(): texture is NULL");
	return _texture_arena;
}

SDL_Texture* SDLWindow::get_texture_console()
{
	if (!_texture_console)
		throw SurfaceNULL("SDLWindow::get_console_SDL_surface: texture is NULL");
	return _texture_console;
}

SDL_Texture* SDLWindow::get_texture_ztats()
{
	if (!_texture_ztats)
		throw SurfaceNULL("SDLWindow::get_ztats_SDL_surface: texture is NULL");
	return _texture_ztats;
}

SDL_Texture* SDLWindow::get_texture_mini_win()
{
	if (!_texture_mini_win)
		throw SurfaceNULL("SDLWindow::get_mini_win_SDL_surface: texture is NULL");
	return _texture_mini_win;
}

SDL_Texture* SDLWindow::get_texture_tiny_win()
{
	if (!_texture_tiny_win)
		throw SurfaceNULL("SDLWindow::get_tiny_win_SDL_surface: texture is NULL");
	return _texture_tiny_win;
}

int SDLWindow::blit_all()
{
	int result = 0;

	result = std::min(result, blit_ztats());
	result = std::min(result, blit_tiny_win());
	result = std::min(result, blit_mini_win());
	result = std::min(result, blit_console());
	result = std::min(result, blit_arena());
	result = std::min(result, blit_entire_window_texture());

	return result;
}

/// Blit that texture that holds the blue frame. I.e., blit this first, and then the other textures on top.
/// The texture_entire_window is some kind of background texture.

int SDLWindow::blit_entire_window_texture()
{
	int result = -1;

	if (SDL_SetRenderTarget(_renderer, NULL) == 0)
		result = SDL_RenderCopy(_renderer, _texture_entire_window, NULL, NULL);
	else
		return result;

	// Might be necessary due to some refresh / double buffering stuff that I read about but do not fully understand.  :-P
	SDL_RenderPresent(_renderer);
	SDL_RenderPresent(_renderer);
	SDL_RenderPresent(_renderer);

	return result;
}

/// If icon_size != 0, then the value is used as an offset to make a hex arena fill the screen properly.

int SDLWindow::blit_arena()
{
	SDL_Rect dstRect;
	dstRect.x = _frame_icon_size - 2;
	dstRect.y = _frame_icon_size - 2;
	dstRect.w = _arena_w;
	dstRect.h = _arena_h;

	if (SDL_SetRenderTarget(_renderer, _texture_entire_window) == 0 && SDL_RenderCopy(_renderer, _texture_arena, NULL, &dstRect) == 0)
		return blit_entire_window_texture();
	return -1;
}

int SDLWindow::blit_ztats()
{
	return ZtatsWin::Instance().blit();
}

int SDLWindow::blit_tiny_win()
{
	return TinyWin::Instance().blit();
}

int SDLWindow::blit_mini_win()
{
	return MiniWin::Instance().blit();
}

int SDLWindow::blit_console()
{
	int console_w = Console::Instance().get_size().first;
	int console_h = Console::Instance().get_size().second;

	SDL_Rect dstRect;
	dstRect.x = _w - _frame_icon_size - console_w;
	dstRect.y = _h - _frame_icon_size - console_h;
	dstRect.w = console_w;
	dstRect.h = console_h;

	if (SDL_SetRenderTarget(_renderer, _texture_entire_window) < 0)
		std::cerr << "ERROR: sdlwindow.cc: blit_console: resetRenderer failed: " << IMG_GetError() << "." << std::endl;

	if (SDL_RenderCopy(_renderer, _texture_console, NULL, &dstRect) == 0)
		return blit_entire_window_texture();
	else {
		std::cerr << "ERROR: sdlwindow.cc: blit_console: RenderCopy failed: " << IMG_GetError() << "." << std::endl;
		return -1;
	}

	return 0;
}

int SDLWindow::frame_icon_size()
{
	return _frame_icon_size;
}

// TODO: Add two args for x y offsets with default values 0.
int SDLWindow::draw_frame(int arena_width, int arena_height)
{
	boost::filesystem::path frame_path((std::string)DATADIR);
	frame_path = frame_path / (std::string)PACKAGE_NAME / "data" / "frame.png";

	// Load PNG
	SDL_Surface* tmp_frame_surface = NULL;
	if (!(tmp_frame_surface = IMG_Load(frame_path.c_str()))) {
		std::cerr << "ERROR: sdlwindow.cc: Couldn't load frame PNG: " << IMG_GetError() << std::endl;
		return -1;
	}

	// Create identical texture copy from surface.
 	SDL_Texture* tmp_frame_texture = NULL;
	if (!(tmp_frame_texture = SDL_CreateTextureFromSurface(_renderer, tmp_frame_surface))) {
		std::cerr << "ERROR: sdlwindow.cc: Could not create _frame_texture.\n";
		return -1;
	}

	// Fill frame vector
	for (int y = 0; y < tmp_frame_surface->h; y += _frame_icon_size) {
		for (int x = 0; x < tmp_frame_surface->w - 1; x += _frame_icon_size) {
			SDL_Rect srcRect;
			srcRect.x = x;
			srcRect.y = y;
			srcRect.w = _frame_icon_size;
			srcRect.h = _frame_icon_size;

			SDL_Texture* tmp_frame_tile_texture =
					SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
									  _frame_icon_size, _frame_icon_size);

			if (!tmp_frame_tile_texture) {
				std::cerr << "ERROR: sdlwindow.cc: Couldn't create tmp_surface: " << IMG_GetError() << std::endl;
				return -1;
			}

			if (SDL_SetRenderTarget(_renderer, tmp_frame_tile_texture) < 0) {
				std::cerr << "ERROR: sdlwindow.cc: Couldn't create temporary frame tile texture: " << IMG_GetError() << std::endl;
				return -1;
			}

			if (SDL_RenderCopy(_renderer, tmp_frame_texture, &srcRect, NULL) < 0) {
				std::cerr << "ERROR: sdlwindow.cc: Couldn't RenderCopy into frame vector: " << IMG_GetError() << std::endl;
				return -1;
			}

			_vec_frameicons.push_back(tmp_frame_tile_texture);
		}
	}
	SDL_FreeSurface(tmp_frame_surface);
	SDL_DestroyTexture(tmp_frame_texture);

	// /////////////////////////////////////////////////////////////////////////////////////
	// Now draw the actual frame...
	// /////////////////////////////////////////////////////////////////////////////////////

	// Create main texture first
	if (create_texture_entire_window() != 0) {
		std::cerr << "ERROR: Cannot create main window's main texture.\n";
		return -1;
	}

	if (SDL_SetRenderTarget(_renderer, _texture_entire_window) < 0) {
		std::cerr << "ERROR: sdlwindow.cc: set render target to main window's main window: " << IMG_GetError() << std::endl;
		return -1;
	}
	SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 0);
	SDL_RenderClear(_renderer);

	// Blit left, big frame
	SDL_Rect tile_dst_rect;
	for (int y = 0; y < arena_height*2; y++) {
		for (int x = 0; x < arena_width*2; x++) {
			tile_dst_rect.x = x * _frame_icon_size;
			tile_dst_rect.y = y * _frame_icon_size;
			tile_dst_rect.w = _frame_icon_size;
			tile_dst_rect.h = _frame_icon_size;

			//  _
			// |
			if (tile_dst_rect.x == 0 && tile_dst_rect.y == 0)
				SDL_RenderCopy(_renderer, _vec_frameicons[0], NULL, &tile_dst_rect);
			// SDL_BlitSurface(_vec_frameicons[0], NULL, _win, &tile_dst_rect);
			// _
			//  |
			else if (tile_dst_rect.x ==	_frame_icon_size * arena_width * 2 - _frame_icon_size
					&& tile_dst_rect.y == 0)
				SDL_RenderCopy(_renderer, _vec_frameicons[2], NULL, &tile_dst_rect);
			// SDL_BlitSurface(_vec_frameicons[2], NULL, _win, &tile_dst_rect);
			// |_
			else if (tile_dst_rect.x == 0
					&& tile_dst_rect.y >= _frame_icon_size * arena_height * 2 - _frame_icon_size)
				SDL_RenderCopy(_renderer, _vec_frameicons[8], NULL, &tile_dst_rect);
			// SDL_BlitSurface(_vec_frameicons[8], NULL, _win, &tile_dst_rect);
			// _|
			else if (tile_dst_rect.x == _frame_icon_size * arena_width * 2 - _frame_icon_size
					&& tile_dst_rect.y == _frame_icon_size * arena_height * 2 - _frame_icon_size)
				SDL_RenderCopy(_renderer, _vec_frameicons[10], NULL, &tile_dst_rect);
			// SDL_BlitSurface(_vec_frameicons[10], NULL, _win, &tile_dst_rect);
			// |
			else if (tile_dst_rect.x == 0 && tile_dst_rect.y > 0)
				SDL_RenderCopy(_renderer, _vec_frameicons[7], NULL, &tile_dst_rect);
			// SDL_BlitSurface(_vec_frameicons[7], NULL, _win, &tile_dst_rect);
			// _
			else if (tile_dst_rect.x >= 0 && tile_dst_rect.y == 0)
				SDL_RenderCopy(_renderer, _vec_frameicons[13], NULL, &tile_dst_rect);
			//	SDL_BlitSurface(_vec_frameicons[13], NULL, _win, &tile_dst_rect);
			// _
			else if (tile_dst_rect.x >= 0 && tile_dst_rect.y ==
					_frame_icon_size * arena_height * 2 - _frame_icon_size)
				SDL_RenderCopy(_renderer, _vec_frameicons[13], NULL, &tile_dst_rect);
			// SDL_BlitSurface(_vec_frameicons[13], NULL, _win, &tile_dst_rect);
			// |
			else if (tile_dst_rect.x ==	_frame_icon_size * arena_width * 2 - _frame_icon_size
					&& tile_dst_rect.y > 0)
				SDL_RenderCopy(_renderer, _vec_frameicons[7], NULL, &tile_dst_rect);
			// SDL_BlitSurface(_vec_frameicons[7], NULL, _win, &tile_dst_rect);
		}
	}

	_arena_w = 4 + (2*arena_width*_frame_icon_size) - (2*_frame_icon_size);
	_arena_h = 4 + (2*arena_height*_frame_icon_size) - (2*_frame_icon_size);

	if (!(_texture_arena = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_TARGET, _arena_w, _arena_h)))
		return -1;
	else
		std::cout << "INFO: sdlwindow.cc: Created arena texture inside with dimensions w: " << _arena_w << ", h: " << _arena_h <<
				 	 ", inside SDL-Window with dimensions: w: " << _w << ", h: " << _h << "\n";

	// Determine what's left on the rhs of the screen, after the arena was drawn.
	int rest_width = (_w - 2 * arena_width * _frame_icon_size) / _frame_icon_size;

	// Blit two right frames for game messages
	for (int y = 0; y < arena_height * 2; y++) {
		for (int x = 0; x <= rest_width; x++) {
			tile_dst_rect.x =
					(2 * arena_width * _frame_icon_size) +
					(x * _frame_icon_size) - _frame_icon_size;
			tile_dst_rect.y = y * _frame_icon_size;
			tile_dst_rect.w = _frame_icon_size;
			tile_dst_rect.h = _frame_icon_size;

			// _ _
			//  |
			if (x == 0 && y == 0)
				SDL_RenderCopy(_renderer, _vec_frameicons[1], NULL, &tile_dst_rect);
			// SDL_BlitSurface(_vec_frameicons[1], NULL, _win, &tile_dst_rect);
			// _|_
			else if (x == 0 && y == arena_height * 2 - 1)
				SDL_RenderCopy(_renderer, _vec_frameicons[9], NULL, &tile_dst_rect);
			// SDL_BlitSurface(_vec_frameicons[9], NULL, _win, &tile_dst_rect);
			// _|
			else if (x == rest_width && y == arena_height * 2 - 1)
				SDL_RenderCopy(_renderer, _vec_frameicons[10], NULL, &tile_dst_rect);
			// SDL_BlitSurface(_vec_frameicons[10], NULL, _win, &tile_dst_rect);
			else if (x == rest_width && y == 0)
				SDL_RenderCopy(_renderer, _vec_frameicons[2], NULL, &tile_dst_rect);
			// SDL_BlitSurface(_vec_frameicons[2], NULL, _win, &tile_dst_rect);
			// _
			else if (x > 0 && y == 0)
				SDL_RenderCopy(_renderer, _vec_frameicons[13], NULL, &tile_dst_rect);
			// SDL_BlitSurface(_vec_frameicons[13], NULL, _win, &tile_dst_rect);

			// _|
			//  |
			else if (x == rest_width && y == arena_height)
				SDL_RenderCopy(_renderer, _vec_frameicons[6], NULL, &tile_dst_rect);
			// SDL_BlitSurface(_vec_frameicons[6], NULL, _win, &tile_dst_rect);
			// |_
			// |
			else if (x == 0 && y == arena_height)
				SDL_RenderCopy(_renderer, _vec_frameicons[4], NULL, &tile_dst_rect);
			// SDL_BlitSurface(_vec_frameicons[4], NULL, _win, &tile_dst_rect);
			// _
			else if (x > 0 && y == arena_height)
				SDL_RenderCopy(_renderer, _vec_frameicons[13], NULL, &tile_dst_rect);
			// SDL_BlitSurface(_vec_frameicons[13], NULL, _win, &tile_dst_rect);
			// _|
			//  |
			else if (x == rest_width && y == arena_height - 2)
				SDL_RenderCopy(_renderer, _vec_frameicons[6], NULL, &tile_dst_rect);
			// SDL_BlitSurface(_vec_frameicons[6], NULL, _win, &tile_dst_rect);
			// |_
			// |
			else if (x == 0 && y == arena_height - 2)
				SDL_RenderCopy(_renderer, _vec_frameicons[4], NULL, &tile_dst_rect);
			// SDL_BlitSurface(_vec_frameicons[4], NULL, _win, &tile_dst_rect);
			// _
			else if (x > 0 && y == arena_height - 2)
				SDL_RenderCopy(_renderer, _vec_frameicons[13], NULL, &tile_dst_rect);
			// SDL_BlitSurface(_vec_frameicons[13], NULL, _win, &tile_dst_rect);

			// _|
			//  |
			else if (x == rest_width &&	y == arena_height - 5)
				SDL_RenderCopy(_renderer, _vec_frameicons[6], NULL, &tile_dst_rect);
			// SDL_BlitSurface(_vec_frameicons[6], NULL, _win, &tile_dst_rect);
			// |_
			// |
			else if (x == 0 &&y == arena_height - 5)
				SDL_RenderCopy(_renderer, _vec_frameicons[4], NULL, &tile_dst_rect);
			// SDL_BlitSurface(_vec_frameicons[4], NULL, _win, &tile_dst_rect);
			// _
			else if (x > 0 &&  y == arena_height - 5)
				SDL_RenderCopy(_renderer, _vec_frameicons[13], NULL, &tile_dst_rect);
			// SDL_BlitSurface(_vec_frameicons[13], NULL, _win, &tile_dst_rect);
			// _
			else if (x > 0 && y == arena_height * 2 - 1)
				SDL_RenderCopy(_renderer, _vec_frameicons[13], NULL, &tile_dst_rect);
			// SDL_BlitSurface(_vec_frameicons[13], NULL, _win, &tile_dst_rect);
			// (rhs) |
			else if (x == rest_width && y > 0)
				SDL_RenderCopy(_renderer, _vec_frameicons[7], NULL, &tile_dst_rect);
			// SDL_BlitSurface(_vec_frameicons[7], NULL, _win, &tile_dst_rect);
		}
	}

	// Update window
	SDL_SetRenderTarget(_renderer, NULL);
	SDL_RenderClear(_renderer);
	if (SDL_RenderCopy(_renderer, _texture_entire_window, NULL, NULL) < 0) {
		std::cerr << "ERROR: sdlwindow.cc: could not render copy frame into main window's texture: " << IMG_GetError() << std::endl;
		return -1;
	}
	SDL_RenderPresent(_renderer);

	return 0;
}

SDL_Renderer* SDLWindow::get_renderer() const
{
	return _renderer;
}

/**
 * Makes the renderer's target the main window again.
 */

bool SDLWindow::resetRenderer()
{
	// NULL points renderer to default, I think. _win_content is not the default it seems, but only yet another texture.
	if ((SDL_SetRenderTarget(_renderer, NULL)) < 0) {
		std::cerr << "WARNING: sdlwindow.cc: Couldn't reset renderer: " << IMG_GetError() << std::endl;
		return false;
	}

	return true;
}
