// ----------------------------------------------------------------
// leibniz
//
// Copyright (c) 2009  Andreas Bauer <baueran@gmail.com>
//
// This is not free software.  See COPYING for further information.
// ----------------------------------------------------------------

#include <gtkmm.h>
#include <sstream>

#include "../mapobj.hh"
#include "addobjectwin.hh"

AddObjectWin::AddObjectWin(Glib::RefPtr<Gdk::Pixbuf>& icon, int icon_no) 
  : id_lbl("ID:"), object_lbl("Select type of object:"), icon_lbl("undef icon no."),
    cb_removable("removable?")
{
  removable = false;
  _object_no = 0;
  _icon_no = icon_no;

  set_title("Add an object to the icon");
  show_all_children();
  set_modal(true);

  align.set_padding(10, 10, 10, 10);
  get_vbox()->add(align);

  _icon = new Gtk::Image(icon);
  vbox.add(*_icon);
  std::stringstream ss;
  ss << icon_no;
  icon_lbl.set_text(ss.str());
  vbox.add(icon_lbl);

  hbox_id.set_spacing(10);
  hbox_id.add(id_lbl);
  hbox_id.pack_start(id_entry, Gtk::PACK_SHRINK);
  vbox.add(hbox_id);

  hbox.set_spacing(10);
  hbox.add(object_lbl);
  hbox.pack_start(objects_combo, Gtk::PACK_SHRINK);
  vbox.add(hbox);

  objects_combo.append("Monster");
  objects_combo.append("Item");

  vbox.add(cb_removable);

  align.add(vbox);
  
  bcancel = manage(add_button(Gtk::Stock::CANCEL, 0));
  bok = manage(add_button(Gtk::Stock::OK, 1));

  bcancel->signal_clicked().connect(sigc::mem_fun(*this, &AddObjectWin::on_button_cancel));
  bok->signal_clicked().connect(sigc::mem_fun(*this, &AddObjectWin::on_button_ok));

  vbox.set_spacing(10);

  bok->set_can_default();
  bok->grab_default();

  objects_combo.set_active(_object_no);

  show_all_children();
}

AddObjectWin::~AddObjectWin(void)
{
}

void AddObjectWin::on_button_cancel(void)
{
  hide();
}

void AddObjectWin::on_button_ok(void)
{
  _object_no = objects_combo.get_active_row_number();
  id = id_entry.get_text();
  removable = cb_removable.get_active();

  hide();
}

MAPOBJ_TYPES AddObjectWin::get_object_type(void)
{
  switch (_object_no) {
  case 0:
    return MAPOBJ_MONSTER;
  default:
    return MAPOBJ_ITEM;
  }
}

int AddObjectWin::get_icon_no()
{
  return _icon_no;
}
