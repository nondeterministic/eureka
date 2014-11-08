// ----------------------------------------------------------------
// leibniz
//
// Copyright (c) 2009  Andreas Bauer <baueran@gmail.com>
//
// This is not free software.  See COPYING for further information.
// ----------------------------------------------------------------

#ifndef __EXPANDMAPWIN_HH
#define __EXPANDMAPWIN_HH

#include <gtkmm.h>

class ExpandMapWin : public Gtk::Dialog
{
public:
  ExpandMapWin(const char*, const char*);
  virtual ~ExpandMapWin();
  void get_values(int&, int&, int&, int&);

protected:
  int _top, _bot, _right, _left;
  Gtk::Button* bok;
  Gtk::Button* bcancel;
  Gtk::Alignment align;
  Gtk::VBox vbox;
  Gtk::HBox hbox_top, hbox_bot, hbox_right, hbox_left;
  Gtk::Label top_lbl, bot_lbl, right_lbl, left_lbl, title_lbl;
  Gtk::Entry top_entry, bot_entry, right_entry, left_entry;

  void on_button_cancel(void);
  void on_button_ok(void);
};

#endif
