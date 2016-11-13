// ----------------------------------------------------------------
// leibniz
//
// Copyright (c) 2009  Andreas Bauer <baueran@gmail.com>
//
// This is not free software.  See COPYING for further information.
// ----------------------------------------------------------------

#include <iostream>
#include "iconprops.hh"

IconProps::IconProps()
{
	_radius = 0;
	_icon = 0;
	_next_anim = -1;
	_trans = IT_FULLY;
	_is_walkable = IW_FULL;
	_name = "nothing special";
	_seffect = "";
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
