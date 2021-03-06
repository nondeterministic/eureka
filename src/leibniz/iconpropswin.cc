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

#include <sstream>
#include <string>
#include <iostream>
#include <type_traits> // for underlying_type

#include "iconpropswin.hh"
#include "../iconprops.hh" 
#include "../indoorsicons.hh"

IconPropsWin::IconPropsWin(Glib::RefPtr<Gdk::Pixbuf> icon) : name_lbl("Name:"), icon_lbl("undef icon no."), default_lua_name_lbl("default_lua_name:")
{
  _name = "undef";
  _icon_no = 0;
  _default_lua_name = "";

  set_title("Edit icon properties");
  show_all_children();
  set_modal(true);

  align.set_padding(10, 10, 10, 10);
  get_vbox()->add(align);

  _icon = new Gtk::Image(icon);
  vbox.add(*_icon);
  vbox.add(icon_lbl);
  
  hbox_name.set_spacing(10);
  hbox_name.add(name_lbl);
  hbox_name.pack_start(name_entry, Gtk::PACK_SHRINK);
  vbox.add(hbox_name);

  hbox_default_lua_name.set_spacing(10);
  hbox_default_lua_name.add(default_lua_name_lbl);
  hbox_default_lua_name.pack_start(default_lua_name_entry, Gtk::PACK_SHRINK);
  vbox.add(hbox_default_lua_name);

  walk_combo.append("full speed");
  walk_combo.append("slow");
  walk_combo.append("unpassable");

  trans_combo.append("fully transparent");
  trans_combo.append("semi-transparent");
  trans_combo.append("not transparent");

  vbox.add(walk_combo);
  vbox.add(trans_combo);

  align.add(vbox);
  
  bcancel = manage(add_button(Gtk::Stock::CANCEL, 0));
  bok = manage(add_button(Gtk::Stock::OK, 1));

  bcancel->signal_clicked().connect(sigc::mem_fun(*this, &IconPropsWin::on_button_cancel));
  bok->signal_clicked().connect(sigc::mem_fun(*this, &IconPropsWin::on_button_ok));

  vbox.set_spacing(10);

  bok->set_can_default();
  bok->grab_default();

  show_all_children();
}

IconPropsWin::~IconPropsWin(void)
{
  delete _icon;
}

void IconPropsWin::on_button_cancel(void)
{
  hide();
}

void IconPropsWin::on_button_ok(void)
{
  _name = name_entry.get_text();
  _default_lua_name = default_lua_name_entry.get_text();

  switch (trans_combo.get_active_row_number()) {
  case 0:
	  _iconProps._trans = PropertyStrength::Full;
	  break;
  case 1:
	  _iconProps._trans = PropertyStrength::Some;
	  break;
  case 2:
	  _iconProps._trans = PropertyStrength::None;
	  break;
  }

  switch (walk_combo.get_active_row_number()) {
  case 0:
	  _iconProps._is_walkable = PropertyStrength::Full;
	  break;
  case 1:
	  _iconProps._is_walkable = PropertyStrength::Some;
	  break;
  case 2:
	  _iconProps._is_walkable = PropertyStrength::None;
	  break;
  }

  hide();
}

std::string IconPropsWin::get_default_lua_name()
{
	return _default_lua_name;
}

void IconPropsWin::set_default_lua_name(std::string newName)
{
	_default_lua_name = newName;
	default_lua_name_entry.set_text(_default_lua_name);
}

void IconPropsWin::set_trans(PropertyStrength it)
{
	std::cout << "Setting trans to: " << static_cast<utype>(it) << "\n";

	_iconProps._trans = it;
	trans_combo.set_active(static_cast<utype>(it));
	queue_draw();
}

PropertyStrength IconPropsWin::get_trans()
{
	return _iconProps._trans;
}

void IconPropsWin::set_walkable(PropertyStrength iw)
{
	std::cout << "Setting walkable to: " << static_cast<utype>(iw) << "\n";

	_iconProps._is_walkable = iw;
	walk_combo.set_active(static_cast<utype>(iw));
	queue_draw();
}

PropertyStrength IconPropsWin::get_walkable()
{
	return _iconProps._is_walkable;
}

std::string IconPropsWin::get_name(void)
{
    return _name;
}

void IconPropsWin::set_name(const char* n)
{
	name_entry.set_text(n);
	_name = n;

	queue_draw();
}

void IconPropsWin::set_icon_no(unsigned t)
{
  std::stringstream tmp;
  tmp << t;
  icon_lbl.set_text(tmp.str());
  _icon_no = t;

  queue_draw();
}

unsigned IconPropsWin::get_icon_no(void)
{
  return _icon_no;
}
