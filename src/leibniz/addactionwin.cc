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

#include <gtkmm.h>
#include "../action.hh"
#include "addactionwin.hh"

AddActionWin::AddActionWin() : action_lbl("Select type of action:")
{
  _action_no = 0;

  set_title("Add an action to the icon");
  set_modal(true);

  align.set_padding(10, 10, 10, 10);
  get_vbox()->add(align);

  actions_combo.append("ACT_ON_ENTER");
  actions_combo.append("ACT_ON_REACH");
  actions_combo.append("ACT_ON_PULLPUSH");
  actions_combo.append("ACT_ON_SEARCH");

  hbox.set_spacing(10);
  hbox.add(action_lbl);
  hbox.pack_start(actions_combo, Gtk::PACK_SHRINK);
  vbox.add(hbox);

  align.add(vbox);
  
  bcancel = manage(add_button(Gtk::Stock::CANCEL, 0));
  bok = manage(add_button(Gtk::Stock::OK, 1));

  bcancel->signal_clicked().connect(sigc::mem_fun(*this, &AddActionWin::on_button_cancel));
  bok->signal_clicked().connect(sigc::mem_fun(*this, &AddActionWin::on_button_ok));

  vbox.set_spacing(10);

  bok->set_can_default();
  bok->grab_default();

  actions_combo.set_active(_action_no);

  show_all_children();
}

AddActionWin::~AddActionWin(void)
{
}

void AddActionWin::on_button_cancel(void)
{
  hide();
}

void AddActionWin::on_button_ok(void)
{
  _action_no = actions_combo.get_active_row_number();
  hide();
}

int AddActionWin::get_action(void) const
{
	switch (_action_no) {
	case 0:
		return ACT_ON_ENTER;
	case 1:
		return ACT_ON_REACH;
	case 2:
		return ACT_ON_PULLPUSH;
	default:
		return ACT_ON_SEARCH;
	}
}
