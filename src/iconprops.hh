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

#ifndef __ICONPROPS_HH
#define __ICONPROPS_HH

#include <string>
#include <type_traits>

enum class PropertyStrength: int
{
	None = 2,
	Some = 1,
	Full = 0
};

// cf. http://stackoverflow.com/questions/14589417/can-an-enum-class-be-converted-to-the-underlying-type
typedef std::underlying_type<PropertyStrength>::type utype;

class IconProps
{
public:
  IconProps();
  ~IconProps();
  void set_icon(unsigned);
  unsigned get_icon();
  void set_name(const char*);
  std::string get_name();
  void set_next_anim(int);
  int next_anim();
  std::string sound_effect();
  void set_sound_effect(std::string);
  void set_light_radius(int);
  int light_radius();
  bool is_walkable();
  std::string default_lua_name();
  void set_default_lua_name(std::string);

  PropertyStrength _trans;
  PropertyStrength _is_walkable;
  PropertyStrength _poisonous;
  PropertyStrength _magical_force_field;
  
protected:
  unsigned _icon;
  std::string _name;
  std::string _default_lua_name;
  int _flags;
  int _next_anim;
  int _radius;
  std::string _seffect;
};

#endif
