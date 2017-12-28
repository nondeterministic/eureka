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

Creature::Creature(const GameCharacter& gc)
{
	_rh_item = gc._rh_item;
	_lh_item = gc._lh_item;
	_armour = gc._armour;
	_head_armour = gc._head_armour;
	_feet_armour = gc._feet_armour;
	_hands_armour = gc._hands_armour;
	_condition = gc._condition;
	_att = gc._att;
	_end = gc._end;
	_char = gc._char;
	_dxt = gc._dxt;
	_sp_max = gc._sp_max;
	_sp = gc._sp;
	_hp = gc._hp;
	_hp_max = gc._hp_max;
	_sex = gc._sex;
	_iq = gc._iq;
	_wis = gc._wis;
	_luck = gc._luck;
	_str = gc._str;
	_race = gc._race;
	_gold = gc._gold;

	_name = gc._name;
	_plural_name = gc._plural_name;

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

Creature::Creature(const Creature& gc)
{
	_rh_item = gc._rh_item;
	_lh_item = gc._lh_item;
	_armour = gc._armour;
	_head_armour = gc._head_armour;
	_feet_armour = gc._feet_armour;
	_hands_armour = gc._hands_armour;
	_condition = gc._condition;
	_att = gc._att;
	_end = gc._end;
	_char = gc._char;
	_dxt = gc._dxt;
	_sp_max = gc._sp_max;
	_sp = gc._sp;
	_hp = gc._hp;
	_hp_max = gc._hp_max;
	_sex = gc._sex;
	_iq = gc._iq;
	_wis = gc._wis;
	_luck = gc._luck;
	_str = gc._str;
	_race = gc._race;
	_gold = gc._gold;

	_name = gc._name;
	_plural_name = gc._plural_name;

	_image_surf = gc._image_surf;
	_image = gc._image;
	_distance = gc._distance;
	_max_group = gc._max_group;
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
