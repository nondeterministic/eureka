// This source file is part of eureka
//
// Copyright (c) 2007-2018  Andreas Bauer <a@pspace.org>
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

#include <gtkmm.h>
#include <memory>
#include <iostream>

#include "tabcontext.hh"

TabContext::TabContext()
{
	_box = std::make_shared<Gtk::VBox>(new Gtk::VBox());
	_xoffset = 0;
	_yoffset = 0;
	_icon_brush_number = 0;

	std::cout << "INFO: tabcontext: TabContext().\n";
}

//TabContext::TabContext(std::shared_ptr<Gtk::VBox> newbox) : _box(newbox)
//{
//	_xoffset = 0;
//	_yoffset = 0;
//	_icon_brush_number = 0;
//}

TabContext::TabContext(const TabContext& t)
{
	_box = std::make_shared<Gtk::VBox>(t._box.get());
	_xoffset = t._xoffset;
	_yoffset = t._yoffset;
	_icon_brush_number = t._icon_brush_number;

	std::cout << "INFO: tabcontext: TabContext copy constructor.\n";
}

TabContext::~TabContext()
{
	std::cout << "~TabContext()" << std::endl;
}

TabContext& TabContext::operator=(const TabContext& other)
{
	// check for self-assignment
	if (&other == this)
		return *this;

	_box = std::make_shared<Gtk::VBox>(other._box.get());
	_xoffset = other._xoffset;
	_yoffset = other._yoffset;
	_icon_brush_number = other._icon_brush_number;

	std::cout << "INFO: tabcontext: TabContext assignment constructor.\n";

	return *this;
}

std::shared_ptr<Gtk::VBox> TabContext::get_box() const
{
	// return std::make_shared<Gtk::VBox>(new Gtk::VBox());
	return _box;
}

//Gtk::VBox* TabContext::get_box() const
//{
//	return _box;
//}

// This offset is counted in terms of icons, not pixel!

unsigned TabContext::get_xoffset() const
{
  return _xoffset;
}

// This offset is counted in terms of icons, not pixel!

unsigned TabContext::get_yoffset() const
{
  return _yoffset;
}

void TabContext::set_xoffset(unsigned x)
{
  _xoffset = x;
}

void TabContext::set_yoffset(unsigned y)
{
  _yoffset = y;
}

unsigned TabContext::get_icon_brush_no(void) const
{
  return _icon_brush_number;
}

void TabContext::set_icon_brush_no(unsigned number)
{
  _icon_brush_number = number;
}
