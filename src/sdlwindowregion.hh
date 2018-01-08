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

#ifndef __SDLWINDOWREGION_HH
#define __SDLWINDOWREGION_HH

#include <string>
#include <memory>

#include <SDL2/SDL.h>

#include "util.hh"
#include "charset.hh"
#include "sdlwindow.hh"

class SDLWindowRegion
{
protected:
	Charset* _font;
	SDL_Rect _dimensions;
	SDL_Texture* _texture;
	SDL_Texture* _texture_last;
	SDL_Renderer* _renderer;
	int _y_frame_offset, _x_frame_offset, _inter_line_padding;
	Uint32 rmask, gmask, bmask, amask;

	SDLWindowRegion();
	virtual ~SDLWindowRegion();
	bool resetRenderer();

public:

	void set_dimensions(SDL_Rect dimensions);
	void set_texture(SDL_Texture* s);
	void display_texture(std::shared_ptr<SDL_Texture> s);
	SDL_Texture* get_texture();
	SDL_Rect get_dimensions();
	SDL_Renderer* get_renderer();
	int blit();
	void printch(int x_pos, int y_pos, int c);
	void print_noblit(int x_pos, int y_pos, const std::string s, Alignment align = Alignment::LEFTALIGN);
	void println_noblit(int line, const std::string s, Alignment align = Alignment::LEFTALIGN);
	void print(int x_pos, int y_pos, const std::string s, Alignment align = Alignment::LEFTALIGN);
	void println(int line, const std::string s, Alignment align = Alignment::LEFTALIGN);
	void save_texture();
	void display_last();
	void clear();
	void surface_from_file(std::string);
};

#endif
