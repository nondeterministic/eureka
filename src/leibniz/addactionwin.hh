// ----------------------------------------------------------------
// leibniz
//
// Copyright (c) 2009  Andreas Bauer <baueran@gmail.com>
//
// This is not free software.  See COPYING for further information.
// ----------------------------------------------------------------

#ifndef __ADDACTIONWIN_HH
#define __ADDACTIONWIN_HH

#include <gtkmm.h>

class AddActionWin : public Gtk::Dialog
{
public:
  AddActionWin();
  virtual ~AddActionWin();
  int get_action() const;

protected:
  Gtk::Button* bok;
  Gtk::Button* bcancel;
  Gtk::Alignment align;
  Gtk::HBox hbox;
  Gtk::VBox vbox;
  Gtk::Label action_lbl;
  Gtk::ComboBoxText actions_combo;

  void on_button_cancel(void);
  void on_button_ok(void);

  int _action_no;
};

#endif
