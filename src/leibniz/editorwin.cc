// This source file is part of eureka / leibniz
//
// Copyright (c) 2007-2017  Andreas Bauer <baueran@gmail.com>
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

#include <stdexcept>
#include <iostream>
#include <memory>
#include <cmath>
#include <sstream>
#include <thread>

#include <gtkmm/stock.h>
#include <gdk/gdkx.h>
#include <gdkmm/pixbuf.h>
#include <glibmm/refptr.h>

#include "sdleditor.hh"
#include "editorwin.hh"
#include "newmapwin.hh"
#include "../world.hh"
#include "../mapobj.hh"
#include "tabcontext.hh"
#include "selectmapwin.hh"
#include "expandmapwin.hh"
#include "iconpropswin.hh"
#include "../outdoorsicons.hh"
#include "../indoorsicons.hh"
#include "addactionwin.hh"
#include "addobjectwin.hh"
#include "../action.hh"
#include "../actiononenter.hh"
#include "../actionpullpush.hh"
#include "eventwin.hh"
#include "../gameevent.hh"
#include "../eventermap.hh"
#include "../eventchangeicon.hh"

EditorWin::EditorWin(bool new_world)
: _indoors_icon_pic((World::Instance().get_path() / World::Instance().get_name() / "images" / "icons_indoors.png").c_str()),
  _outdoors_icon_pic((World::Instance().get_path() / World::Instance().get_name() / "images" / "icons_outdoors.png").c_str()),
  _rb_draw_map("Draw map"),
  _rb_fill_map("Fill map"),
  _rb_draw_obj("Draw object"),
  _rb_del_obj("Delete object"),
  _rb_add_action("Add action/event..."),
  _rb_del_action("Delete action"),
  _tb_show_map("Show map"),
  _tb_show_obj("Show objects"),
  _tb_show_act("Show actions"),
  _coords_lbl("Dummy")
{
	_show_grid = true;
	_drag = false;

	// Initially there is no editor attached.
	_sdleditor = NULL;

	signal_configure_event().connect(sigc::mem_fun(*this, &EditorWin::on_my_configure_event), false);

	add_events(Gdk::KEY_PRESS_MASK | Gdk::KEY_RELEASE_MASK);
	signal_key_press_event().connect(sigc::mem_fun(*this, &EditorWin::on_my_key_press_event), false);

	//#ifndef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
	signal_draw().connect(sigc::mem_fun(*this, &EditorWin::on_my_sig_expose));
	//#endif

	// Now, on to standard stuff, i.e., GUI setup
	set_title("Leibniz - " + World::Instance().get_name());

	set_default_size(400,300);

	add(_vbox);

	// Create actions for menus and toolbars:
	_ref_actiongr = Gtk::ActionGroup::create();

	_ref_actiongr->add(Gtk::Action::create("FileMenu", "File"));

	_ref_actiongr->add(Gtk::Action::create("FileMenuNew", Gtk::Stock::NEW, "New world..."));
	_ref_actiongr->add(Gtk::Action::create("FileMenuSave", Gtk::Stock::SAVE, "Save current world"), sigc::mem_fun(*this, &EditorWin::on_menu_file_save));
	_ref_actiongr->add(Gtk::Action::create("FileMenuQuit", Gtk::Stock::QUIT), sigc::mem_fun(*this, &EditorWin::on_menu_file_quit));

	_ref_actiongr->add(Gtk::Action::create("MapMenu", "Map"));

	_ref_actiongr->add(Gtk::Action::create("MapMenuNew", Gtk::Stock::NEW, "_Create new map..."),	Gtk::AccelKey("<alt>C"), sigc::mem_fun(*this, &EditorWin::on_menu_map_new));
	_ref_actiongr->add(Gtk::Action::create("MapMenuOpen", Gtk::Stock::OPEN, "_Open existing map.."),	sigc::mem_fun(*this, &EditorWin::on_menu_map_open));
	_ref_actiongr->add(Gtk::Action::create("MapMenuExpand", Gtk::Stock::FULLSCREEN,"Change map size..."),sigc::mem_fun(*this, &EditorWin::on_menu_map_expand));
	_ref_actiongr->add(Gtk::Action::create("MapMenuClose", Gtk::Stock::CLOSE, "_Close map"),	Gtk::AccelKey("<ctrl>W"), sigc::mem_fun(*this, &EditorWin::on_menu_map_close));

	_ref_actiongr->get_action("FileMenuNew")->set_sensitive(false);
	_ref_actiongr->get_action("FileMenuSave")->set_sensitive(false);

	if (World::Instance().get_maps()->size() == 0)
		_ref_actiongr->get_action("MapMenuOpen")->set_sensitive(false);

	if (get_curr_map() == NULL) {
		_ref_actiongr->get_action("MapMenuClose")->set_sensitive(false);
		_ref_actiongr->get_action("MapMenuExpand")->set_sensitive(false);
	}

	_ref_uimgr = Gtk::UIManager::create();
	_ref_uimgr->insert_action_group(_ref_actiongr);

	add_accel_group(_ref_uimgr->get_accel_group());

	Glib::ustring ui_info =
			"<ui>"
			" <menubar name='MenuBar'>"
			"  <menu action='FileMenu'>"
			"   <menuitem action='FileMenuNew'/>"
			"   <separator/>"
			"   <menuitem action='FileMenuSave'/>"
			"   <separator/>"
			"   <menuitem action='FileMenuQuit'/>"
			"  </menu>"
			"  <menu action='MapMenu'>"
			"   <menuitem action='MapMenuNew'/>"
			"   <menuitem action='MapMenuOpen'/>"
			"   <menuitem action='MapMenuExpand'/>"
			"   <separator/>"
			"   <menuitem action='MapMenuClose'/>"
			"  </menu>"
			" </menubar>"
			"</ui>";

#ifdef GLIBMM_EXCEPTIONS_ENABLED
	try {
		_ref_uimgr->add_ui_from_string(ui_info);
	}
	catch(const Glib::Error& ex) {
		std::cerr << "building menus failed: " <<  ex.what();
	}
#else
	std::auto_ptr<Glib::Error> ex;
	_ref_uimgr->add_ui_from_string(ui_info, ex);
	if(ex.get()) {
		std::cerr << "building menus failed: " <<  ex->what();
	}
#endif //GLIBMM_EXCEPTIONS_ENABLED

	// Get the menubar and toolbar widgets, and add them to a container widget
	Gtk::Widget* ptr_menubar = _ref_uimgr->get_widget("/MenuBar");
	if (ptr_menubar)
		_vbox.pack_start(*ptr_menubar, Gtk::PACK_SHRINK);

	// vbox.add(nb_main);
	_vbox.add(_hbox);
	_hbox.pack_start(_vtoolbox, Gtk::PACK_SHRINK);
	_rb_draw_map.set_active();
	_rb_draw_map.set_mode(false);
	_rb_fill_map.set_active();
	_rb_fill_map.set_mode(false);
	_rb_draw_obj.set_mode(false);
	_rb_del_obj.set_mode(false);
	_rb_add_action.set_mode(false);
	_rb_del_action.set_mode(false);
	_toolbox_gr = _rb_draw_map.get_group();
	_rb_fill_map.set_group(_toolbox_gr);
	_rb_draw_obj.set_group(_toolbox_gr);
	_rb_del_obj.set_group(_toolbox_gr);
	_rb_add_action.set_group(_toolbox_gr);
	_rb_del_action.set_group(_toolbox_gr);
	_hbox.add(_nb_main);
	_tb_show_map.set_active();
	_tb_show_obj.set_active();
	_tb_show_act.set_active();
	_togglehbox.pack_start(_tb_show_map, Gtk::PACK_SHRINK);
	_togglehbox.pack_start(_tb_show_obj, Gtk::PACK_SHRINK);
	_togglehbox.pack_start(_tb_show_act, Gtk::PACK_SHRINK);
	_tb_show_map.signal_toggled().connect(sigc::mem_fun(*this, &EditorWin::on_tb_show_map_toggled));
	_tb_show_obj.signal_toggled().connect(sigc::mem_fun(*this, &EditorWin::on_tb_show_obj_toggled));
	_tb_show_act.signal_toggled().connect(sigc::mem_fun(*this, &EditorWin::on_tb_show_act_toggled));

	show_all_children();

	_thread = NULL;
//	thread = new std::thread(swallow_sdl_events);
//	thread->detach();
}

EditorWin::~EditorWin(void)
{
	std::cout << "~EditorWin()\n";

	if (_thread != NULL)
		delete _thread;
}

void EditorWin::swallow_sdl_events()
{
	SDL_Event e;

	while (true) {
		if (isInSDLWindow) {
			std::cout << "Waiting for event...\n";
			SDL_WaitEvent(&e);
		}
	}
}

void EditorWin::on_tb_show_map_toggled()
{
	if (_sdleditor != NULL)
		_sdleditor->set_show_map(_tb_show_map.get_active());
	else
		std::cerr << "ERROR: editorwin.cc: show map toggled: sdleditor == NULL.\n";
}

void EditorWin::on_tb_show_obj_toggled()
{
	if (_sdleditor != NULL)
		_sdleditor->set_show_obj(_tb_show_obj.get_active());
	else
		std::cerr << "ERROR: editorwin.cc: show obj toggled: sdleditor == NULL.\n";
}

void EditorWin::on_tb_show_act_toggled()
{
	if (_sdleditor != NULL)
		_sdleditor->set_show_act(_tb_show_act.get_active());
	else
		std::cerr << "ERROR: editorwin.cc: show act toggled: sdleditor == NULL.\n";
}

void EditorWin::on_menu_file_save()
{
	for (auto curr_map = World::Instance().get_maps()->begin(); curr_map != World::Instance().get_maps()->end(); curr_map++)
		if ((*curr_map)->modified())
			if ((*curr_map)->xml_write_map_data())
				(*curr_map)->set_notmodified();

	World::Instance().xml_write_world_data();
	_ref_actiongr->get_action("FileMenuSave")->set_sensitive(false);
}

void EditorWin::on_menu_file_quit()
{
	hide();
	std::cout << "INFO: editorwin.cc: Trying to delete SDL editor...\n";
	if (_sdleditor != NULL)
		delete _sdleditor;
	std::cout << "INFO: editorwin.cc: Deleted _sdleditor.\n";
}

void EditorWin::on_menu_map_new()
{
	NewMapWin newmap_win;
	Gtk::Main::run(newmap_win);

	if (!newmap_win.get_name_entry().empty())
		add_sdleditor_tab(newmap_win.get_name_entry().c_str());
}

void EditorWin::on_menu_map_open()
{
	SelectMapWin selectmap_win;
	Gtk::Main::run(selectmap_win);
	std::shared_ptr<Map> the_map;

	try {
		the_map = World::Instance().get_map(selectmap_win.get_selected_map().c_str());
		if (the_map->xml_load_map_data())
			add_sdleditor_tab(the_map->get_name().c_str());
		else
			std::cerr << "Map data load failed." << std::endl;
	}
	catch (const MapNotFound& e) {
		std::cerr << "No such map exists in memory?!" << std::endl;
	}
}

void EditorWin::on_menu_map_close()
{
	if (get_curr_map() == NULL)
		return;

	if (get_curr_map()->modified()) {
		Gtk::MessageDialog
		notsaved_dlg("Map data not saved to disk.",
				true,
				Gtk::MESSAGE_QUESTION,
				Gtk::BUTTONS_YES_NO);
		notsaved_dlg.set_title("Map not saved");
		notsaved_dlg.set_secondary_text("Do you really want to close it?");
		if (notsaved_dlg.run() == Gtk::RESPONSE_NO)
			return;
	}

	// If there is at least one other tab open, switch to that before
	// deleting objects.
	//
	// TODO: this case may not be as stable as the else-branch of this
	// if...  (if in doubt, rework again from the else-branch whose code
	// has been stable for some time.)
	if (_nb_main.get_n_pages() > 1) {
		// As the current context returned by context() depends on the
		// current page, we create a pointer to the current context.
		std::shared_ptr<TabContext> tmp_context = context();
		std::shared_ptr<Gtk::VBox> tmp_vbox = tmp_context->get_box();

		// Remove the tabcontext from the context vector
		for (auto curr_context = _tab_contexts.begin(); curr_context != _tab_contexts.end(); curr_context++) {
			if (*curr_context == tmp_context) {
				_tab_contexts.erase(curr_context);
				break;
			}
		}

		if (_sdleditor != NULL) {
			// Store pointer to map data temporarily
			std::shared_ptr<Map> tmp_map = get_curr_map();

			// Free some RAM
			tmp_map->unload_map_data();

			delete _sdleditor;
			_sdleditor = NULL;

			// If the map has never been saved before, remove it for good
			if (!tmp_map->exists_on_disk())
				World::Instance().delete_map(tmp_map);
		}

		if (_nb_main.get_current_page() == 0)
			_nb_main.next_page();
		else
			_nb_main.prev_page();

		// Now remove vbox from current page.  Note that this may cause an automatic switch if tmp_vbox is the current one.
		_nb_main.remove(*tmp_vbox);

		// No need to delete vbox as the vbox will be reused for the remaining opened tab(s).
		// In fact, deleting would be a very, very bad idea!
	}
	// This case should really never occur...
	else if (_nb_main.get_n_pages() == 0)
		return;
	// This is the case when exactly one editor is currently open.
	else {
		// Not really necessary as the SDL window isn't really a child widget of the event box, but doesn't hurt either...
		_tab_ebox.remove();

		// Remove eventbox from current vbox
		context()->get_box()->remove(_tab_ebox);

		// As the current context returned by context() depends on the current page, we create a pointer to the current context.
		std::shared_ptr<TabContext> tmp_context = context();

		// Now remove vbox from current page
		_nb_main.remove(*(context()->get_box()));

		// Remove the tabcontext from the context vector
		for (auto curr_context = _tab_contexts.begin(); curr_context != _tab_contexts.end(); curr_context++) {
			if (*curr_context == tmp_context) {
				_tab_contexts.erase(curr_context);
				break;
			}
		}

		if (_sdleditor != NULL) {
			// Store pointer to map data temporarily
			std::shared_ptr<Map> tmp_map = get_curr_map();

			// Free some RAM
			tmp_map->unload_map_data();

			delete _sdleditor;
			_sdleditor = NULL;

			// If the map has never been saved before, remove it for good
			if (!tmp_map->exists_on_disk())
				World::Instance().delete_map(tmp_map);
		}

		// Now also remove scrolledwindow as there is no more tabcontext
		// where the user can store a current brush in.  Clicking into
		// the scrolledwindow will in fact crash.
		_swin_icons.hide();
		_togglehbox.hide();
		_vtoolbox.hide();

		_ref_actiongr->get_action("MapMenuClose")->set_sensitive(false);
		_ref_actiongr->get_action("MapMenuExpand")->set_sensitive(false);
	}

	std::vector<std::shared_ptr<Map>>* vmaps = World::Instance().get_maps();
	for (auto curr_map = vmaps->begin(); curr_map != vmaps->end(); curr_map++) {
		if ((*curr_map)->exists_on_disk()) {
			// There is now at least one unopened map that can be
			// reopened, hence activate menu entry for that
			_ref_actiongr->get_action("MapMenuOpen")->set_sensitive(true);
			break;
		}
	}
}

// Note, negative expansion deletes rows/columns.

void EditorWin::on_menu_map_expand()
{
	ExpandMapWin* expandmap_win = new ExpandMapWin("Change map size", "How many rows/columns do you want to add?");

	if (expandmap_win->run()) {
		int top, bot, right, left;
		expandmap_win->get_values(top, bot, right, left);
		_sdleditor->get_map()->expand_map(top, bot, right, left);
		_ref_actiongr->get_action("FileMenuSave")->set_sensitive(true);
		this->queue_draw();
	}

	delete expandmap_win;
}

void EditorWin::determine_map_offsets()
{
	if (_tab_ebox.get_parent() == nullptr || _sdleditor == NULL)
		return;

	bool is_outdoors = get_curr_map()->is_outdoors();

	unsigned ts =
			is_outdoors?
					(World::Instance().get_outdoors_tile_size() - 1) :
					(World::Instance().get_indoors_tile_size() + (grid_on()? 1 : 0));

	int tx = 0, ty = 0, tw = 0, th = 0;
	_tab_ebox.get_window()->get_geometry(tx, ty, tw, th);

	Offsets new_offsets = _sdleditor->determine_offsets((unsigned)tw, (unsigned)th);

	// Determine how many icons are hidden and store it in the TabContext.
	context()->set_xoffset(new_offsets.left/(is_outdoors? (ts - 9) : ts));
	context()->set_yoffset(new_offsets.top/ts);
}

/// User has button pressed AND moves the mouse in editor window (or just moves)

bool EditorWin::on_tab_button_motion_pressed(GdkEventMotion* event)
{
	int map_x = 0;
	int map_y = 0;
	_sdleditor->pixel_to_map(event->x, event->y, map_x, map_y);

	std::stringstream ss;
	ss << "X: " << map_x << " y: " << map_y;

	_coords_lbl.set_text(ss.str());

	if (event->state & Gdk::BUTTON1_MASK)
		place_icon_on_map((int)event->x, (int)event->y);

	return false; // Not done.  Event will be further handled by GTKMM etc.
}

void EditorWin::place_icon_on_map(int x, int y)
{
	if (_rb_draw_map.get_active())
		put_curr_tile_on_map(x, y);
	else if (_rb_fill_map.get_active())
		fill_with_curr_tile(x, y);
	else if (_rb_draw_obj.get_active())
		add_object(x, y);
	else if (_rb_del_obj.get_active())
		rm_obj(x, y);
	else if (_rb_add_action.get_active())
		add_action(x, y);
	else if (_rb_del_action.get_active())
		del_action(x, y);
}

/// User clicked in editor window

bool EditorWin::on_tab_button_press_event(GdkEventButton* event)
{
	_drag = true;

	place_icon_on_map((int)event->x, (int)event->y);

	//	if (rb_draw_map.get_active())
	//		put_curr_tile_on_map((int)event->_, (int)event->y);
	//	else if (rb_draw_obj.get_active()) {
	//		add_object((int)event->x, (int)event->y);
	//		// put_curr_tile_in_obj_register((int)event->x, (int)event->y);
	//	}
	//	else if (rb_del_obj.get_active())
	//		rm_obj((int)event->x, (int)event->y);
	//	else if (rb_add_action.get_active())
	//		add_action((int)event->x, (int)event->y);
	//	else if (rb_del_action.get_active())
	//		del_action((int)event->x, (int)event->y);

	return false;
}

bool EditorWin::on_tab_button_release_event(GdkEventButton* event)
{
	// Unset drag handle!
	_drag = false;
	std::cout << "MOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO\n";
	return false;
}

bool EditorWin::on_motion_notify_event(GdkEventMotion* event)
{
	if (!_drag)
		return false;

	if (_rb_draw_map.get_active())
		put_curr_tile_on_map((int)event->x, (int)event->y);
	else if (_rb_draw_obj.get_active())
		put_curr_tile_on_map((int)event->x, (int)event->y);
		// put_curr_tile_in_obj_register((int)event->x, (int)event->y);

	return true;
}


void EditorWin::fill_with_curr_tile(int x, int y)
{
	int map_x = 0;
	int map_y = 0;
	_sdleditor->pixel_to_map(x, y, map_x, map_y);
	unsigned curr_brush = context()->get_icon_brush_no();
	std::shared_ptr<Map> map = get_curr_map();

	fill(map, curr_brush, map->get_tile(map_x, map_y), (unsigned)map_x, (unsigned)map_y);
	_ref_actiongr->get_action("FileMenuSave")->set_sensitive(true);
}

/// Fills position map_x, map_y on the map, which currently has tile old_brush on it, with new_brush.

void EditorWin::fill(std::shared_ptr<Map> map, unsigned new_brush, unsigned old_brush, unsigned map_x, unsigned map_y)
{
	map->set_tile(map_x, map_y, new_brush);

	for (int xoff = -1; xoff < 2; xoff++) {
		for (int yoff = -1; yoff < 2; yoff++) {
			if (xoff == 0 && yoff == 0)
				continue;

			if ((int)map_x + xoff < 0 || (int)map_y + yoff < 0)
				return;

			if ((int)map_x + xoff > (int)(map->height()) || (int)map_y + yoff > (int)(map->width()))
				return;

			if (map->get_tile((unsigned)(map_x + xoff), (unsigned)(map_y + yoff)) == (int)(old_brush))
				fill(map, new_brush, old_brush, map_x + xoff, map_y + yoff);
		}
	}
}

/// x and y are the current screen coordinates where the user clicked

void EditorWin::put_curr_tile_on_map(int x, int y)
{
	int map_x = 0;
	int map_y = 0;
	_sdleditor->pixel_to_map(x, y, map_x, map_y);
	unsigned curr_brush = context()->get_icon_brush_no();

	if ((unsigned)get_curr_map()->get_tile(map_x, map_y) != curr_brush) {
		if (get_curr_map()->set_tile(map_x, map_y, curr_brush) == 0) {
			_ref_actiongr->get_action("FileMenuSave")->set_sensitive(true);
			int icon_size = 0;
			if (get_curr_map()->is_outdoors())
				icon_size = World::Instance().get_indoors_tile_size();

			else
				icon_size = World::Instance().get_outdoors_tile_size();
			this->queue_draw_area(map_x, map_y, icon_size, icon_size);
		}
	}
}

void EditorWin::del_action(int x, int y)
{
	int map_x = 0;
	int map_y = 0;
	_sdleditor->pixel_to_map(x, y, map_x, map_y);

	get_curr_map()->del_action(map_x, map_y);
	this->queue_draw();
}

void EditorWin::add_action(int x, int y)
{
	int map_x = 0;
	int map_y = 0;
	_sdleditor->pixel_to_map(x, y, map_x, map_y);

	// If the current icon already hosts an action, then try to add
	// events to the action.  Only one action per icon though, but many
	// events per actions - if that makes any sense.  :-)
	// TODO: I think, I just removed the one action per tile constraint...
	if (get_curr_map()->get_actions((unsigned)map_x, (unsigned)map_y).size() > 0) {
		EventWin event_win;
		if (event_win.run()) {
			if (event_win.get_type() == EVENT_ENTER_MAP) {
				// Deleted by ~Action()
				std::shared_ptr<EventEnterMap> new_ev(new EventEnterMap());

				new_ev->set_x(event_win.get_x());
				new_ev->set_y(event_win.get_y());
				new_ev->set_map_name(event_win.get_city().c_str());

				get_curr_map()->add_event_to_action(map_x, map_y, new_ev);
			}
			else if (event_win.get_type() == EVENT_CHANGE_ICON) {
				std::shared_ptr<EventChangeIcon> new_ev(new EventChangeIcon());

				new_ev->x = event_win.get_x();
				new_ev->y = event_win.get_y();
				new_ev->icon_now = 0; // TODO: There is no GUI element to define this value, so one has to change it manually in the XML file!
				new_ev->icon_new = 0; // TODO: There is no GUI element to define this value, so one has to change it manually in the XML file!

				get_curr_map()->add_event_to_action(map_x, map_y, new_ev);
			}
			else
				std::cerr << "WARNING: editorwin.cc: Event not implemented yet! :-(" << std::endl;
		}
		return;
	}

	AddActionWin action_win;
	if (action_win.run()) {
		switch (action_win.get_action()) {
		case ACT_ON_ENTER: {
			// ~Map() deletes the memory.
			std::shared_ptr<Action> new_act(new ActionOnEnter(map_x, map_y, "ACT_ON_ENTER"));
			std::cout << "INFO: editorwin.cc: Added action at (" << map_x << ", " << map_y << ")" << std::endl;
			get_curr_map()->add_action(new_act);
			break;
		}
		case ACT_ON_PULLPUSH: {
			std::shared_ptr<Action> new_act(new ActionPullPush(map_x, map_y, "ACT_ON_PULLPUSH"));
			std::cout << "INFO: editorwin.cc: Added action at (" << map_x << ", " << map_y << ")" << std::endl;
			get_curr_map()->add_action(new_act);
			break;
		}
		default:
			std::cerr << "WARNING: editorwin.cc: Action not implemented yet! :-(" << std::endl;
			break;
		}

		this->queue_draw();
	}
}

void EditorWin::rm_obj(int x, int y)
{
	int map_x = 0;
	int map_y = 0;
	_sdleditor->pixel_to_map(x, y, map_x, map_y);

	get_curr_map()->pop_obj(map_x, map_y);

	_ref_actiongr->get_action("FileMenuSave")->set_sensitive(true);

	// This could be much more efficient if I would only draw the icon just drawn, but f*ck it for now...
	this->queue_draw();
}

// void EditorWin::put_curr_tile_in_obj_register(int x, int y)
// {
//   int map_x = 0;
//   int map_y = 0;
//   _sdleditor->pixel_to_map(x, y, map_x, map_y);
//   unsigned curr_icon_no = context()->get_icon_brush_no();

//   try {
//     get_curr_map()->push_obj(map_x, map_y, curr_icon_no);

//     ref_actiongr->get_action("FileMenuSave")->set_sensitive(true);

//     // This could be much more efficient if I would only draw the
//     // icon just drawn, but f*ck it for now...
//     this->queue_draw();
//   }
//   catch(...) {
//     std::cerr << "Adding of object failed." << std::endl;
//   }
// }

void EditorWin::add_object(int x, int y)
{
	AddObjectWin obj_win(_selected_icon_pb, context()->get_icon_brush_no());

	if (obj_win.run()) {
		MapObj newObj;
		int map_x = 0;
		int map_y = 0;
		_sdleditor->pixel_to_map(x, y, map_x, map_y);

		newObj.set_type(obj_win.get_object_type());
		newObj.set_coords(map_x, map_y);
		newObj.set_icon(obj_win.get_icon_no());
		newObj.id = obj_win.id;
		newObj.lua_name = obj_win.default_lua_name;

		// Auto-roam if type is person, animal or monster
		switch (newObj.get_type()) {
		unsigned x, y;
		newObj.get_coords(x, y);
		case MAPOBJ_ANIMAL:
		case MAPOBJ_PERSON:
			newObj.move_mode = ROAM;
			newObj.set_origin(x,y);
			break;
		case MAPOBJ_MONSTER:
			newObj.move_mode = FOLLOWING;
			newObj.set_origin(x,y);
			newObj.personality = HOSTILE;
			break;
		default: // MAPOBJ_ITEM:
			break;
		}

		// TODO: Just some dummy action added to object, so user can later type in the real one(s)
		if (obj_win.has_action()) {
			std::shared_ptr<Action> ptr(new ActionPullPush(map_x, map_y, "ACT_ON_PULL_PUSH"));
			newObj.add_action(ptr);
		}

		get_curr_map()->push_obj(newObj);

		_ref_actiongr->get_action("FileMenuSave")->set_sensitive(true);
		this->queue_draw();
	}

	_drag = false;
}

// User clicked in icon bar

bool EditorWin::on_swindow_button_press_event(GdkEventButton* event)
{
	Glib::RefPtr<Gtk::Adjustment> h_adj = _swin_icons.get_hadjustment();
	Glib::RefPtr<Gtk::Adjustment> v_adj = _swin_icons.get_vadjustment();
	int vadj = v_adj->get_value();
	int hadj = h_adj->get_value();
	int icon_size = 0;
	int icons_per_row = 0;
	int icon_no_pressed = 0;

	if (get_curr_map()->is_outdoors()) {
		if (event->x > _outdoors_icon_pic.get_width() || event->y > _outdoors_icon_pic.get_height())
			return true;

		icon_size = World::Instance().get_outdoors_tile_size() - 1;
		// The icon map is 661 pixels wide to not cut off abruptly, but
		// now we have to remove the -1 again in the icons_per_row
		// calculation.  So it's 660/33 = 20 icons per row.
		icons_per_row = (_outdoors_icon_pic.get_width() - 1)/icon_size;
		// TODO: Is the below OK now?
		// This 2 (resp 3) offsets are odd: it seems I need that because
		// the swindow has a tiny weeny frame around it and the event->x
		// coordinates are not relative wrt. the frame but relative to
		// the outside window!
		int x_coord = (int)event->x - _swin_icons_hbox.get_border_width() + hadj;
		int y_coord = (int)event->y - (_swin_icons_hbox.get_border_width() + 2) + vadj;

		icon_no_pressed = x_coord/icon_size + y_coord/icon_size*icons_per_row;
	}
	else {
		if (event->x > _indoors_icon_pic.get_width() || event->y > _indoors_icon_pic.get_height())
			return true;

		icon_size = World::Instance().get_indoors_tile_size();
		icons_per_row = _indoors_icon_pic.get_width()/icon_size;

		icon_no_pressed = ((int)event->x+hadj)/icon_size + (((int)event->y+vadj)/icon_size*icons_per_row);
	}

	// Determine icon rectangle
	int icon_x = (icon_no_pressed%icons_per_row) * icon_size;
	int icon_y = (int)(icon_no_pressed/icons_per_row) * icon_size;

	// Set selected icon for dialog to display
	Glib::RefPtr<Gdk::Pixbuf> icon_pixbuf = (get_curr_map()->is_outdoors()?
			_outdoors_icon_pic.get_pixbuf() :
			_indoors_icon_pic.get_pixbuf());
	// Glib::RefPtr<Gdk::Pixbuf> chosen_icon_pb =
	_selected_icon_pb = Gdk::Pixbuf::create_subpixbuf(icon_pixbuf, icon_x, icon_y, icon_size, icon_size);

	// Select current icon brush
	if (event->button == 1)
		context()->set_icon_brush_no(icon_no_pressed);
	// Edit icon properties
	else if (event->button == 3) {
		context()->set_icon_brush_no(icon_no_pressed);

		IconPropsWin icon_props_win(_selected_icon_pb);
		IconProps* curr_icon_props;

		if (get_curr_map()->is_outdoors())
			curr_icon_props = OutdoorsIcons::Instance().get_props(icon_no_pressed);
		else
			curr_icon_props = IndoorsIcons::Instance().get_props(icon_no_pressed);

		icon_props_win.set_icon_no(icon_no_pressed);
		icon_props_win.set_name(curr_icon_props->get_name().c_str());
		icon_props_win.set_walkable(curr_icon_props->_is_walkable);
		icon_props_win.set_trans(curr_icon_props->_trans);
		icon_props_win.set_default_lua_name(curr_icon_props->default_lua_name());

		if (icon_props_win.run()) {
			IconProps new_icon_props;
			new_icon_props.set_icon(icon_props_win.get_icon_no());
			new_icon_props.set_name(icon_props_win.get_name().c_str());
			new_icon_props._is_walkable  = icon_props_win.get_walkable();
			new_icon_props._trans = icon_props_win.get_trans();
			new_icon_props.default_lua_name() = icon_props_win.get_default_lua_name();

			if (get_curr_map()->is_outdoors())
				OutdoorsIcons::Instance().add_props(new_icon_props);
			else
				IndoorsIcons::Instance().add_props(new_icon_props);

			_ref_actiongr->get_action("FileMenuSave")->set_sensitive(true);
		}
	}

	return true;
}

bool EditorWin::on_my_key_press_event(GdkEventKey* event)
{
	bool handled = false;

	if (_tab_ebox.get_parent() == 0)
		return false;

	bool is_outdoors = get_curr_map()->is_outdoors();
	unsigned ts = 0;

	if (is_outdoors)
		ts = World::Instance().get_outdoors_tile_size() - 1;
	else
		ts = (World::Instance().get_indoors_tile_size() + (grid_on()? 1 : 0));

	determine_map_offsets();

	Offsets new_offsets;

	if ((std::string)gdk_keyval_name(event->keyval) == "Up") {
		new_offsets = _sdleditor->move(DIR_UP);
		handled = true;
	}
	else if ((std::string)gdk_keyval_name(event->keyval) == "Down") {
		new_offsets = _sdleditor->move(DIR_DOWN);
		handled = true;
	}
	else if ((std::string)gdk_keyval_name(event->keyval) == "Right") {
		new_offsets = _sdleditor->move(DIR_RIGHT);
		handled = true;
	}
	else if ((std::string)gdk_keyval_name(event->keyval) == "Left") {
		new_offsets =  _sdleditor->move(DIR_LEFT);
		handled = true;
	}

	// std::cout << "Key string: " << gdk_keyval_name(event->keyval) << "\n";
	// std::cout << "Key value: " << event->keyval << "\n";

	if (handled) {
		// Determine how many icons are hidden and store it in the
		// TabContext
		context()->set_xoffset(new_offsets.left/(is_outdoors? (ts - 9): ts));
		context()->set_yoffset(new_offsets.top/ts);
	}

	// TODO: This is weird, when I call queue_draw, then all widgets
	// redraw and sometimes not the SDL Window.  If I call a certain
	// area of a sub-widget to redraw, no matter how large the area,
	// then it seems to work

	// this->queue_draw_area(0, 0, 0, 0);
	_nb_main.queue_draw_area(0, 0, 100, 100);

	return handled;
}

bool EditorWin::on_my_configure_event(GdkEventConfigure* event)
{
	if (_tab_ebox.get_parent() != 0 && _sdleditor != NULL) {
		int tx = 0, ty = 0, tw = 0, th = 0;
		_tab_ebox.get_window()->get_geometry(tx, ty, tw, th);

		determine_map_offsets();
		_sdleditor->resize(tw, th);
	}

	return false;
}

bool EditorWin::on_my_sig_expose(const Cairo::RefPtr<Cairo::Context>& event)
{
	if (_tab_ebox.get_parent() != 0 && _sdleditor != NULL) {
		int tx = 0, ty = 0, tw = 0, th = 0;
		_tab_ebox.get_window()->get_geometry(tx, ty, tw, th);

		determine_map_offsets();
		_sdleditor->resize(tw, th);
		_sdleditor->show_grid();
		_sdleditor->show_map();
		_sdleditor->refresh();
	}
	else
		std::cerr << "WARNING: editorwin.cc: expose signal received but event box either without parent or SDLeditor == NULL.\n";

	// Returning true here is EXTREMELY important!
	//
	// It means that the expose event has been handled and is not passed
	// on.  If false is returned, we redraw, and keep the event in the
	// queue, after a while that leads to a relatively weird crash with
	// an X-server error message.
	return false;
}

//void EditorWin::on_state_changed(Gtk::StateFlags flags)
//{
//	std::cout << "TEST: on_state_changed\n";
//}

void EditorWin::on_my_switch_page(Gtk::Widget* page_notused, guint page_num)
{
	std::cout << "INFO: editorwin.cc: on_my_switch_page called.\n";

	std::string new_curr_map_name = _nb_main.get_tab_label_text(*(_tab_contexts[page_num]->get_box())).c_str();
	std::shared_ptr<Gtk::VBox> new_curr_vbox = _tab_contexts[page_num]->get_box();

	// When the notebook is empty, then scrolledwindow should not have a parent.
	if (_tab_ebox.get_parent() == 0) {
		new_curr_vbox->add(_tab_ebox);
		// _tab_ebox.add(_socket);

		_tab_ebox.add_events(Gdk::BUTTON_PRESS_MASK
								| Gdk::BUTTON_RELEASE_MASK
								| Gdk::BUTTON1_MOTION_MASK
								| Gdk::BUTTON2_MOTION_MASK);
		_tab_ebox.add_events(Gdk::POINTER_MOTION_MASK); // Needed for displaying mouse coords
		_tab_ebox.signal_button_press_event().connect(sigc::mem_fun(*this, &EditorWin::on_tab_button_press_event));
		_tab_ebox.signal_button_release_event().connect(sigc::mem_fun(*this, &EditorWin::on_tab_button_release_event));
		_tab_ebox.signal_motion_notify_event().connect(sigc::mem_fun(*this, &EditorWin::on_tab_button_motion_pressed));

		// /////////////////////////////////////
		// TEST
		// _tab_ebox.signal_state_flags_changed().connect(sigc::mem_fun(*this, &EditorWin::on_state_changed));
		// add_events(Gdk::POINTER_MOTION_MASK);
		// _tab_ebox.signal_motion_notify_event().connect(sigc::mem_fun(*this, &EditorWin::on_motion_notify_event));
		// signal_motion_notify_event().connect(sigc::mem_fun(*this, &EditorWin::on_motion_notify_event), false);
		//    _tab_ebox.signal_motion_notify_event().connect(sigc::mem_fun(*this, &EditorWin::on_tab_motion));
		// /////////////////////////////////////
	}
	// If it did have a parent, just reparent to the new tab.
	else {
		std::cout << "INFO: editorwin.cc: on_my_switch_page: Reparent editor tab." << std::endl;
		_tab_ebox.reparent(*new_curr_vbox);
	}

	// Determine size of window (and later also SDL display)
	int tx = 0, ty = 0, tw = 0, th = 0;
	if (_tab_ebox.get_window())
		_tab_ebox.get_window()->get_geometry(tx, ty, tw, th);
	else
		std::cerr << "ERROR: editorwin:.cc: Could not get EventBox's window. This is serious.\n";

	if (_sdleditor != NULL) {
		delete _sdleditor;
		_sdleditor = NULL;
	}

	// Create new editor, depending on whether the map is indoors or
	// outdoors, and connect it to the map accordingly.
	std::shared_ptr<Map> curr_map; //  = NULL;
	try {
		curr_map = World::Instance().get_map(new_curr_map_name.c_str());
	}
	catch (const MapNotFound& e) {
		std::cerr << e.print() << std::endl;
		return;
	}
	if (curr_map->is_outdoors()) {
		_sdleditor = SDLEditor::create("outdoors", new_curr_map_name);
		if (_indoors_icon_pic.get_parent() == &_swin_icons_hbox) {
			_swin_icons_hbox.remove(_indoors_icon_pic);
			_swin_icons_hbox.pack_start(_outdoors_icon_pic, Gtk::PACK_SHRINK);
		}
		_tb_show_obj.set_sensitive(false);
		_rb_fill_map.set_sensitive(false);
		_rb_draw_obj.set_sensitive(false);
		_rb_del_obj.set_sensitive(false);
		_rb_draw_map.set_active(true);
	}
	else {
		_sdleditor = SDLEditor::create("indoors", new_curr_map_name);
		if (_outdoors_icon_pic.get_parent() == &_swin_icons_hbox) {
			_swin_icons_hbox.remove(_outdoors_icon_pic);
			_swin_icons_hbox.pack_start(_indoors_icon_pic, Gtk::PACK_SHRINK);
		}
		_tb_show_obj.set_sensitive(true);
		_rb_draw_obj.set_sensitive(true);
		_rb_fill_map.set_sensitive(true);
		_rb_del_obj.set_sensitive(true);
		_rb_add_action.set_sensitive(true);
	}

	if (_sdleditor == NULL) {
		std::cerr << "ERROR: editorwin.cc: Could not allocate memory for editor." << std::endl;
		return;
	}
	else
		std::cout << "INFO: editorwin.cc: SDLEditor created.\n";

	_sdleditor->set_show_map(_tb_show_map.get_active());
	_sdleditor->set_show_obj(_tb_show_obj.get_active());
	_sdleditor->set_show_act(_tb_show_act.get_active());

	// Restore offsets
	if (get_curr_map()->is_outdoors()) {
		int ts = (int)World::Instance().get_outdoors_tile_size() - 1;
		_sdleditor->adjust_offsets(context()->get_yoffset()*ts, 0,      // top, bot
				                   context()->get_xoffset()*(ts-9), 0); // left, right
	}
	else {
		int ts = ((int)World::Instance().get_indoors_tile_size() + (grid_on()? 1 : 0));
		_sdleditor->adjust_offsets(context()->get_yoffset()*ts, 0,  // top, bot
					               context()->get_xoffset()*ts, 0); // left, right
	}

	_tab_ebox.show_all();
	this->queue_draw();

	_sdleditor->set_grid(grid_on());
	_sdleditor->open_display(&_tab_ebox, tw, th);
}

bool EditorWin::add_sdleditor_tab(const std::string tab_name)
{
	std::cout << "INFO: editorwin.cc: About to add SDLEditor tab." << std::endl;

	_tab_contexts.push_back(std::make_shared<TabContext>());
	_nb_main.append_page(*(_tab_contexts.back()->get_box()), tab_name);

	// Not sure, if this is the right place or thing to do.  Check!
	// If this is the first page to be added, connect signal.
	if (_nb_main.get_n_pages() == 1)
		_nb_main.signal_switch_page().connect(sigc::mem_fun(*this, &EditorWin::on_my_switch_page));

	show_all_children();
	_nb_main.set_current_page(_nb_main.get_n_pages() - 1);

	// Opening of a map when no map is currently open:
	if (_swin_icons.get_parent() == 0) {
		_swin_icons.set_size_request(-1, 200);
		// hbox.add(vtoolbox);  Seems this is already done above!
		_vtoolbox.pack_start(_rb_draw_map, Gtk::PACK_SHRINK);
		_vtoolbox.pack_start(_rb_fill_map, Gtk::PACK_SHRINK);
		_vtoolbox.pack_start(_rb_draw_obj, Gtk::PACK_SHRINK);
		_vtoolbox.pack_start(_rb_del_obj, Gtk::PACK_SHRINK);
		_vtoolbox.pack_start(_rb_add_action, Gtk::PACK_SHRINK);
		_vtoolbox.pack_start(_rb_del_action, Gtk::PACK_SHRINK);
		_vtoolbox.pack_start(_coords_lbl, Gtk::PACK_SHRINK);
		_vbox.pack_start(_togglehbox, Gtk::PACK_SHRINK);
		_vbox.pack_start(_swin_icons, Gtk::PACK_SHRINK);
		_swin_icons.add(_swin_icons_hbox);

		try {
			if (!World::Instance().get_map(tab_name.c_str())->is_outdoors())
				_swin_icons_hbox.pack_start(_indoors_icon_pic, Gtk::PACK_SHRINK);
			else
				_swin_icons_hbox.pack_start(_outdoors_icon_pic, Gtk::PACK_SHRINK);
		}
		catch (const MapNotFound& e) {
			std::cerr << e.print() << std::endl;
			return false;
		}

		_swin_icons.add_events(Gdk::BUTTON_PRESS_MASK);
		_swin_icons.signal_button_press_event().connect(sigc::mem_fun(*this, &EditorWin::on_swindow_button_press_event));
	}
	// A map is already open... just change the displayed icon box in the bottom instead of creating it.
	else {
		try {
			if (World::Instance().get_map(tab_name.c_str())->is_outdoors()) {
				if (_indoors_icon_pic.get_parent() == &_swin_icons_hbox) {
					_swin_icons_hbox.remove(_indoors_icon_pic);
					_swin_icons_hbox.pack_start(_outdoors_icon_pic, Gtk::PACK_SHRINK);
				}
			}
			else {
				if (_outdoors_icon_pic.get_parent() == &_swin_icons_hbox) {
					_swin_icons_hbox.remove(_outdoors_icon_pic);
					_swin_icons_hbox.pack_start(_indoors_icon_pic, Gtk::PACK_SHRINK);
				}
			}
		}
		catch (const MapNotFound& e) {
			std::cerr << e.print() << std::endl;
			return false;
		}
	}

	show_all_children();
	_ref_actiongr->get_action("MapMenuClose")->set_sensitive(true);
	_ref_actiongr->get_action("MapMenuExpand")->set_sensitive(true);
	_ref_actiongr->get_action("FileMenuSave")->set_sensitive(true);

	return true;
}

void EditorWin::set_grid(bool state)
{
	_show_grid = state;
}

bool EditorWin::grid_on(void) const
{
	return _show_grid;
}

std::shared_ptr<Map> EditorWin::get_curr_map(void) const
{
	if (_sdleditor == NULL) {
		std::cerr << "WARNING: editorwin.cc: map will be NULL as _sdleditor is NULL.\n";
		return NULL;
	}

	return _sdleditor->get_map();
}

std::shared_ptr<TabContext> EditorWin::context(void) const
{
	if (_tab_contexts.size() > 0  &&  _nb_main.get_current_page() < (int)(_tab_contexts.size()))
		return _tab_contexts.at(_nb_main.get_current_page());
	throw std::invalid_argument("Cannot access _tab_contexts' page, if page number exceeds the size of _tab_contexts.");
}
