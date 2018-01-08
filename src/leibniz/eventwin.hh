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
