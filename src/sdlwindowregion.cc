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
#include <utility>

#include <SDL2/SDL.h>

#include "util.hh"
#include "charset.hh"
#include "sdlwindow.hh"
#include "sdlwindowregion.hh"

SDLWindowRegion::SDLWindowRegion()
{

	_font = &Charset::Instance();

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

	_texture_last = NULL;
	_texture = NULL;

	// Used for positioning characters inside the region
	_x_frame_offset = 2;
	_y_frame_offset = 4;
	_inter_line_padding = 2;

	_renderer = SDLWindow::Instance().get_renderer();
}

SDLWindowRegion::~SDLWindowRegion()
{
	if (_texture_last != NULL) {
		SDL_DestroyTexture(_texture_last);
		_texture_last = NULL;
	}

	if (_texture != NULL) {
		SDL_DestroyTexture(_texture);
		_texture = NULL;
	}
}

bool SDLWindowRegion::resetRenderer()
{
	return SDLWindow::Instance().resetRenderer();
}

void SDLWindowRegion::set_dimensions(SDL_Rect dimensions)
{
	_dimensions = dimensions;
}

void SDLWindowRegion::set_texture(SDL_Texture* s)
{
	if (_texture != NULL) {
		std::cerr << "ERROR: sdlwindowregion.hh: set_texture failed. Won't set it more than once.\n";
		return;
	}

	_texture = s;

	if (_texture == NULL) {
		std::cerr << "ERROR: sdlwindowregion.hh: set_texture failed. Texture is NULL.\n";
		exit(EXIT_FAILURE);
	}

	int w, h;
	if (SDL_QueryTexture(_texture, NULL, NULL, &w, &h) < 0) {
		std::cerr << "ERROR: sdlwindowregion.hh: set_texture failed: " << IMG_GetError() << "\n";
		exit(EXIT_FAILURE);
	}

	// Now create the second texture of same dimensions...
	if ((_texture_last = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h)) == NULL) {
		std::cerr << "ERROR: sdlwindowregion.hh: cannot create _texture_last: " << IMG_GetError() << "\n";
		exit(EXIT_FAILURE);
	}
	if (SDL_SetRenderTarget(_renderer, _texture_last) < 0) {
		std::cerr << "ERROR: sdlwindowregion.hh: cannot set render target to _texture_last: " << IMG_GetError() << "\n";
		exit(EXIT_FAILURE);
	}
	SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 0);
	SDL_RenderClear(_renderer);
	if (SDL_RenderCopy(_renderer, _texture, NULL, NULL) < 0) {
		std::cerr << "ERROR: sdlwindowregion.hh: cannot copy to _texture_last: " << IMG_GetError() << "\n";
		exit(EXIT_FAILURE);
	}
	resetRenderer();

	std::cout << "INFO: sdlwindowregion.hh: created texture with w: " << w << ", h: " << h << "\n";
}

void SDLWindowRegion::save_texture()
{
	SDL_SetRenderTarget(_renderer, _texture_last);
	SDL_RenderClear(_renderer);
	SDL_RenderCopy(_renderer, _texture, NULL, NULL);
	resetRenderer();
}

/// Put texture s onto the region's own texture.

void SDLWindowRegion::display_texture(std::shared_ptr<SDL_Texture> s)
{
	// Save existing texture for later
	save_texture();

	if (!s) {
		std::cerr << "WARNING: sdlwindowregion.hh: texture s == NULL.\n";
		return;
	}

	int w, h;
	if (SDL_QueryTexture(s.get(), NULL, NULL, &w, &h) < 0) {
		std::cerr << "ERROR: sdlwindowregion.hh: display_texture has invalid texture as argument: " << IMG_GetError() << "\n";
		exit(EXIT_FAILURE);
	}

	if (SDL_SetRenderTarget(_renderer, _texture) < 0) {
		std::cerr << "INFO: sdlwindowregion.hh: set render target failed.\n";
		return;
	}

	if (SDL_RenderCopy(_renderer, s.get(), NULL, NULL) < 0)
		std::cerr << "INFO: sdlwindowregion.hh: render copy failed.\n";

	resetRenderer();

	blit();
}

void SDLWindowRegion::display_last()
{
	SDL_SetRenderTarget(_renderer, _texture);
	SDL_RenderClear(_renderer);
	SDL_RenderCopy(_renderer, _texture_last, NULL, NULL);
	resetRenderer();
	blit();
}

SDL_Texture* SDLWindowRegion::get_texture()
{
	if (_texture == NULL)
		std::cerr << "WARNING: sdlwindowregion.hh: texture == NULL. This is serious. Probably some initialisation problem.\n";
	return _texture;
}

SDL_Rect SDLWindowRegion::get_dimensions()
{
	return _dimensions;
}

SDL_Renderer* SDLWindowRegion::get_renderer()
{
	return _renderer;
}

int SDLWindowRegion::blit()
{
	int result = -1;

	if (SDL_SetRenderTarget(_renderer, SDLWindow::Instance().get_texture_entire_win()) == 0 &&
			SDL_RenderCopy(_renderer, _texture, NULL, &_dimensions) == 0)
	{
		SDL_RenderPresent(_renderer);
		result = 0;
	}
	else
		std::cerr << "WARNING: sdlwindowregion.hh: blit failed: " << IMG_GetError() << "\n";

	return result;
}

void SDLWindowRegion::printch(int x_pos, int y_pos, int c)
{
	_font->printch(get_texture(), c, x_pos + _x_frame_offset, y_pos + _y_frame_offset);
}

void SDLWindowRegion::print_noblit(int x_pos, int y_pos, const std::string s, Alignment align)
{
	for (unsigned i = 0, j = 0; i < s.length(); i++, j++) {
		if (align == LEFTALIGN)
			_font->printch(get_texture(), s[i], x_pos + _x_frame_offset + j * _font->char_width(), y_pos + _y_frame_offset);
		else if (align == RIGHTALIGN)
			print_noblit(_dimensions.w - s.length() * _font->char_width() - _x_frame_offset, y_pos, s);
		else
			print_noblit((int)((_dimensions.w - s.length() * _font->char_width()) / 2), y_pos, s);
	}
}

void SDLWindowRegion::println_noblit(int line, const std::string s, Alignment align)
{
	if (line < _dimensions.h / _font->char_height())
		print_noblit(0, line * _font->char_height() + _inter_line_padding * line, s, align);
}

void SDLWindowRegion::print(int x_pos, int y_pos, const std::string s, Alignment align)
{
	print_noblit(x_pos, y_pos, s, align);
	blit();
}

void SDLWindowRegion::println(int line, const std::string s, Alignment align)
{
	println_noblit(line, s, align);
	blit();
}

void SDLWindowRegion::clear()
{
	if (SDL_SetRenderTarget(_renderer, _texture) < 0) {
		std::cerr << "WARNING: sdlwindowregion.hh: clear(): setting render target failed: " << IMG_GetError() << "\n";
		return;
	}

	if (SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 0) < 0) {
		std::cerr << "WARNING: sdlwindowregion.hh: clear(): setting draw color failed: " << IMG_GetError() << "\n";
		return;
	}

	if (SDL_RenderClear(_renderer) < 0) {
		std::cerr << "WARNING: sdlwindowregion.hh: clear(): render clear failed: " << IMG_GetError() << "\n";
		return;
	}

	resetRenderer();
}

/// It is your own responsibility that filename points to a file which has the exact same dimensions
/// as the _texture.

void SDLWindowRegion::surface_from_file(std::string filename)
{
	SDL_Surface* _tmp_surf = NULL;

	if ((_tmp_surf = IMG_Load(filename.c_str())) == NULL)
		std::cerr << "ERROR: sdlwindowregion.cc: could not load surface: '" << filename << "'.\n";

	SDL_Texture* tmp_txt = SDL_CreateTextureFromSurface(_renderer, _tmp_surf);
	SDL_SetRenderTarget(_renderer, _texture);
	SDL_RenderCopy(_renderer, tmp_txt, NULL, NULL);
	resetRenderer();

	SDL_FreeSurface(_tmp_surf);
	SDL_DestroyTexture(tmp_txt);
}
