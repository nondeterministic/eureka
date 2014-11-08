// ----------------------------------------------------------------
// leibniz
//
// Copyright (c) 2009  Andreas Bauer <baueran@gmail.com>
//
// This is not free software.  See COPYING for further information.
// ----------------------------------------------------------------

#include <sstream>
#include <string>
#include <iostream>
#include "iconpropswin.hh"
#include "../iconprops.hh" 

IconPropsWin::IconPropsWin(Glib::RefPtr<Gdk::Pixbuf>& icon) : name_lbl("Name:"), icon_lbl("undef icon no.")
{
  _name = "undef";
  _transparency = 0;
  _icon_no = 0;

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

  trans_combo.append("fully transparent");
  trans_combo.append("semi-transparent");
  trans_combo.append("not transparent");

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

  switch (trans_combo.get_active_row_number()) {
  case 0:
    _transparency = FULLY_TRANS;
    break;
  case 1:
    _transparency = SEMI_TRANS;
    break;
  case 2:
    _transparency = NOT_TRANS;
    break;
  default:
    _transparency = FULLY_TRANS;
  }
  
  hide();
}

std::string IconPropsWin::get_name(void)
{
    return _name;
}

void IconPropsWin::set_name(const char* n)
{
  name_entry.set_text(n);
  _name = n;
}

void IconPropsWin::set_transparency(int t)
{
  _transparency = t;

  if (t & SEMI_TRANS)
    trans_combo.set_active(1);
  else if (t & NOT_TRANS)
    trans_combo.set_active(2);
  else {
    trans_combo.set_active(0);
    _transparency = FULLY_TRANS;
  }
}

int IconPropsWin::get_transparency(void)
{
  if (_transparency & SEMI_TRANS)
    return SEMI_TRANS;
  else if (_transparency & NOT_TRANS)
    return NOT_TRANS;
  else
    return FULLY_TRANS;
}

void IconPropsWin::set_icon_no(unsigned t)
{
  std::stringstream tmp;
  tmp << t;
  icon_lbl.set_text(tmp.str());
  _icon_no = t;
}

unsigned IconPropsWin::get_icon_no(void)
{
  return _icon_no;
}
