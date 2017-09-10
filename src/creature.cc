// This source file is part of eureka
//
// Copyright (c) 2007-2016  Andreas Bauer <baueran@gmail.com>
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

#include "creature.hh"
#include <string>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

Creature::Creature()
{
  _image_surf = NULL;
  _image = "";
  _distance = 0;
  _max_group = 0;
}

Creature::~Creature()
{
  if (_image_surf != NULL) {
    SDL_FreeSurface(_image_surf);
    _image_surf = NULL;
  }
}

Creature::Creature(const Creature& c)
{
	_image_surf = c._image_surf;
	_image = c._image;
	_distance = c._distance;
	_max_group = c._max_group;
	std::cout << "CREATURE DEEP COPY MOTHERFUCK!\n";
}

int Creature::distance()
{
  return _distance;
}

void Creature::set_distance(int d)
{
  _distance = d;
}

int Creature::max_group()
{
  return _max_group;
}

void Creature::set_max_group(int mg)
{
  _max_group = mg;
}

void Creature::set_img(const char* img)
{
  _image = img;
}

void Creature::set_img(std::string img)
{
  set_img(img.c_str());
}

std::string Creature::img()
{
  return _image;
}

SDL_Surface* Creature::load_img()
{
	if (_image_surf != NULL)
		SDL_FreeSurface(_image_surf);

	if ((_image_surf = IMG_Load(_image.c_str())) == NULL)
		cerr << "ERROR: creature.cc: _image_surf == NULL after attempting to load '" << _image << "'.\n";

	return _image_surf;
}

// SDL_Surface* Creature::img_surf()
// {
//   if (_image_surf)
//     return _image_surf;
//   else if (_image.length() > 0)
//     set_img(_image.c_str());
//   else {
//     std::cerr << "SERIOUS: creature img_surf returns null\n";
//     return NULL;
//   }
// }
