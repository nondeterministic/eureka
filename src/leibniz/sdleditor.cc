// This source file is part of eureka
//
// Copyright (c) 2007-2018  Andreas Bauer <baueran@gmail.com>
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

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../world.hh"
#include "../map.hh"
#include "sdleditor.hh"
#include "indoorssdleditor.hh"
#include "outdoorssdleditor.hh"
#include "editorwin.hh"
#include "../indoorsicons.hh"
#include "../outdoorsicons.hh"

SDLEditor::SDLEditor()
{
	_show_map = true;
	_show_obj = true;
	_show_grid = true;
	_show_act = true;

	_texture = NULL;
	_sdl_window = NULL;
	_renderer = NULL;
	_event_box = NULL;

	_width = 0;
	_height = 0;
}

SDLEditor::~SDLEditor()
{
	std::cout << "INFO: sdleditor.cc: in destructor, tyring to destroy SDL renderer...\n";
	SDL_DestroyRenderer(_renderer);
	_renderer = NULL;
	std::cout << "INFO: sdleditor.cc: in destructor, tyring to destroy SDL window...\n";
	SDL_DestroyWindow(_sdl_window);
	_sdl_window = NULL;
	std::cout << "INFO: sdleditor.cc: in destructor, tyring to quit SDL...\n";
	SDL_Quit();
	std::cout << "INFO: sdleditor.cc: finishing destructor.\n";
}

SDLEditor* SDLEditor::create(std::string type, std::string name)
{
	try {
		std::shared_ptr<Map> map = World::Instance().get_map(name.c_str());

		if (type == "indoors")
			return new IndoorsSDLEditor(map);
		else
			return new OutdoorsSDLEditor(map);
	}
	catch(MapNotFound &e) {
		std::cerr << "Exception: sdleditor.cc: " << e.print() << "\n";
		return NULL;
	}
}

void SDLEditor::set_show_map(bool status)
{
	_show_map = status;
}

void SDLEditor::set_show_obj(bool status)
{
	_show_obj = status;
}

void SDLEditor::set_show_act(bool status)
{
	_show_act = status;
}

int SDLEditor::clear()
{
	if (SDL_SetRenderTarget(_renderer, _texture) < 0)
		return -1;

	if (SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 0) < 0)
		return -1;

	return SDL_RenderClear(_renderer);
}

int SDLEditor::refresh(void)
{
	if (SDL_SetRenderTarget(_renderer, NULL) < 0) {
		std::cerr << "ERROR: sdleditor.cc: could not set render target in refresh: " << IMG_GetError() << std::endl;
		return -1;
	}

	if (SDL_RenderCopy(_renderer, _texture, NULL, NULL) < 0) {
		std::cerr << "ERROR: sdleditor.cc: could not render copy in refresh: " << IMG_GetError() << std::endl;
		return -1;
	}

	if (SDL_UpdateWindowSurface(_sdl_window) < 0) {
		std::cerr << "ERROR: sdleditor.cc: could not update window surface in refresh: " << IMG_GetError() << std::endl;
		return -1;
	}

	return 0;
}

void SDLEditor::resize(unsigned w, unsigned h)
{
	if (w != _width || h != _height) {
		_width = w;
		_height = h;

		SDL_DestroyTexture(_texture);
		_texture  = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_TARGET, _width, _height);

		int w, h;
		if (SDL_QueryTexture(_texture, NULL, NULL, &w, &h) < 0) {
			std::cerr << "ERROR: sdleditor.cc: Failed to create texture in resize().\n";
			return;
		}

		SDL_SetWindowSize(_sdl_window, _width, _height);
		refresh();
	}
}

void SDLEditor::open_display(Gtk::EventBox* event_box, unsigned width, unsigned height)
{
	_event_box = event_box;
	_width = width;
	_height = height;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    	std::cerr << "ERROR: SDL_Init() failed: " << SDL_GetError() << std::endl;
		throw std::runtime_error("ERROR: sdleditor.cc: Error initialising SDL.");
    }

    Window _window = GDK_WINDOW_XID(Glib::unwrap(_event_box->get_window()));
	if ((_sdl_window = SDL_CreateWindowFrom((const void*)_window)) == NULL) {
		std::cerr << "ERROR: sdleditor.cc: Failed to create the SDL window.\n";
		exit(EXIT_FAILURE);
	}
//	std::cout << "aa\n";
//	SDL_DestroyWindow(_sdl_window);
//	std::cout << "bb\n";
//	exit(EXIT_SUCCESS);

	SDL_SetWindowResizable(_sdl_window, SDL_TRUE);

	_renderer = SDL_CreateRenderer(_sdl_window, -1, SDL_RENDERER_SOFTWARE | SDL_RENDERER_TARGETTEXTURE);
	_texture  = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_TARGET, _width, _height);

	int w, h;
	if (SDL_QueryTexture(_texture, NULL, NULL, &w, &h) < 0) {
		std::cerr << "ERROR: sdleditor.cc: Failed to create texture.\n";
		exit(EXIT_FAILURE);
	}

	if (convert_icons_to_textures(_renderer) < 0) {
		std::cerr << "ERROR: sdleditor.cc: Failed to convert icons.\n";
		exit(EXIT_FAILURE);
	}

	std::cout << "INFO: sdleditor.cc: SDL initialised.\n";
}

bool SDLEditor::convert_icons_to_textures(SDL_Renderer* renderer)
{
	// We need to convert both or when saving map, the number of tiles of one could be 0,
	// and the icon properties deleted in world XML-file.
	return IndoorsIcons::Instance().convert_icons_to_textures(renderer) >= 0 &&
			OutdoorsIcons::Instance().convert_icons_to_textures(renderer) >= 0;
}
