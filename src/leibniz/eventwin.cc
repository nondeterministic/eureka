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

#include <string>
#include <iostream>
#include "../gameevent.hh"
#include "eventwin.hh"

EventWin::EventWin()
{
  set_title("Add an event to the existing action");
  set_modal(true);

  align.set_padding(10, 10, 10, 10);
  get_vbox()->add(align);

  x_entry = NULL;
  y_entry = NULL;
  x_lbl = NULL;
  y_lbl = NULL;
  x_hbox = NULL;
  y_hbox = NULL;
  city_entry = NULL;
  city_lbl = NULL;
  city_hbox = NULL;

  events_combo.append("EVENT_ENTER_MAP");
  events_combo.append("EVENT_CHANGE_MUSIC");
  events_combo.append("EVENT_ATTACK_PARTY");
  events_lbl.set_text("Event type:");
  events_hbox.set_spacing(10);
  events_hbox.add(events_lbl);
  events_hbox.pack_start(events_combo, Gtk::PACK_SHRINK);
  events_combo.set_active(0);
  vbox.add(events_hbox);
  events_combo.signal_changed().
    connect(sigc::mem_fun(*this, &EventWin::on_combo_changed));

  align.add(vbox);
  modular_vbox = new Gtk::VBox();
  vbox.add(*modular_vbox);

  bcancel = manage(add_button(Gtk::Stock::CANCEL, 0));
  bok = manage(add_button(Gtk::Stock::OK, 1));

  bcancel->
    signal_clicked().
    connect(sigc::mem_fun(*this, &EventWin::on_button_cancel));
  bok->
    signal_clicked().
    connect(sigc::mem_fun(*this, &EventWin::on_button_ok));

  bok->set_sensitive(false);

  show_all_children();
}

EventWin::~EventWin()
{
  remove_all_ui();
}

int EventWin::get_type(void)
{
  switch (events_combo.get_active_row_number())
    {
    case 0: // EVENT_ENTER_MAP
      return EVENT_ENTER_MAP;
      break;
    default:
      return -666;
      break;
    }
}

void EventWin::remove_all_ui(void)
{
  if (modular_vbox)
    {
      delete modular_vbox;
      modular_vbox = NULL;
    }
  
  if (x_entry)
    {
      delete x_entry;
      x_entry = NULL;
    }
  if (x_lbl)
    {
      delete x_lbl;
      x_lbl = NULL;
    }
  if (x_hbox)
    {
      delete x_hbox;
      x_hbox = NULL;
    }
  if (y_entry)
    {
      delete y_entry;
      y_entry = NULL;
    }
  if (y_lbl)
    {
      delete y_lbl;
      y_lbl = NULL;
    }
  if (y_hbox)
    {
      delete y_hbox;
      y_hbox = NULL;
    }
  if (city_entry)
    {
      delete city_entry;
      city_entry = NULL;
    }
  if (city_lbl)
    {
      delete city_lbl;
      city_lbl = NULL;
    }
  if (city_hbox)
    {
      delete city_hbox;
      city_hbox = NULL;
    }
}

void EventWin::add_event_enter_map_ui(void)
{
  remove_all_ui();
  modular_vbox = new Gtk::VBox();
  vbox.add(*modular_vbox);

  if (!x_hbox)
    {
      x_hbox = new Gtk::HBox();
      modular_vbox->add(*x_hbox);
    }
  if (!x_lbl)
    {
      x_lbl = new Gtk::Label("x:");
      x_hbox->add(*x_lbl);
    }
  if (!x_entry)
    {
      x_entry = new Gtk::Entry();
      x_entry->set_text("0");
      x_hbox->add(*x_entry);
    }
  if (!y_hbox)
    {
      y_hbox = new Gtk::HBox();
      modular_vbox->add(*y_hbox);
    }
  if (!y_lbl)
    {
      y_lbl = new Gtk::Label("y:");
      y_hbox->add(*y_lbl);
    }
  if (!y_entry)
    {
      y_entry = new Gtk::Entry();
      y_entry->set_text("0");
      y_hbox->add(*y_entry);
    }
  if (!city_hbox)
    {
      city_hbox = new Gtk::HBox();
      modular_vbox->add(*city_hbox);
    }
  if (!city_lbl)
    {
      city_lbl = new Gtk::Label("Map name:");
      city_hbox->add(*city_lbl);
    }
  if (!city_entry)
    {
      city_entry = new Gtk::Entry();
      city_entry->set_text("map_name");
      city_hbox->add(*city_entry);
    }
}

void EventWin::on_combo_changed(void)
{
  switch (events_combo.get_active_row_number())
    {
    case 0: // EVENT_ENTER_MAP
      add_event_enter_map_ui();
      break;
    default:
      remove_all_ui();
      break;
    }
  bok->set_sensitive(true);
  show_all_children();
}

void EventWin::on_button_cancel(void)
{
}

void EventWin::on_button_ok(void)
{
}

unsigned EventWin::get_x(void)
{
  if (x_entry)
    return (unsigned)atoi(x_entry->get_text().c_str());
  return 0;
}

unsigned EventWin::get_y(void)
{
  if (y_entry)
    return (unsigned)atoi(y_entry->get_text().c_str());
  return 0;
}

std::string EventWin::get_city(void)
{
  if (city_entry)
    return city_entry->get_text().c_str();
  throw NoEventEntry("EventWin::city_entry is NULL.");
}
