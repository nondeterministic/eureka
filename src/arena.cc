// This source file is part of eureka
//
// Copyright (c) 2007-2017 Andreas Bauer <baueran@gmail.com>
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

#include <string>
#include <iostream>
#include <memory>

#include <SDL2/SDL.h>

#include "arena.hh"
#include "world.hh"
#include "map.hh"
#include "hexarena.hh"
#include "squarearena.hh"
#include "indoorsicons.hh"
#include "outdoorsicons.hh"

bool Offsets::operator== (Offsets a)
{
	return a.top == top && a.bot == bot && a.left == left && a.right == right;
}

bool Offsets::operator!= (Offsets a)
{
	return !(a.top == top && a.bot == bot && a.left == left && a.right == right);
}

Arena::Arena()
{
	_sdlwindow = NULL;

	_show_map = true;
	_show_obj = true;
	_show_grid = true;
	_show_act = true;
	_party_is_moving = false;
	_texture = NULL;

	_renderer = SDLWindow::Instance().get_renderer();

	_left_hidden = 0;
	_right_hidden = 0;
	_top_hidden = 0;
	_bot_hidden = 0;
}

Arena::~Arena()
{
}

SDL_Texture* Arena::get_win_texture()
{
	return _texture;
}

std::shared_ptr<Arena> Arena::create(std::string type, std::string name)
{
	try {
		if (type == "indoors") {
			return std::make_shared<SquareArena>(World::Instance().get_map(name.c_str()));
		}
		else {
			return std::make_shared<HexArena>(World::Instance().get_map(name.c_str()));
		}
	}
	catch (MapNotFound& e) {
		std::cerr << "Creation of arena failed: " << e.print() << "\n";
		return NULL;
	}
	catch (std::exception& e) {
		std::cerr << "Creation of arena failed: " << e.what() << "\n";
		return NULL;
	}
}

void Arena::set_map(std::shared_ptr<Map> map)
{
	_map = map;
}

void Arena::set_show_map(bool status)
{
	_show_map = status;
}

void Arena::set_show_obj(bool status)
{
	_show_obj = status;
}

void Arena::set_show_act(bool status)
{
	_show_act = status;
}

void Arena::resetRenderer()
{
	SDLWindow::Instance().resetRenderer();
}

void Arena::moving(bool m)
{
	_party_is_moving = m;
}

bool Arena::is_moving()
{
	return _party_is_moving;
}

void Arena::adjust_offsets(int top, int bot, int left, int right)
{
	_top_hidden += top;
	_bot_hidden += bot;
	_left_hidden += left;
	_right_hidden += right;
}

void Arena::set_offsets(unsigned top,  unsigned bot, unsigned left, unsigned right)
{
	_top_hidden = top;
	_bot_hidden = bot;
	_left_hidden = left;
	_right_hidden = right;
}

void Arena::set_SDLWindow_object(SDLWindow* win)
{
	if (win == NULL) {
		std::cerr << "ERROR: arena.cc: win == NULL.\n";
		exit(EXIT_FAILURE);
	}

	_sdlwindow = win;

	if ((_texture = _sdlwindow->get_texture_arena()) == NULL) {
		std::cerr << "ERROR: arena.cc: couldn't set _texture.\n";
		exit(EXIT_FAILURE);
	}

	int w, h;
	if (SDL_QueryTexture(_texture, NULL, NULL, &w, &h) < 0) {
		std::cerr << "WARNING: arena.cc: cannot set texture: " << IMG_GetError() << "\n";
		exit(EXIT_FAILURE);
	}

	// Clear map, as in hexarena we get otherwise funny artefacts on the corners...
	_sdlwindow->clear_texture_arena();
}

/// x and y are screen coordinates in pixels

int Arena::put_tile(int x, int y, SDL_Texture* tile)
{
	if (x < 0 || y < 0) {
		std::cerr << "WARNING: arena.cc: put_tile called with negative coords." << std::endl;
		return -1;
	}

	if (tile == NULL) {
		std::cerr << "WARNING: arena.cc: Brush to paint tile is NULL. " << std::endl;
		return -1;
	}

	int w, h;
	if (SDL_QueryTexture(tile, NULL, NULL, &w, &h) < 0) {
		std::cerr << "WARNING: arena.cc: put_tile(): tile invalid: " << IMG_GetError() << "\n";
		return -1;
	}

	if (SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND) < 0) {
		std::cerr << "ERROR: arena.cc: setting blend mode not working: " << IMG_GetError() << "\n";
		return -1;
	}

	if (SDL_SetRenderTarget(_renderer, _texture) < 0) {
		std::cerr << "WARNING: arena.cc: put_tile(): cannot change render target: " << IMG_GetError() << "\n";
		return -1;
	}

	SDL_Rect rect = get_tile_coords(x, y);
	if (SDL_RenderCopy(_renderer, tile, NULL, &rect) < 0) {
		std::cerr << "WARNING: arena.cc: put_tile(): render-copy failed: " << IMG_GetError() << "\n";
		return -1;
	}

	return 0;
}

int Arena::blit()
{
	return _sdlwindow->blit_arena();
}
