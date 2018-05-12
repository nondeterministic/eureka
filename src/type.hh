// This source file is part of eureka
//
// Copyright (c) 2007-2018 Andreas Bauer <a@pspace.org>
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

#ifndef TYPE_HH
#define TYPE_HH

#include <map>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class Type
{
public:
	Type();
	virtual ~Type();
	virtual int load_charset() = 0;
	void col_printch(SDL_Texture*, int, int, int, SDL_Color, SDL_Color);
	void printch(SDL_Texture*, int, int = -1, int = -1, SDL_Color* = NULL, SDL_Color* = NULL);
	int char_width();
	int char_height();
	void set_select(bool);
	void toggle_select();

protected:
	std::map<int, SDL_Texture*> _map_chars;
	int _w, _h;
	bool _select;
};

#endif
