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

#ifndef __ICONS_HH
#define __ICONS_HH

#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "iconprops.hh"

class Icons
{
protected:
	Icons();
	std::vector<IconProps> _icons_props;
	SDL_Surface* _icons_surface;
	std::vector<SDL_Surface*> _icon_surfaces;
	std::vector<SDL_Texture*> _icon_textures;
	int convert_icons_to_textures(SDL_Renderer*, int, int);

public:
	virtual ~Icons();
	void reserve(unsigned);
	SDL_Texture* get_sdl_icon(unsigned);
	unsigned number_of_icons();

	// This function adds properties about an icon to the world's
	// register.  However, if the properties for an icon already exist
	// in that register, they will simply and silently be overwritten.
	// Life's tough!
	void add_props(IconProps);
	// Get properties object for an icon, NULL if none was specified or
	// the icon does not exist.
	IconProps* get_props(int);
};

#endif
