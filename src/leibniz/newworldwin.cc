// leibniz
//
// Copyright (c) 2009  Andreas Bauer <baueran@gmail.com>
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

#include <cstdlib>
#include "newworldwin.hh"
#include "../world.hh"

#include "boost/filesystem.hpp"
#include "boost/filesystem/path.hpp"

NewWorldWin::NewWorldWin(void)
  :  name_lbl("World identifier/name:"),
     path_lbl("World path:")
{
  set_title("Set some basic world properties");
  show_all_children();
  set_modal(true);

  align.set_padding(10, 10, 10, 10);
  get_vbox()->add(align);

  hbox_e.set_spacing(10);
  hbox_e.add(name_lbl);
  hbox_e.pack_start(name_entry, Gtk::PACK_SHRINK);

  hbox_path.set_spacing(10);
  hbox_path.add(path_lbl);
  hbox_path.pack_start(path_entry, Gtk::PACK_SHRINK);
  path_entry.set_text((std::string)(getenv("HOME")) + "/.leibniz/worlds/");

  vbox.add(hbox_e);
  vbox.add(hbox_path);
  align.add(vbox);

  name_entry.add_events(Gdk::KEY_PRESS_MASK|Gdk::KEY_RELEASE_MASK);
  name_entry.
    signal_key_release_event().
    connect(sigc::mem_fun(*this, &NewWorldWin::my_key_release_event), false);
  
  bcancel = manage(add_button(Gtk::Stock::CANCEL, 0));
  bok = manage(add_button(Gtk::Stock::OK, 1));

  bcancel->
    signal_clicked().
    connect(sigc::mem_fun(*this, &NewWorldWin::on_button_cancel));
  bok->
    signal_clicked().
    connect(sigc::mem_fun(*this, &NewWorldWin::on_button_ok));
  // Until the user entered a name/id for the world to be created
  bok->set_sensitive(false);

  vbox.set_spacing(10);

  bok->set_can_default();
  bok->grab_default();

  show_all_children();
}

// See http://www.mail-archive.com/gtkmm-list@gnome.org/msg07222.html
bool NewWorldWin::my_key_release_event(GdkEventKey* event)
{
  if (!name_entry.get_text().empty())
    bok->set_sensitive();
  else
    bok->set_sensitive(false);

  return false;
}

NewWorldWin::~NewWorldWin(void)
{
}

void NewWorldWin::on_button_cancel(void)
{
  World::Instance().set_name("");
  hide();
}

void NewWorldWin::on_button_ok(void)
{
	boost::filesystem::path tmp_path(path_entry.get_text());
	World::Instance().set_name(name_entry.get_text().c_str());
	World::Instance().set_world_path(tmp_path.c_str());
	hide();
}
