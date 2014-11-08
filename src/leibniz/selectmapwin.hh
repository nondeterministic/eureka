// ----------------------------------------------------------------
// leibniz
//
// Copyright (c) 2009  Andreas Bauer <baueran@gmail.com>
//
// This is not free software.  See COPYING for further information.
// ----------------------------------------------------------------

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
