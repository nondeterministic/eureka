// ----------------------------------------------------------------
// leibniz
//
// Copyright (c) 2009  Andreas Bauer <baueran@gmail.com>
//
// This is not free software.  See COPYING for further information.
// ----------------------------------------------------------------

#include <iostream>
#include <memory>
#include <cmath>
#include <strstream>
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
  rb_draw_map("Draw map"),
  rb_fill_map("Fill map"),
  rb_draw_obj("Draw object"),
  rb_del_obj("Delete object"),
  rb_add_action("Add action/event..."),
  rb_del_action("Delete action"),
  tb_show_map("Show map"),
  tb_show_obj("Show objects"),
  tb_show_act("Show actions"),
  coords_lbl("Dumm")
{
  _show_grid = true;
  _drag = false;

  // Initially there is no editor attached.
  _sdleditor = NULL;

  // Not sure, if this is the right place or thing to do.  Check!
  nb_main.signal_switch_page().connect(sigc::mem_fun(*this, &EditorWin::on_my_switch_page));

  signal_configure_event().connect(sigc::mem_fun(*this, &EditorWin::on_my_configure_event), false);

  add_events(Gdk::KEY_PRESS_MASK | Gdk::KEY_RELEASE_MASK);
  signal_key_press_event().connect(sigc::mem_fun(*this, &EditorWin::on_my_key_press_event), false);

  //#ifndef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
  signal_draw().connect(sigc::mem_fun(*this, &EditorWin::on_my_sig_expose));
  //#endif

  // Now, on to standard stuff, i.e., GUI setup
  set_title("Leibniz - " + World::Instance().get_name());

  set_default_size(400,300);

  add(vbox);

  //Create actions for menus and toolbars:
  ref_actiongr = Gtk::ActionGroup::create();

  ref_actiongr->add(Gtk::Action::create("FileMenu", "File"));

  ref_actiongr->add(Gtk::Action::create("FileMenuNew",
                                        Gtk::Stock::NEW,
                                        "New world..."));
  ref_actiongr->add(Gtk::Action::create("FileMenuSave",
                                        Gtk::Stock::SAVE,
                                        "Save current world"),
                    sigc::mem_fun(*this, &EditorWin::on_menu_file_save));
  ref_actiongr->add(Gtk::Action::create("FileMenuQuit",
                                        Gtk::Stock::QUIT),
                    sigc::mem_fun(*this, &EditorWin::on_menu_file_quit));

  ref_actiongr->add(Gtk::Action::create("MapMenu", "Map"));

  ref_actiongr->add(Gtk::Action::create("MapMenuNew",
                                        Gtk::Stock::NEW,
                                        "_Create new map..."),
                    Gtk::AccelKey("<alt>C"),
                    sigc::mem_fun(*this, &EditorWin::on_menu_map_new));
  ref_actiongr->add(Gtk::Action::create("MapMenuOpen",
                                        Gtk::Stock::OPEN,
                                        "_Open existing map.."),
                    sigc::mem_fun(*this, &EditorWin::on_menu_map_open));
  ref_actiongr->add(Gtk::Action::create("MapMenuExpand",
                                        Gtk::Stock::FULLSCREEN,
                                        "Change map size..."),
                    sigc::mem_fun(*this, &EditorWin::on_menu_map_expand));
  ref_actiongr->add(Gtk::Action::create("MapMenuClose",
                                        Gtk::Stock::CLOSE,
                                        "_Close map"),
                    Gtk::AccelKey("<ctrl>W"),
                    sigc::mem_fun(*this, &EditorWin::on_menu_map_close));

  ref_actiongr->get_action("FileMenuNew")->set_sensitive(false);
  ref_actiongr->get_action("FileMenuSave")->set_sensitive(false);

  if (World::Instance().get_maps()->size() == 0)
    ref_actiongr->get_action("MapMenuOpen")->set_sensitive(false);

  if (get_curr_map() == NULL)
    {
      ref_actiongr->get_action("MapMenuClose")->set_sensitive(false);
      ref_actiongr->get_action("MapMenuExpand")->set_sensitive(false);
    }

  ref_uimgr = Gtk::UIManager::create();
  ref_uimgr->insert_action_group(ref_actiongr);

  add_accel_group(ref_uimgr->get_accel_group());

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
  try
    {
      ref_uimgr->add_ui_from_string(ui_info);
    }
  catch(const Glib::Error& ex)
    {
      std::cerr << "building menus failed: " <<  ex.what();
    }
#else
  std::auto_ptr<Glib::Error> ex;
  ref_uimgr->add_ui_from_string(ui_info, ex);
  if(ex.get())
    {
      std::cerr << "building menus failed: " <<  ex->what();
    }
#endif //GLIBMM_EXCEPTIONS_ENABLED

  // Get the menubar and toolbar widgets, and add them to a container widget
  Gtk::Widget* ptr_menubar = ref_uimgr->get_widget("/MenuBar");
  if (ptr_menubar)
    vbox.pack_start(*ptr_menubar, Gtk::PACK_SHRINK);

  // vbox.add(nb_main);
  vbox.add(hbox);
  hbox.pack_start(vtoolbox, Gtk::PACK_SHRINK);
  rb_draw_map.set_active();
  rb_draw_map.set_mode(false);
  rb_fill_map.set_active();
  rb_fill_map.set_mode(false);
  rb_draw_obj.set_mode(false);
  rb_del_obj.set_mode(false);
  rb_add_action.set_mode(false);
  rb_del_action.set_mode(false);
  toolbox_gr = rb_draw_map.get_group();
  rb_fill_map.set_group(toolbox_gr);
  rb_draw_obj.set_group(toolbox_gr);
  rb_del_obj.set_group(toolbox_gr);
  rb_add_action.set_group(toolbox_gr);
  rb_del_action.set_group(toolbox_gr);
  hbox.add(nb_main);
  tb_show_map.set_active();
  tb_show_obj.set_active();
  tb_show_act.set_active();
  togglehbox.pack_start(tb_show_map, Gtk::PACK_SHRINK);
  togglehbox.pack_start(tb_show_obj, Gtk::PACK_SHRINK);
  togglehbox.pack_start(tb_show_act, Gtk::PACK_SHRINK);
  tb_show_map.signal_toggled().connect(sigc::mem_fun(*this, &EditorWin::on_tb_show_map_toggled));
  tb_show_obj.signal_toggled().connect(sigc::mem_fun(*this, &EditorWin::on_tb_show_obj_toggled));
  tb_show_act.signal_toggled().connect(sigc::mem_fun(*this, &EditorWin::on_tb_show_act_toggled));

  show_all_children();
}

EditorWin::~EditorWin(void)
{
  // TODO: Free any memory here?!
}

void EditorWin::on_tb_show_map_toggled(void)
{
  if (_sdleditor != NULL)
    _sdleditor->set_show_map(tb_show_map.get_active());
}

void EditorWin::on_tb_show_obj_toggled(void)
{
  if (_sdleditor != NULL)
    _sdleditor->set_show_obj(tb_show_obj.get_active());
}

void EditorWin::on_tb_show_act_toggled(void)
{
  if (_sdleditor != NULL)
    _sdleditor->set_show_act(tb_show_act.get_active());
}

void EditorWin::on_menu_file_save(void)
{
  for (auto curr_map = World::Instance().get_maps()->begin(); curr_map != World::Instance().get_maps()->end(); curr_map++)
    if ((*curr_map)->modified())
      if ((*curr_map)->xml_write_map_data())
        (*curr_map)->set_notmodified();

  World::Instance().xml_write_world_data();
  ref_actiongr->get_action("FileMenuSave")->set_sensitive(false);
}

void EditorWin::on_menu_file_quit(void)
{
  if (_sdleditor != NULL)
    delete _sdleditor;
  hide();
}

void EditorWin::on_menu_map_new(void)
{
  NewMapWin newmap_win;
  Gtk::Main::run(newmap_win);

  if (!newmap_win.get_name_entry().empty())
    add_sdleditor_tab(newmap_win.get_name_entry().c_str());
}

void EditorWin::on_menu_map_open(void)
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

void EditorWin::on_menu_map_close(void)
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
  if (nb_main.get_n_pages() > 1) {
    // As the current context returned by context() depends on the
    // current page, we create a pointer to the current context.
    TabContext* tmp_context = context();
    Gtk::Box* tmp_vbox = tmp_context->get_box();

    std::cout << "There are " << _tab_contexts.size() << " contexts."
              << std::endl;
    // Remove the tabcontext from the context vector
    for (std::vector<TabContext>::iterator
           curr_context = _tab_contexts.begin();
         curr_context != _tab_contexts.end();
         curr_context++) {
      if (&(*curr_context) == tmp_context) {
        _tab_contexts.erase(curr_context);
        std::cout << "Removed tabcontext from vector" << std::endl;
        break;
      }
    }
    std::cout << "There are " << _tab_contexts.size() << " contexts."
              << std::endl;

    if (_sdleditor != NULL) {
      // Store pointer to map data temporarily
      std::shared_ptr<Map> tmp_map = get_curr_map();

      // Free some RAM
      tmp_map->unload_map_data();

      delete _sdleditor;
      _sdleditor = NULL;
      std::cout << "Deleted sdleditor" << std::endl;

      // If the map has never been saved before, remove it for
      // good
      if (!tmp_map->exists_on_disk())
        World::Instance().delete_map(tmp_map);
    }

    if (nb_main.get_current_page() == 0)
      nb_main.next_page();
    else
      nb_main.prev_page();

    // Now remove vbox from current page.  Note that this may cause
    // an automatic switch if tmp_vbox is the current one.
    nb_main.remove(*tmp_vbox);
    std::cout << "Removed page containing vbox" << std::endl;

    // No need to delete vbox as the vbox will be reused for the
    // remaining opened tab(s).  In fact, deleting would be a very,
    // very bad idea!
  }
  // This case should really never occur...
  else if (nb_main.get_n_pages() == 0)
    return;
  // This is the case when exactly one editor is currently open.
  else {
    // First remove the socket from the eventbox We don't have to
    // specify which as an ebox can only hold one object.
    _tab_ebox.remove();
    std::cout << "Removed socket from eventbox" << std::endl;

    // Remove eventbox from current vbox
    context()->get_box()->remove(_tab_ebox);
    std::cout << "Removed eventbox from vbox" << std::endl;

    // As the current context returned by context() depends on the
    // current page, we create a pointer to the current context.
    TabContext* tmp_context = context();

    // Now remove vbox from current page
    nb_main.remove(*(context()->get_box()));
    std::cout << "Removed page containing vbox" << std::endl;

    std::cout << "There are " << _tab_contexts.size() << " contexts."
              << std::endl;
    // Remove the tabcontext from the context vector
    for (std::vector<TabContext>::iterator
           curr_context = _tab_contexts.begin();
         curr_context != _tab_contexts.end();
         curr_context++) {
      if (&(*curr_context) == tmp_context) {
        // Delete the vbox as no other editor needs it.
        delete curr_context->get_box();
        std::cout << "Removed vbox" << std::endl;

        _tab_contexts.erase(curr_context);
        std::cout << "Removed tabcontext from vector" << std::endl;
        break;
      }
    }
    std::cout << "There are " << _tab_contexts.size() << " contexts."
              << std::endl;

    if (_sdleditor != NULL) {
      // Store pointer to map data temporarily
      std::shared_ptr<Map> tmp_map = get_curr_map();

      // Free some RAM
      tmp_map->unload_map_data();

      delete _sdleditor;
      _sdleditor = NULL;
      std::cout << "Deleted sdleditor" << std::endl;

      // If the map has never been saved before, remove it for good
      if (!tmp_map->exists_on_disk())
        World::Instance().delete_map(tmp_map);
    }

    // Now also remove scrolledwindow as there is no more tabcontext
    // where the user can store a current brush in.  Clicking into
    // the scrolledwindow will in fact crash.
    _swin_icons.hide();
    togglehbox.hide();
    vtoolbox.hide();

    ref_actiongr->get_action("MapMenuClose")->set_sensitive(false);
    ref_actiongr->get_action("MapMenuExpand")->set_sensitive(false);
  }

  std::vector<std::shared_ptr<Map>>* vmaps = World::Instance().get_maps();
  for (auto curr_map = vmaps->begin(); curr_map != vmaps->end(); curr_map++) {
    if ((*curr_map)->exists_on_disk()) {
      // There is now at least one unopened map that can be
      // reopened, hence activate menu entry for that
      ref_actiongr->get_action("MapMenuOpen")->set_sensitive(true);
      break;
    }
  }
}

// Note, negative expansion deletes rows/columns.

void EditorWin::on_menu_map_expand(void)
{
  ExpandMapWin* expandmap_win =
    new ExpandMapWin("Change map size",
                     "How many rows/columns do you want to add?");

  if (expandmap_win->run())
    {
      int top, bot, right, left;
      expandmap_win->get_values(top, bot, right, left);
      _sdleditor->get_map()->expand_map(top, bot, right, left);
      ref_actiongr->get_action("FileMenuSave")->set_sensitive(true);
      this->queue_draw();
    }

  delete expandmap_win;
}

void EditorWin::determine_map_offsets(void)
{
  if (_tab_ebox.get_parent() == 0 || _sdleditor == NULL)
    return;

  bool is_outdoors = get_curr_map()->is_outdoors();

  unsigned ts =
    is_outdoors?
    (World::Instance().get_outdoors_tile_size() - 1) :
    (World::Instance().get_indoors_tile_size() + (grid_on()? 1 : 0));

  int tx = 0, ty = 0, tw = 0, th = 0;
  _tab_ebox.get_window()->get_geometry(tx, ty, tw, th);

  Offsets new_offsets =
    _sdleditor->determine_offsets((unsigned)tw, (unsigned)th);

  // Determine how many icons are hidden and store it in the
  // TabContext.
  context()->set_xoffset(new_offsets.left/(is_outdoors? (ts - 9) : ts));
  context()->set_yoffset(new_offsets.top/ts);
}

// User has button pressed AND moves the mouse in editor window

bool EditorWin::on_tab_button_motion_pressed(GdkEventMotion* event)
{
    int map_x = 0;
    int map_y = 0;
    _sdleditor->pixel_to_map(event->x, event->y, map_x, map_y);

	strstream ss;
	ss << "X: " << map_x << " y: " << map_y;

	coords_lbl.set_text(ss.str());

    if (event->state & Gdk::BUTTON1_MASK)
    	place_icon_on_map((int)event->x, (int)event->y);

	return false; // Not done.  Event will be further handled by GTKMM etc.
}

void EditorWin::place_icon_on_map(int x, int y)
{
	if (rb_draw_map.get_active())
		put_curr_tile_on_map(x, y);
	else if (rb_fill_map.get_active())
		fill_with_curr_tile(x, y);
	else if (rb_draw_obj.get_active())
		add_object(x, y);
	else if (rb_del_obj.get_active())
		rm_obj(x, y);
	else if (rb_add_action.get_active())
		add_action(x, y);
	else if (rb_del_action.get_active())
		del_action(x, y);
}

// User clicked in editor window

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

	return true;
}

bool EditorWin::on_tab_button_release_event(GdkEventButton* event)
{
	// Unset drag handle!
	_drag = false;

	return false;
}

/*
  bool EditorWin::on_motion_notify_event(GdkEventMotion* event)
  {
  if (!_drag)
  return false;

  if (rb_draw_map.get_active())
  put_curr_tile_on_map((int)event->x, (int)event->y);
  else if (rb_draw_obj.get_active())
  // put_curr_tile_on_map((int)event->x, (int)event->y);
  put_curr_tile_in_obj_register((int)event->x, (int)event->y);

  return true;
  }
*/

void EditorWin::fill_with_curr_tile(int x, int y)
{
	int map_x = 0;
	int map_y = 0;
	_sdleditor->pixel_to_map(x, y, map_x, map_y);
	unsigned curr_brush = context()->get_icon_brush_no();
	std::shared_ptr<Map> map = get_curr_map();

	fill(map, curr_brush, map->get_tile(map_x, map_y), (unsigned)map_x, (unsigned)map_y);
    ref_actiongr->get_action("FileMenuSave")->set_sensitive(true);
}

// Fills position map_x, map_y on the map, which currently has tile old_brush on it, with new_brush.

void EditorWin::fill(std::shared_ptr<Map> map, unsigned new_brush, unsigned old_brush, unsigned map_x, unsigned map_y)
{
	map->set_tile(map_x, map_y, new_brush);

	for (int xoff = -1; xoff < 2; xoff++)
	{
		for (int yoff = -1; yoff < 2; yoff++)
		{
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

// x and y are the current screen coordinates where the user clicked

void EditorWin::put_curr_tile_on_map(int x, int y)
{
  int map_x = 0;
  int map_y = 0;
  _sdleditor->pixel_to_map(x, y, map_x, map_y);
  unsigned curr_brush = context()->get_icon_brush_no();

  if ((unsigned)get_curr_map()->get_tile(map_x, map_y) != curr_brush) {
    if (get_curr_map()->set_tile(map_x, map_y, curr_brush) == 0) {
      ref_actiongr->get_action("FileMenuSave")->set_sensitive(true);
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
				std::cout << "Event not implemented yet!  :-(" << std::endl;
		}
		return;
	}

	AddActionWin action_win;
	if (action_win.run()) {
		switch (action_win.get_action()) {
		case ACT_ON_ENTER: {
			// ~Map() deletes the memory.
			std::shared_ptr<Action> new_act(new ActionOnEnter(map_x, map_y, "ACT_ON_ENTER"));
			std::cout << "Added action at (" << map_x << ", " << map_y << ")" << std::endl;
			get_curr_map()->add_action(new_act);
			break;
		}
		case ACT_ON_PULLPUSH: {
			std::shared_ptr<Action> new_act(new ActionPullPush(map_x, map_y, "ACT_ON_PULLPUSH"));
			std::cout << "Added action at (" << map_x << ", " << map_y << ")" << std::endl;
			get_curr_map()->add_action(new_act);
			break;
		}
		default:
			std::cout << "Action not implemented yet!  :-(" << std::endl;
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

  ref_actiongr->get_action("FileMenuSave")->set_sensitive(true);
  // This could be much more efficient if I would only draw the
  // icon just drawn, but f*ck it for now...
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
		newObj.removable = obj_win.removable;
		newObj.id = obj_win.id;

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

		ref_actiongr->get_action("FileMenuSave")->set_sensitive(true);
		this->queue_draw();
	}
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
	std::cout << "Icon: " << icon_x << ", " << icon_y << std::endl;

	// Set selected icon for dialog to display
	Glib::RefPtr<Gdk::Pixbuf> icon_pixbuf = (get_curr_map()->is_outdoors()?
			_outdoors_icon_pic.get_pixbuf() :
			_indoors_icon_pic.get_pixbuf());
	// Glib::RefPtr<Gdk::Pixbuf> chosen_icon_pb =
	_selected_icon_pb = Gdk::Pixbuf::create_subpixbuf(icon_pixbuf,
			icon_x,
			icon_y,
			icon_size,
			icon_size);

	// Select current icon brush
	if (event->button == 1)
		context()->set_icon_brush_no(icon_no_pressed);
	// Edit icon properties
	else if (event->button == 3) {
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

		if (icon_props_win.run()) {
			IconProps new_icon_props;
			new_icon_props.set_icon(icon_props_win.get_icon_no());
			new_icon_props.set_name(icon_props_win.get_name().c_str());
			new_icon_props._is_walkable  = icon_props_win.get_walkable();
			new_icon_props._trans = icon_props_win.get_trans();

			if (get_curr_map()->is_outdoors())
				OutdoorsIcons::Instance().add_props(new_icon_props);
			else
				IndoorsIcons::Instance().add_props(new_icon_props);

			ref_actiongr->get_action("FileMenuSave")->set_sensitive(true);
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
  nb_main.queue_draw_area(0, 0, 100, 100);

  return handled;
}

// READ: http://osdir.com/ml/gnome.gtkmm/2003-08/msg00186.html
bool EditorWin::on_my_configure_event(GdkEventConfigure* event)
{
  if (_tab_ebox.get_parent() != 0 && _sdleditor != NULL)
    {
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

  // Returning true here is EXTREMELY important!
  //
  // It means that the expose event has been handled and is not passed
  // on.  If false is returned, we redraw, and keep the event in the
  // queue, after a while that leads to a relatively weird crash with
  // an X-server error message.
  return true;
}

void EditorWin::on_my_switch_page(Gtk::Widget* page, guint page_num)
{
  std::string new_curr_map_name =
    nb_main.get_tab_label_text(*(_tab_contexts[page_num].get_box())).c_str();

  Gtk::Box* new_curr_vbox = _tab_contexts[page_num].get_box();
  std::cout << "Retrieved current vbox for editor tab" << std::endl;

  // When the notebook is empty, then scrolledwindow should not have a
  // parent.
  if (_tab_ebox.get_parent() == 0) {
    std::cout << "Set up editor tab..." << std::endl;
    new_curr_vbox->add(_tab_ebox);
    std::cout << "Adding socket..." << std::endl;
    _tab_ebox.add(_socket);

    _tab_ebox.add_events(Gdk::BUTTON_PRESS_MASK
                         | Gdk::BUTTON_RELEASE_MASK
                         | Gdk::BUTTON1_MOTION_MASK
                         | Gdk::BUTTON2_MOTION_MASK);
    _tab_ebox.add_events(Gdk::POINTER_MOTION_MASK);
    _tab_ebox.signal_button_press_event().connect(sigc::mem_fun(*this, &EditorWin::on_tab_button_press_event));
    _tab_ebox.signal_button_release_event().connect(sigc::mem_fun(*this, &EditorWin::on_tab_button_release_event));
    _tab_ebox.signal_motion_notify_event().connect(sigc::mem_fun(*this, &EditorWin::on_tab_button_motion_pressed));
    //    _tab_ebox.signal_motion_notify_event().connect(sigc::mem_fun(*this, &EditorWin::on_tab_motion));
  }
  // If it did have a parent, just reparent to the new tab.
  else {
    std::cout << "Reparent editor tab..." << std::endl;
    _tab_ebox.reparent(*new_curr_vbox);
  }

  std::cout << "Done setting up tab... Now setting up editor." << std::endl;

  // Determine size of window (and later also SDL display)
  int tx = 0, ty = 0, tw = 0, th = 0;
  _tab_ebox.get_window()->get_geometry(tx, ty, tw, th);

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
    tb_show_obj.set_sensitive(false);
    rb_fill_map.set_sensitive(false);
    rb_draw_obj.set_sensitive(false);
    rb_del_obj.set_sensitive(false);
    rb_draw_map.set_active(true);
  }
  else {
    _sdleditor = SDLEditor::create("indoors", new_curr_map_name);
    if (_outdoors_icon_pic.get_parent() == &_swin_icons_hbox) {
      _swin_icons_hbox.remove(_outdoors_icon_pic);
      _swin_icons_hbox.pack_start(_indoors_icon_pic, Gtk::PACK_SHRINK);
    }
    tb_show_obj.set_sensitive(true);
    rb_draw_obj.set_sensitive(true);
    rb_fill_map.set_sensitive(true);
    rb_del_obj.set_sensitive(true);
    rb_add_action.set_sensitive(true);
  }

  if (_sdleditor == NULL) {
    std::cerr << "Could not allocate memory for editor." << std::endl;
    return;
  }
  else
    std::cerr << "Allocated memory for editor." << std::endl;

  _sdleditor->set_show_map(tb_show_map.get_active());
  _sdleditor->set_show_obj(tb_show_obj.get_active());
  _sdleditor->set_show_act(tb_show_act.get_active());

  std::cout << "Restoring offsets..." << std::endl;

  // Restore offsets
  if (get_curr_map()->is_outdoors()) {
    unsigned ts = World::Instance().get_outdoors_tile_size() - 1;
    _sdleditor->adjust_offsets(context()->get_yoffset()*ts, 0,  // top, bot
                               context()->get_xoffset()*(ts-9), 0); // left, right
  }
  else {
    unsigned ts =
      (World::Instance().get_indoors_tile_size() + (grid_on()? 1 : 0));
    _sdleditor->adjust_offsets(context()->get_yoffset()*ts, 0,  // top, bot
                               context()->get_xoffset()*ts, 0); // left, right
  }

  _sdleditor->set_grid(grid_on());
  _sdleditor->open_display(&_socket, tw, th);

  this->queue_draw();
}

bool EditorWin::add_sdleditor_tab(const char* tab_name)
{
  std::cout << "Adding tab..." << std::endl;

  try {
    TabContext new_tab_context(new Gtk::VBox());
    _tab_contexts.push_back(new_tab_context);
  }
  catch(const std::exception& e) {
    return false;
  }

  nb_main.append_page(*(_tab_contexts.back().get_box()), tab_name);
  show_all_children();
  nb_main.set_current_page(nb_main.get_n_pages() - 1);

  // Opening of a map when no map is currently open:
  if (_swin_icons.get_parent() == 0) {
    _swin_icons.set_size_request(-1, 200);
    // hbox.add(vtoolbox);  Seems this is already done above!
    vtoolbox.pack_start(rb_draw_map, Gtk::PACK_SHRINK);
    vtoolbox.pack_start(rb_fill_map, Gtk::PACK_SHRINK);
    vtoolbox.pack_start(rb_draw_obj, Gtk::PACK_SHRINK);
    vtoolbox.pack_start(rb_del_obj, Gtk::PACK_SHRINK);
    vtoolbox.pack_start(rb_add_action, Gtk::PACK_SHRINK);
    vtoolbox.pack_start(rb_del_action, Gtk::PACK_SHRINK);
    vtoolbox.pack_start(coords_lbl, Gtk::PACK_SHRINK);
    vbox.pack_start(togglehbox, Gtk::PACK_SHRINK);
    vbox.pack_start(_swin_icons, Gtk::PACK_SHRINK);
    _swin_icons.add(_swin_icons_hbox);

    try {
      if (!World::Instance().get_map(tab_name)->is_outdoors())
        _swin_icons_hbox.pack_start(_indoors_icon_pic, Gtk::PACK_SHRINK);
      else
        _swin_icons_hbox.pack_start(_outdoors_icon_pic, Gtk::PACK_SHRINK);
    }
    catch (const MapNotFound& e) {
      std::cerr << e.print() << std::endl;
      return false;
    }

    _swin_icons.add_events(Gdk::BUTTON_PRESS_MASK);
    _swin_icons.
      signal_button_press_event().
      connect(sigc::mem_fun(*this, &EditorWin::on_swindow_button_press_event));
  }
  // A map is already open... just change the displayed icon box in
  // the bottom instead of creating it.
  else {
    try {
      if (World::Instance().get_map(tab_name)->is_outdoors()) {
        if (_indoors_icon_pic.get_parent() == &_swin_icons_hbox) {
          _swin_icons_hbox.remove(_indoors_icon_pic);
          _swin_icons_hbox.pack_start(_outdoors_icon_pic, Gtk::PACK_SHRINK);
        }
      }
      else {
        if (_outdoors_icon_pic.get_parent() == &_swin_icons_hbox)
          {
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
  ref_actiongr->get_action("MapMenuClose")->set_sensitive(true);
  ref_actiongr->get_action("MapMenuExpand")->set_sensitive(true);
  ref_actiongr->get_action("FileMenuSave")->set_sensitive(true);
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
  if (_sdleditor == NULL)
    return NULL;

  return _sdleditor->get_map();
}

TabContext* EditorWin::context(void)
{
  return &_tab_contexts[nb_main.get_current_page()];
}
