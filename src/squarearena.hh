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

#ifndef __SQARENA_HH
#define __SQARENA_HH

#include <utility>
#include <memory>

#include <SDL2/SDL.h>

#include "map.hh"
#include "arena.hh"

class SquareArena : public Arena
{
public:
	SquareArena(std::shared_ptr<Map>);
	~SquareArena();

	// Overriding virtual methods
	void show_grid();
	void show_map(int = 0, int = 0);
	std::shared_ptr<Map> get_map() const;
	void set_offset(int, int);
	Offsets move(int);
	Offsets determine_offsets();

	SDL_Rect get_tile_coords(int, int) const;
	void screen_to_map(int, int, int&, int&);
	void map_to_screen(int, int, int&, int&);
	void get_center_coords(int&, int&);
	std::pair<int, int> show_party(int = -1, int = -1);
	unsigned tile_size() const;
	bool adjacent(int, int, int, int);
	unsigned max_y_coordinate();
	unsigned max_x_coordinate();

protected:
	Offsets offsets();
	bool in_los(int, int, int, int);
	bool is_illuminated(int,int);

	// only used inside los:
	void swap(int&, int&);

	int _corner_tile_uneven_offset;
	unsigned _width, _height;

private:
	int _water_anim, _party_anim;
};

#endif
