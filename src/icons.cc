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

#include <vector>
#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "icons.hh"
#include "iconprops.hh"
#include "world.hh"

Icons::Icons()
{
	// std::cout << "Icons()\n";
	_icons_surface = NULL;
}

Icons::~Icons()
{
	// std::cout << "~Icons()\n";

	SDL_FreeSurface(_icons_surface);

	for (SDL_Texture* texture: _icon_textures)
		SDL_DestroyTexture(texture);
	_icon_textures.clear();

	for (SDL_Surface* surface: _icon_surfaces)
		SDL_FreeSurface(surface);
	_icon_surfaces.clear();

	_icons_props.clear();
}

void Icons::reserve(unsigned size)
{
	IconProps iconprops;
	_icons_props.assign(size, iconprops);
	std::cout << "INFO: icons.cc: Assigned/reserved how many icons props? " << _icons_props.size() << ".\n";
}

void Icons::add_props(IconProps new_props)
{
	if (new_props.get_icon() < _icons_props.size())
		_icons_props[new_props.get_icon()] = new_props;
	else
		std::cerr << "WARNING: icons.cc: adding IconProps for icon " << new_props.get_icon() << " failed: only " << _icons_props.size() << " icons loaded.\n";
}

IconProps* Icons::get_props(int icon_no)
{
	try {
		if (icon_no >= 0 && icon_no < (int)_icons_props.size()) {
			IconProps* props = &_icons_props.at(icon_no);

			if (props == NULL) {
				std::cerr << "icons.cc: get_props: props == NULL.\n";
				return NULL;
			}
			else
				return props;
		}
		else {
			// std::cerr << "WARNING: icons.cc: get_props(" << icon_no << ") yields no sensible result (_icons_props.size: " << _icons_props.size() << ").\n";
			return NULL;
		}
	}
	catch (...) {
		std::cerr << "WARNING: icons.cc: Exception caught in get_props(" << icon_no << ") as it yields no sensible result.\n";
		return NULL;
	}
}

unsigned Icons::number_of_icons(void)
{
	return _icon_textures.size();
}

SDL_Texture* Icons::get_sdl_icon(unsigned no)
{
	SDL_Texture* result = NULL;

	if (no < _icon_textures.size()) {
		result = _icon_textures[no];

		int w, h;
		if (SDL_QueryTexture(result, NULL, NULL, &w, &h) < 0) {
			std::cerr << "WARNING: icons.cc: get_sdl_icon() result for " << no << " with w: " << w << ", h: " << h << " invalid: " << IMG_GetError() << "\n";
			return NULL;
		}
	}
	else
		std::cerr << "ERROR: icons.cc: get_sdl_icon() called with parameter that is out-of-bounds: " << no << ". (Did you call initialise()?)\n";

	return result;
}

/// Basically, CONVERT the icons' surfaces to textures, once we have a renderer.
/// Because so far, the icons were only loaded as old-school SDL_Surfaces, which are
/// useless for efficient rendering.

int Icons::convert_icons_to_textures(SDL_Renderer* renderer, int icon_size, int icon_size_offset)
{
	// If initialised more than once, we need to clean up in between first...
	if (_icon_textures.size() > 0) {
		for (SDL_Texture* texture: _icon_textures)
			SDL_DestroyTexture(texture);
		_icon_textures.clear();
	}

	SDL_Texture* tmp_all_icons_texture = SDL_CreateTextureFromSurface(renderer, _icons_surface);

	if (SDL_SetTextureBlendMode(tmp_all_icons_texture, SDL_BLENDMODE_BLEND) < 0) {
		std::cerr << "ERROR: icons.cc: setting blend mode not working: " << IMG_GetError() << "\n";
		return -1;
	}

	int icons_texture_w, icons_texture_h;
	if (SDL_QueryTexture(tmp_all_icons_texture, NULL, NULL, &icons_texture_w, &icons_texture_h) < 0) {
		std::cerr << "ERROR: icons.cc: icons_texture invalid: " << IMG_GetError() << "\n";
		return -1;
	}

	// Fill icon vector
	for (int y = 0; y < icons_texture_h; y += icon_size) {
		for (int x = 0; x < icons_texture_w - icon_size_offset; x += icon_size) {
			SDL_Rect srcRect;
			srcRect.x = x;
			srcRect.y = y;
			srcRect.w = icon_size;
			srcRect.h = icon_size;

			SDL_Texture* single_icon_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
																 icon_size, icon_size);

			if (SDL_SetTextureBlendMode(single_icon_texture, SDL_BLENDMODE_BLEND) < 0) {
				std::cerr << "ERROR: icons.cc: setting blend mode not working: " << IMG_GetError() << "\n";
				continue;
			}

			if (SDL_SetRenderTarget(renderer, single_icon_texture) < 0) {
				std::cerr << "ERROR: icons.cc: setting rendering target not working: " << IMG_GetError() << "\n";
				exit(-1);
			}

			// see also https://forums.libsdl.org/viewtopic.php?p=40949
			SDL_SetRenderDrawColor(renderer, 0,0,0,0);
			SDL_RenderClear(renderer);

			if (SDL_RenderCopy(renderer, tmp_all_icons_texture, &srcRect, NULL) < 0) {
				std::cerr << "WARNING: icons.cc: render copy failed: " << IMG_GetError() << "\n";
				continue;
			}

			if (single_icon_texture == NULL) {
				std::cerr << "WARNING: icons.cc: tmp_icon_texture == NULL\n";
				continue;
			}

			int w, h;
			if (SDL_QueryTexture(single_icon_texture, NULL, NULL, &w, &h) < 0) {
				std::cerr << "WARNING: icons.cc: tmp_icon_texture invalid: " << IMG_GetError() << "\n";
				continue;
			}

			_icon_textures.push_back(single_icon_texture);
		}
	}

	SDL_SetRenderTarget(renderer, NULL);
	SDL_DestroyTexture(tmp_all_icons_texture);

	std::cout << "INFO: icons.cc: Converted and stored " << _icon_textures.size() << " icons.\n";

	return 0;
}
