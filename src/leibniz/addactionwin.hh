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

#ifndef __ADDACTIONWIN_HH
#define __ADDACTIONWIN_HH

#include <gtkmm.h>

class AddActionWin : public Gtk::Dialog
{
public:
  AddActionWin();
  virtual ~AddActionWin();
  int get_action() const;

protected:
  Gtk::Button* bok;
  Gtk::Button* bcancel;
  Gtk::Alignment align;
  Gtk::HBox hbox;
  Gtk::VBox vbox;
  Gtk::Label action_lbl;
  Gtk::ComboBoxText actions_combo;

  void on_button_cancel(void);
  void on_button_ok(void);

  int _action_no;
};

#endif
