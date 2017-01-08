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
  bool has_action();
  int get_icon_no();
  std::string default_lua_name;
  // bool removable;
  std::string id;

protected:
  Gtk::Image* _icon;
  Gtk::Button* bok;
  Gtk::Button* bcancel;
  Gtk::Alignment align;
  Gtk::HBox hbox, hbox_id, hbox_actions, hbox_lua_name;
  Gtk::VBox vbox, rb_type_vbox;
  Gtk::Label id_lbl, object_lbl, icon_lbl, action_lbl, lua_name_lbl;
  Gtk::ComboBoxText objects_combo;
  Gtk::ComboBoxText actions_combo;
  Gtk::Entry id_entry;
  Gtk::Entry lua_name_entry;
  // Gtk::CheckButton cb_removable;

  Gtk::Frame rb_type_frame;
  Gtk::RadioButton rb_type_item, rb_type_person, rb_type_monster, rb_type_animal;
  void handle_rb_type_item_toggled();
  void handle_rb_type_person_toggled();
  void handle_rb_type_monster_toggled();
  void handle_rb_type_animal_toggled();

  void on_button_cancel(void);
  void on_button_ok(void);

  int _object_no, _action_no;
  bool _has_action;
  unsigned _icon_no;
};

#endif
