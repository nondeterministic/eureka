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

#ifndef EDITORWIN_HH
#define EDITORWIN_HH

#include <gtkmm.h>
#include <gtkmm/socket.h>
#include <gdkmm/pixbuf.h>
#include <glibmm/refptr.h>

#include <vector>
#include <memory>
#include <thread>

#include "../indoorsmap.hh"
#include "sdleditor.hh"
#include "tabcontext.hh"

class EditorWin : public Gtk::Window
{
public:
	static volatile bool isInSDLWindow;

	EditorWin(bool = true);
	EditorWin(const EditorWin&) = delete;
	virtual ~EditorWin();

	bool add_sdleditor_tab(const std::string);
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
	bool on_motion_notify_event(GdkEventMotion*);
	// void on_state_changed(Gtk::StateFlags);
	// bool on_tab_motion(GdkEventMotion*);
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
	void fill_with_curr_tile(int, int);
	void put_curr_tile_on_map(int, int);
	// void put_curr_tile_in_obj_register(int, int);
	void rm_obj(int, int);
	void add_action(int, int);
	void add_object(int, int);
	void del_action(int, int);
	std::shared_ptr<Map> get_curr_map() const;
	std::shared_ptr<TabContext> context() const;
	Glib::RefPtr<Gdk::Pixbuf> get_selected_icon_pb();

protected:
	SDLEditor* _sdleditor;

	Gtk::Image _indoors_icon_pic, _outdoors_icon_pic;

	Gtk::EventBox _tab_ebox;

	Glib::RefPtr<Gtk::ActionGroup> _ref_actiongr;
	Glib::RefPtr<Gtk::UIManager> _ref_uimgr;
	Glib::RefPtr<Gdk::Pixbuf> _selected_icon_pb;

	Gtk::VBox _vbox, _vtoolbox;
	Gtk::HBox _swin_icons_hbox, _hbox, _togglehbox;
	Gtk::ScrolledWindow _swin_icons;
	Gtk::Notebook _nb_main;
	Gtk::RadioButton _rb_draw_map, _rb_fill_map, _rb_draw_obj, _rb_del_obj, _rb_add_action, _rb_del_action;
	Gtk::RadioButton::Group _toolbox_gr;
	Gtk::ToggleButton _tb_show_map, _tb_show_obj, _tb_show_act;

	Gtk::Label _coords_lbl;

	std::vector<std::shared_ptr<TabContext>> _tab_contexts;

	std::thread* _thread;

	bool _show_grid;
	bool _drag;

	static void swallow_sdl_events();
	void place_icon_on_map(int x, int y);
	void fill(std::shared_ptr<Map>, unsigned, unsigned, unsigned, unsigned);
};

#endif
