// ----------------------------------------------------------------
// leibniz
//
// Copyright (c) 2009  Andreas Bauer <baueran@gmail.com>
//
// This is not free software.  See COPYING for further information.
// ----------------------------------------------------------------

#ifndef __EVENTWIN_HH
#define __EVENTWIN_HH

#include <gtkmm.h>
#include <string>

class NoEventEntry 
{
protected:
  std::string err;

public:
  NoEventEntry(std::string s) { err = s; }
  std::string print() const { return err; }
};

class EventWin : public Gtk::Dialog
{
public:
  EventWin();
  virtual ~EventWin();
  
  int get_type();

  unsigned get_x();
  unsigned get_y();
  std::string get_city();

protected:
  Gtk::Alignment align;
  Gtk::Button* bok;
  Gtk::Button* bcancel;
  Gtk::VBox vbox, *modular_vbox;
  Gtk::ComboBoxText events_combo;
  Gtk::HBox events_hbox;
  Gtk::Label events_lbl;

  // Variable part
  Gtk::Entry* x_entry;
  Gtk::Entry* y_entry;
  Gtk::Label* x_lbl;
  Gtk::Label* y_lbl;
  Gtk::HBox* x_hbox;
  Gtk::HBox* y_hbox;

  Gtk::Entry* city_entry;
  Gtk::Label* city_lbl;
  Gtk::HBox* city_hbox;

  void on_button_cancel();
  void on_button_ok();
  void on_combo_changed();

  void remove_all_ui();
  void add_event_enter_map_ui();
};

#endif
