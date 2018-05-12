// This source file is part of eureka
//
// Copyright (c) 2007-2018  Andreas Bauer <a@pspace.org>
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

#include "boost/lexical_cast.hpp"
#include <memory>
#include <glibmm.h>
#include <cstdlib>
#include <iostream>
#include "newmapwin.hh"
#include "../world.hh"
#include "../indoorsmap.hh"
#include "../outdoorsmap.hh"

NewMapWin::NewMapWin(void)
  : bok(Gtk::Stock::OK),
    bcancel(Gtk::Stock::CANCEL),
    name_lbl("Map identifier/name:"),
    x_lbl("Width:"),
    y_lbl("Height:"),
    rbframe("Map type"),
    dimensionsframe("Dimensions"),
    rb_wilderness("Wilderness"),
    rb_indoors("Indoors")
{
  set_title("Set some basic map properties");
  show_all_children();
  set_modal(true);

  align.set_padding(10, 10, 10, 10);
  add(align);

  hbox_e.set_spacing(10);
  hbox_e.add(name_lbl);
  hbox_e.add(name_entry);

  vbox.add(hbox_e);

  Gtk::RadioButton::Group group = rb_wilderness.get_group();
  rb_indoors.set_group(group);
  rb_indoors.set_active();
  rbvbox.add(rb_wilderness);
  rbvbox.add(rb_indoors);
  rbvbox.set_border_width(5);
  rbframe.add(rbvbox);
  vbox.add(rbframe);

  hbox_x.set_spacing(10);
  hbox_x.add(x_lbl);
  // TODO: Make these two entries accept only (a certain range of)
  // integers!
  x_entry.set_text("10");
  y_entry.set_text("10");
  hbox_x.pack_start(x_entry, Gtk::PACK_SHRINK);
  hbox_y.set_spacing(10);
  hbox_y.add(y_lbl);
  hbox_y.pack_start(y_entry, Gtk::PACK_SHRINK);
  dimensionsvbox.add(hbox_x);
  dimensionsvbox.add(hbox_y);
  dimensionsframe.add(dimensionsvbox);
  vbox.add(dimensionsframe);

  vbox.add(sep);
  align.add(vbox);

  name_entry.add_events(Gdk::KEY_PRESS_MASK|Gdk::KEY_RELEASE_MASK);
  name_entry.
    signal_key_release_event().
    connect(sigc::mem_fun(*this, &NewMapWin::my_key_release_event), false);
  
  bcancel.
    signal_clicked().
    connect(sigc::mem_fun(*this, &NewMapWin::on_button_cancel));
  bok.
    signal_clicked().
    connect(sigc::mem_fun(*this, &NewMapWin::on_button_ok));
  // Until the user entered a name/id for the world to be created
  bok.set_sensitive(false);

  hbox_b.set_homogeneous();
  hbox_b.set_spacing(10);
  hbox_b.pack_end(bok);
  hbox_b.pack_end(bcancel);
  hbox_b_space.set_spacing(50);
  hbox_b.pack_end(hbox_b_space);
  vbox.add(hbox_b);
  vbox.set_spacing(10);

  bok.set_can_default();
  bok.grab_default();

  show_all_children();
}

// See http://www.mail-archive.com/gtkmm-list@gnome.org/msg07222.html
bool NewMapWin::my_key_release_event(GdkEventKey* event)
{
  if (!name_entry.get_text().empty())
    bok.set_sensitive();
  else
    bok.set_sensitive(false);

  return false;
}

NewMapWin::~NewMapWin(void)
{
}

Glib::ustring NewMapWin::get_name_entry(void)
{
  return name_entry.get_text();
}

bool NewMapWin::is_outdoors(void)
{
  return rb_wilderness.get_active();
}

bool NewMapWin::is_indoors(void)
{
  return !is_outdoors();
}

void NewMapWin::on_button_cancel(void)
{
  name_entry.set_text("");
  hide();
}

void NewMapWin::on_button_ok(void)
{
  if (World::Instance().exists_map(name_entry.get_text().c_str()))
    {
      Gtk::MessageDialog 
	msgdlg("Map name already exists.\nChoose a different name!", 
	       true,
	       Gtk::MESSAGE_ERROR,
	       Gtk::BUTTONS_CLOSE);
      msgdlg.set_title("Error");
      msgdlg.run();
      msgdlg.hide(); 
      //      msgdlg.hide_all(); 

      return;
    }

  std::shared_ptr<Map> new_map;

  unsigned width = 0;
  unsigned height = 0;

  try {
    width  = boost::lexical_cast<unsigned>(x_entry.get_text().c_str());
    height = boost::lexical_cast<unsigned>(y_entry.get_text().c_str());
  }
  catch (...)
    {
      std::cerr << "Bad dimensions?" << std::endl;
    }

  if (rb_indoors.get_active())
    new_map = std::make_shared<IndoorsMap>(width, height);
  else
    new_map = std::make_shared<OutdoorsMap>(width, height);

  new_map->set_name(name_entry.get_text().c_str());
  
  bool new_map_added = World::Instance().add_map(new_map);
  
  if (!new_map_added)
    {
      Gtk::MessageDialog 
	msgdlg("Could not add new map to world.\nOut of memory?!", 
	       true,
	       Gtk::MESSAGE_ERROR,
	       Gtk::BUTTONS_CLOSE);
      msgdlg.set_title("Error");
      msgdlg.run();
      msgdlg.hide(); 
      // msgdlg.hide_all(); 
    }

  hide();
}
