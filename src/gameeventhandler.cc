#include "sdlwindow.hh"
#include "gameeventhandler.hh"
#include "gameevent.hh"
#include "gamestate.hh"
#include "map.hh"
#include "mapobj.hh"
#include "indoorsmap.hh"
#include "eventermap.hh"
#include "eventprintcon.hh"
#include "eventluascript.hh"
#include "eventdeleteobj.hh"
#include "gamecontrol.hh"
#include "miniwin.hh"
#include "world.hh"
#include "soundsample.hh"
#include "simplicissimus.hh"
#include "luaapi.hh"
#include "luawrapper.hh"
#include "config.h"

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <memory>

#include <boost/filesystem.hpp>

GameEventHandler::GameEventHandler()
{
}

bool GameEventHandler::handle(std::shared_ptr<GameEvent> event, std::shared_ptr<Map> map, MapObj* obj)
{
	if (std::dynamic_pointer_cast<EventEnterMap>(event))
		return handle_event_enter_map(std::dynamic_pointer_cast<EventEnterMap>(event), map);
	else if (std::dynamic_pointer_cast<EventPrintcon>(event))
		return handle_event_printcon(std::dynamic_pointer_cast<EventPrintcon>(event), map);
	else if (std::dynamic_pointer_cast<EventPlaySound>(event))
		return handle_event_playsound(std::dynamic_pointer_cast<EventPlaySound>(event), map);
	else if (std::dynamic_pointer_cast<EventLuaScript>(event))
		return handle_event_lua_script(std::dynamic_pointer_cast<EventLuaScript>(event), map);
	else if (std::dynamic_pointer_cast<EventChangeIcon>(event))
		return handle_event_change_icon(std::dynamic_pointer_cast<EventChangeIcon>(event), map);
	else if (std::dynamic_pointer_cast<EventDeleteObject>(event))
		return handle_event_delete_object(map, obj);
	else
		std::cerr << "Error: Not handling UNKNOWN EVENT\n";

	return false;
}

// Deletes one (not all!) object from map; see pop_obj() for details.

bool GameEventHandler::handle_event_delete_object(std::shared_ptr<Map> map, MapObj* obj)
{
	if (obj == NULL) {
		std::cerr << "ERROR: gameeventhandler.cc: Trying to delete NULL-object.\n";
		return false;
	}

	unsigned x, y;
	obj->get_coords(x, y);
	map->pop_obj((int)x, (int)y);

	return true;
}

bool GameEventHandler::handle_event_lua_script(std::shared_ptr<EventLuaScript> event, std::shared_ptr<Map> map)
{
	if (luaL_dofile(_lua_state, (conf_world_path / "maps" / event->file_name).c_str())) {
		std::cerr << "ERROR: gameeventhandler.cc: Couldn't execute lua file: " << (conf_world_path / "maps" / event->file_name).string() << "\n";
		return false;
	}

	return true;
}

bool GameEventHandler::handle_event_change_icon(std::shared_ptr<EventChangeIcon> event, std::shared_ptr<Map> map)
{
	if (map->get_tile(event->x, event->y) == event->icon_now)
		return map->set_tile(event->x, event->y, event->icon_new) == 0;

	if (map->get_tile(event->x, event->y) == event->icon_new)
		return map->set_tile(event->x, event->y, event->icon_now) == 0;

	return false;
}

bool GameEventHandler::handle_event_printcon(std::shared_ptr<EventPrintcon> event, std::shared_ptr<Map> map)
{
	GameControl::Instance().printcon(event->text);
	return true;
}

// TODO: Variable map currently not used. Can be null.

bool GameEventHandler::handle_event_playsound(std::shared_ptr<EventPlaySound> event, std::shared_ptr<Map> map)
{
	static SoundSample sample;  // If this isn't static, then the var
	                            // gets discarded before the sample has
                                // finished playing
	boost::filesystem::path samples_path((std::string)DATADIR);
	samples_path = samples_path / PACKAGE_NAME / "data" / World::Instance().get_name() / "sound";

	sample.play((samples_path / event->filename).c_str());
	return true;
}

bool GameEventHandler::handle_event_enter_map(std::shared_ptr<EventEnterMap> event, std::shared_ptr<Map> map)
{
	Party* party = &Party::Instance();
	GameControl* gc = &GameControl::Instance();
	MiniWin* mw = &MiniWin::Instance();

	// Before changing map, when indoors (e.g. climb down a ladder), store state
	if (party->indoors()) {
		std::shared_ptr<Map> new_map = map;
		IndoorsMap tmp_map = *(std::dynamic_pointer_cast<IndoorsMap>(new_map).get()); // Use it to create IndoorsMap (i.e., deep copy of Map())
		std::shared_ptr<IndoorsMap> ind_map = std::make_shared<IndoorsMap>(tmp_map); // Create a shared_ptr of IndoorsMap
		GameState::Instance().add_map(ind_map); // Add to GameState; TODO: What happens when tmp_map falls off the stack? Is the shared_ptr still valid?
	}

	// TODO: It is not nice to create an entire map just to test for a flag, but it works for now...
	{
		std::shared_ptr<Map> tmp_map = World::Instance().get_map(event->get_map_name().c_str());
		IndoorsMap tmp_map2 = *((IndoorsMap*)tmp_map.get()); // Create deep copy of map because otherwise xml_load_data fucks up the map's state
		tmp_map2.xml_load_map_data();
		if (tmp_map2.guarded_city &&
				(gc->get_clock()->tod() == NIGHT || gc->get_clock()->tod() == EARLY_MORNING || gc->get_clock()->tod() == MIDNIGHT))
		{
			gc->printcon("No entry. At this ungodly hour, " + event->get_map_name() + " is under lock and key.");
			gc->do_turn();
			return true;
		}
	}

	gc->printcon("Entering " + event->get_map_name());

	boost::filesystem::path tmp_path((std::string)DATADIR);
	tmp_path /= tmp_path / (std::string)PACKAGE_NAME / "data" / World::Instance().get_name() / "images" / "indoors_city.png";
	mw->save_surf();
	mw->surface_from_file(tmp_path.string());

	if (!party->indoors())
		party->store_outside_coords();

	map->unload_map_data();
	gc->get_arena() = NULL;

	// There is only one landscape which can not be entered, but
	// rather an indoors map may be left to it.  So it's safe to
	// assume every enterable map is indoors.
	gc->set_arena(Arena::create("indoors", event->get_map_name()));

	if (gc->get_arena() == NULL) {
		std::cerr << "ERROR: gameeventhandler.cc::action_on_enter(): Arena NULL.\n";
		std::cerr << "ERROR: gameeventhandler.cc::action_on_enter(): Map name: " << event->get_map_name() << ".\n";
		exit(-1);
	}
	if (gc->get_arena()->get_map() == NULL)
		std::cerr << "ERROR: gameeventhandler.cc::action_on_enter(): arena->get_map NULL.\n";

	// If a map is stored inside the current game status, it means the player modified it in the past,
	// and we should load it instead of loading it from the original game files.
	boost::filesystem::path dir((std::string(getenv("HOME")) + "/.simplicissimus/" + World::Instance().get_name() + "/maps/"));
	std::string old_map_file = dir.string() + gc->get_arena()->get_map()->get_name() + ".xml";

	std::shared_ptr<IndoorsMap> saved_map = GameState::Instance().get_map(gc->get_arena()->get_map()->get_name());
	if (saved_map != NULL) {
		// std::cout << "Using gamestate map.\n";
		gc->get_arena()->set_map(saved_map);
	}
	else if (boost::filesystem::exists(old_map_file)) {
		// std::cout << "Using save game map.\n";
		gc->get_arena()->get_map()->xml_load_map_data(old_map_file);
	}
	else {
		// std::cout << "Using fresh map.\n";
		gc->get_arena()->get_map()->xml_load_map_data();
	}

	gc->get_arena()->set_SDL_surface(SDLWindow::Instance().get_drawing_area_SDL_surface());
	gc->get_arena()->determine_offsets();

	// The following is somewhat yucky code around the fact that you cannot place the party
	// just inside an arbitrary location in the map. So I put it on (1,1) and then use move
	// commands in quick succession to the actual location the party should be in.

	party->set_indoors(true);
	gc->get_arena()->show_map();
	gc->set_party(event->get_x(), event->get_y());
	gc->set_party(1, 1);

	for (unsigned x = 1; x < event->get_x(); x++)
		gc->move_party_quietly(DIR_RIGHT, true);
	for (unsigned y = 1; y < event->get_y(); y++)
		gc->move_party_quietly(DIR_DOWN, true);

	return true;
}
