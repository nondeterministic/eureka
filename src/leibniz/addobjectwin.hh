// ----------------------------------------------------------------
// leibniz
//
// Copyright (c) 2009  Andreas Bauer <baueran@gmail.com>
//
// This is not free software.  See COPYING for further information.
// ----------------------------------------------------------------

#ifndef __ADDOBJWIN_HH
#define __ADDOBJWIN_HH

#include <gtkmm.h>
#include <string>
#include "../mapobj.hh"

class AddObjectWin : public Gtk::Dialog
{
public:
  AddObjectWin(Glib::RefPtr<Gdk::Pixbuf>&, int);
  virtual ~AddObjectWin();
  MAPOBJ_TYPES get_object_type();
  int get_icon_no();
  bool removable;
  std::string id;

protected:
  Gtk::Image* _icon;
  Gtk::Button* bok;
  Gtk::Button* bcancel;
  Gtk::Alignment align;
  Gtk::HBox hbox, hbox_id;
  Gtk::VBox vbox;
  Gtk::Label id_lbl, object_lbl, icon_lbl;
  Gtk::ComboBoxText objects_combo;
  Gtk::Entry id_entry;
  Gtk::CheckButton cb_removable;

  void on_button_cancel(void);
  void on_button_ok(void);

  int _object_no;
  unsigned _icon_no;
};

#endif
