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

#ifndef NEWMAPWIN_HH
#define NEWMAPWIN_HH

#include <gtkmm.h>

class NewMapWin : public Gtk::Window
{
public:
  NewMapWin();
  virtual ~NewMapWin();

  Glib::ustring get_name_entry(void);
  bool is_outdoors();
  bool is_indoors();

protected:
  Gtk::Button bok, bcancel;
  Gtk::HSeparator sep;
  Gtk::Alignment align;
  Gtk::VBox vbox, rbvbox, dimensionsvbox;
  Gtk::HBox hbox_e, hbox_b, hbox_b_space, hbox_x, hbox_y;
  Gtk::Label name_lbl, x_lbl, y_lbl;
  Gtk::Entry name_entry, x_entry, y_entry;
  Gtk::Frame rbframe, dimensionsframe;
  Gtk::RadioButton rb_wilderness, rb_indoors;

  void on_button_cancel(void);
  void on_button_ok(void);
  bool my_key_release_event(GdkEventKey* event);
};

#endif
