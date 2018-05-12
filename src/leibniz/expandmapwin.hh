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

#ifndef __EXPANDMAPWIN_HH
#define __EXPANDMAPWIN_HH

#include <gtkmm.h>

class ExpandMapWin : public Gtk::Dialog
{
public:
  ExpandMapWin(const char*, const char*);
  virtual ~ExpandMapWin();
  void get_values(int&, int&, int&, int&);

protected:
  int _top, _bot, _right, _left;
  Gtk::Button* bok;
  Gtk::Button* bcancel;
  Gtk::Alignment align;
  Gtk::VBox vbox;
  Gtk::HBox hbox_top, hbox_bot, hbox_right, hbox_left;
  Gtk::Label top_lbl, bot_lbl, right_lbl, left_lbl, title_lbl;
  Gtk::Entry top_entry, bot_entry, right_entry, left_entry;

  void on_button_cancel(void);
  void on_button_ok(void);
};

#endif
