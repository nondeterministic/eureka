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

#ifndef TABCONTEXT_HH
#define TABCONTEXT_HH

#include <gtkmm.h>
#include <memory>

class TabContext
{
public:
	TabContext();
	// TabContext(std::shared_ptr<Gtk::VBox>);
	TabContext(const TabContext&);
	~TabContext();
	TabContext& operator=(const TabContext&);

	std::shared_ptr<Gtk::VBox> get_box() const;
	// Gtk::VBox* get_box() const;
	unsigned get_xoffset() const;
	unsigned get_yoffset() const;
	void set_xoffset(unsigned);
	void set_yoffset(unsigned);
	unsigned get_icon_brush_no() const;
	void set_icon_brush_no(unsigned);

protected:
	std::shared_ptr<Gtk::VBox> _box;
	// This offset is counted in terms of icons, not pixel!  This is
	// necessary to not get confused when the user disables the grid in
	// between context switches.  Later we add a grid pixel respectively
	// to all icons if necessary.
	int _xoffset, _yoffset;
	int _icon_brush_number;
};

#endif
