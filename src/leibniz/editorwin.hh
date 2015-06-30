// ----------------------------------------------------------------
// leibniz
//
// Copyright (c) 2009  Andreas Bauer <baueran@gmail.com>
//
// This is not free software.  See COPYING for further information.
// ----------------------------------------------------------------

#ifndef EDITORWIN_HH
#define EDITORWIN_HH

#include <gtkmm.h>
#include <gtkmm/socket.h>
#include <gdkmm/pixbuf.h>
#include <glibmm/refptr.h>
#include <vector>
#include <memory>
#include "sdleditor.hh"
#include "../indoorsmap.hh"
#include "tabcontext.hh"

class EditorWin : public Gtk::Window
{
public:
  EditorWin(bool = true);
  virtual ~EditorWin();

  bool add_sdleditor_tab(const char*);
  void on_menu_file_save();
  void on_menu_file_quit();
  void on_menu_map_new();
  void on_menu_map_close();
  void on_menu_map_open();
  // Note we can use negative expansion to shrink the map, i.e.,
  // delete rows/columns.  No separate menu item will be created for
  // this function.
  void on_menu_map_expand();
  bool on_tab_button_press_event(GdkEventButton*);
  bool on_tab_button_release_event(GdkEventButton*);
  bool on_tab_button_motion_pressed(GdkEventMotion*);
  bool on_swindow_button_press_event(GdkEventButton*);
  bool on_my_key_press_event(GdkEventKey*);
  bool on_my_sig_expose(const Cairo::RefPtr<Cairo::Context>&);
  void on_my_switch_page(Gtk::Widget*, guint);
  bool on_my_configure_event(GdkEventConfigure*);
  void on_tb_show_map_toggled();
  void on_tb_show_obj_toggled();
  void on_tb_show_act_toggled();
  bool grid_on() const;
  void set_grid(bool = true);
  void determine_map_offsets();
  void put_curr_tile_on_map(int, int);
  // void put_curr_tile_in_obj_register(int, int);
  void rm_obj(int, int);
  void add_action(int, int);
  void add_object(int, int);
  void del_action(int, int);
  std::shared_ptr<Map> get_curr_map() const;
  TabContext* context();
  Glib::RefPtr<Gdk::Pixbuf> get_selected_icon_pb();

protected:
  SDLEditor* _sdleditor;

  Gtk::Socket _socket;
  Gtk::Image _indoors_icon_pic, _outdoors_icon_pic;

  // Current tab's ebox
  Gtk::EventBox _tab_ebox;

  Glib::RefPtr<Gtk::ActionGroup> ref_actiongr;
  Glib::RefPtr<Gtk::UIManager> ref_uimgr;
  Glib::RefPtr<Gdk::Pixbuf> _selected_icon_pb;

  Gtk::VBox vbox, vtoolbox;
  Gtk::HBox _swin_icons_hbox, hbox, togglehbox;
  Gtk::ScrolledWindow _swin_icons;
  Gtk::Notebook nb_main;
  Gtk::RadioButton rb_draw_map, rb_draw_obj, rb_del_obj, rb_add_action, rb_del_action;
  Gtk::RadioButton::Group toolbox_gr;
  Gtk::ToggleButton tb_show_map, tb_show_obj, tb_show_act;

  std::vector<TabContext> _tab_contexts;

  void place_icon_on_map(int x, int y);

  bool _show_grid;
  bool _drag;
};

#endif
