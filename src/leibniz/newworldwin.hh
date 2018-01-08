// This source file is part of eureka
//
// Copyright (c) 2007-2018  Andreas Bauer <baueran@gmail.com>
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

#ifndef NEWWORLDWIN_HH
#define NEWWORLDWIN_HH

#include <gtkmm.h>

class NewWorldWin : public Gtk::Dialog
{
public:
  NewWorldWin();
  virtual ~NewWorldWin();

protected:
  Gtk::Button* bok;
  Gtk::Button* bcancel;
  Gtk::Alignment align;
  Gtk::VBox vbox;
  Gtk::HBox hbox_e, hbox_path;
  Gtk::Label name_lbl, path_lbl;
  Gtk::Entry name_entry, path_entry;

  void on_button_cancel(void);
  void on_button_ok(void);
  bool my_key_release_event(GdkEventKey* event);
};

#endif
