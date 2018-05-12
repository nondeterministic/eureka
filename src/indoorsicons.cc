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

#include <iostream>
#include <string>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#ifndef EDITOR_COMPILE
#include "eureka.hh"
#else
#include "leibniz.hh"
#endif

#include "indoorsicons.hh"
#include "world.hh"
#include "icons.hh"
#include "config.h"

IndoorsIcons::IndoorsIcons()
{
	// Load indoors icon set
	_icons_surface = IMG_Load((conf_world_path / "images" / "icons_indoors.png").c_str());
	if (_icons_surface == NULL)
		std::cerr << "ERROR: indooricons.cc: Couldn't load indoors icons: " << IMG_GetError() << ": "
				  << (conf_world_path / "images" / "icons_indoors.png").c_str() << std::endl;

	// Determine, how many icons there are
	unsigned icon_size = World::Instance().get_indoors_tile_size();
	int i = 0;

	for (int y = 0; y < _icons_surface->h; y += icon_size)
		for (int x = 0; x < _icons_surface->w; x += icon_size)
			i++;

	// Tell super class how many icons there are for storing as many properties for them.
	reserve(i);
}

IndoorsIcons& IndoorsIcons::Instance()
{
	static IndoorsIcons inst;
	return inst;
}

/// Basically, CONVERT the icons' surfaces to textures, once we have a renderer.
/// Because so far, the icons were only loaded as old-school SDL_Surfaces, which are
/// useless for efficient rendering.

int IndoorsIcons::convert_icons_to_textures(SDL_Renderer* renderer)
{
	int icon_size_offset = 0;
	int icon_size = World::Instance().get_indoors_tile_size();

	return Icons::convert_icons_to_textures(renderer, icon_size, icon_size_offset);
}
