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

#ifndef __SELECTMAPWIN_HH
#define __SELECTMAPWIN_HH

#include <gtkmm.h>
#include <string>

// /usr/share/doc/gtkmm-documentation/examples/book/treeview/list

class SelectMapWin : public Gtk::Window
{
public:
  SelectMapWin();
  virtual ~SelectMapWin();
  std::string get_selected_map() const;

protected:
  // Tree model columns (cf. GTKMM examples)
  class ModelColumns : public Gtk::TreeModel::ColumnRecord
  {
  public:
    
    ModelColumns()
    {
      add(m_col_name); add(m_col_outdoors);
    }
    
    Gtk::TreeModelColumn<Glib::ustring> m_col_name;
    Gtk::TreeModelColumn<bool> m_col_outdoors;
  };

  Gtk::Button bok, bcancel;
  Gtk::HSeparator sep;
  Gtk::Alignment align;
  Gtk::VBox vbox;
  Gtk::HBox hbox_b, hbox_b_space;
  Gtk::ScrolledWindow _swin;
  Gtk::TreeView _tview;
  Glib::RefPtr<Gtk::ListStore> _tmodel;
  ModelColumns _cols;
  std::string _selected_map;

  void on_button_cancel();
  void on_button_ok();
  void row_changed_event();
};

#endif
