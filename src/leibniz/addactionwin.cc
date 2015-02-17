// ----------------------------------------------------------------
// leibniz
//
// Copyright (c) 2009  Andreas Bauer <baueran@gmail.com>
//
// This is not free software.  See COPYING for further information.
// ----------------------------------------------------------------

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
