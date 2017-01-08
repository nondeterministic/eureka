// ----------------------------------------------------------------
// leibniz
//
// Copyright (c) 2009  Andreas Bauer <baueran@gmail.com>
//
// This is not free software.  See COPYING for further information.
// ----------------------------------------------------------------

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

  void set_walkable(ICON_WALK);
  ICON_WALK get_walkable();

  void set_trans(ICON_TRANS);
  ICON_TRANS get_trans();

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
