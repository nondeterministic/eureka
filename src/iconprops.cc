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

#include <iostream>
#include "iconprops.hh"

IconProps::IconProps()
{
	_flags = 0;
	_radius = 0;
	_icon = 0;
	_next_anim = -1;
	_trans = PropertyStrength::Full;
	_is_walkable = PropertyStrength::Full;
	_poisonous = PropertyStrength::None;
	_magical_force_field = PropertyStrength::None;
	_name = "nothing special";
	_seffect = "";
	_default_lua_name = "";
}

IconProps::~IconProps()
{
}

void IconProps::set_icon(unsigned no)
{
	_icon = no;
}

unsigned IconProps::get_icon(void)
{
	return _icon;
}

int IconProps::light_radius()
{
	return _radius;
}

void IconProps::set_light_radius(int r)
{
	_radius = r;
}

std::string IconProps::get_name(void)
{
	return _name;
}

void IconProps::set_name(const char* newname)
{
    _name = newname;
}

void IconProps::set_next_anim(int next)
{
    _next_anim = next;
}

int IconProps::next_anim()
{
    return _next_anim;
}

std::string IconProps::sound_effect()
{
    return _seffect;
}

void IconProps::set_sound_effect(std::string se)
{
    _seffect = se;
}

std::string IconProps::default_lua_name()
{
	return _default_lua_name;
}

void IconProps::set_default_lua_name(std::string newName)
{
	_default_lua_name = newName;
}
