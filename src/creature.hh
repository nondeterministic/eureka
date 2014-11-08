//
//
// Copyright (c) 2005  Andreas Bauer <baueran@gmail.com>
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

#ifndef __CREATURE_HH
#define __CREATURE_HH

#include "gamecharacter.hh"
#include <string>
#include <SDL.h>

class Creature : public GameCharacter
{
private:
  int _max_group;
  int _distance;
  std::string _image;
  SDL_Surface* _image_surf;

public:
  Creature();
  ~Creature();
  int distance();
  void set_distance(int);
  void set_max_group(int);
  int max_group();
  void set_img(const char*);
  void set_img(std::string);
  std::string img();
  SDL_Surface* img_surf();
  SDL_Surface* load_img();
};

#endif
