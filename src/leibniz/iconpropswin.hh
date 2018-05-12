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

#ifndef __ICONPROPSWIN_HH
#define __ICONPROPSWIN_HH

#include <gtkmm.h>
#include <string>
#include "../iconprops.hh"

class IconPropsWin : public Gtk::Dialog
{
public:
  IconPropsWin(Glib::RefPtr<Gdk::Pixbuf>);
  virtual ~IconPropsWin();
  std::string get_name();
  void set_name(const char*);

  void set_walkable(PropertyStrength);
  PropertyStrength get_walkable();

  void set_trans(PropertyStrength);
  PropertyStrength get_trans();

  void set_icon_no(unsigned);
  unsigned get_icon_no();

  void set_default_lua_name(std::string);
  std::string get_default_lua_name();

protected:
  Gtk::Image* _icon;
  Gtk::Button* bok;
  Gtk::Button* bcancel;
  Gtk::Alignment align;
  Gtk::VBox vbox;
  Gtk::HBox hbox_name, hbox_default_lua_name;
  Gtk::Label name_lbl, icon_lbl, default_lua_name_lbl;
  Gtk::Entry name_entry, default_lua_name_entry;
  Gtk::ComboBoxText trans_combo, walk_combo;

  void on_button_cancel(void);
  void on_button_ok(void);

  std::string _name, _default_lua_name;
  IconProps _iconProps;
  unsigned _icon_no;
};

#endif
