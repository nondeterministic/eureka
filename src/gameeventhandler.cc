// This source file is part of eureka
//
// Copyright (c) 2007-2016  Andreas Bauer <baueran@gmail.com>
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
#include "eventaddobj.hh"
#include "eventleavemap.hh"
#include "gamecontrol.hh"
#include "miniwin.hh"
#include "world.hh"
#include "soundsample.hh"
#include "eureka.hh"
#include "luaapi.hh"
#include "luawrapper.hh"
#include "playlist.hh"
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
	else if (std::dynamic_pointer_cast<EventLeaveMap>(event))
		return handle_event_leave_map(std::dynamic_pointer_cast<EventLeaveMap>(event), map);
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
	else if (std::dynamic_pointer_cast<EventAddObject>(event)) {
		EventAddObject* add_event = std::dynamic_pointer_cast<EventAddObject>(event).get();
		MapObj new_obj = add_event->get_obj();
		return handle_event_add_object(map, &new_obj);
	}
	else
		std::cerr << "ERROR: gameeventhandler.cc: Not handling UNKNOWN EVENT\n";

	return false;
}

bool GameEventHandler::handle_event_delete_object(std::shared_ptr<Map> map, MapObj* obj)
{
	if (obj == NULL) {
		std::cerr << "ERROR: gameeventhandler.cc: Trying to delete NULL-object.\n";
		return false;
	}

	if (obj->lua_name.size() == 0) {
		unsigned x, y;
		obj->get_coords(x, y);
		map->pop_obj(x, y);
	}
	else
		map->rm_obj(obj);

	return true;
}

bool GameEventHandler::handle_event_add_object(std::shared_ptr<Map> map, MapObj* obj)
{
	if (obj == NULL) {
		std::cerr << "ERROR: gameeventhandler.cc: Trying to add NULL-object.\n";
		return false;
	}

	map->push_obj(*obj);
	return true;
}

bool GameEventHandler::handle_event_add_object(std::shared_ptr<Map> map, MapObj obj)
{
	map->push_obj(obj);
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
// Assumes that file name given by 'event' does *** NOT *** already contain the default sound sample path!

bool GameEventHandler::handle_event_playsound(std::shared_ptr<EventPlaySound> event, std::shared_ptr<Map> map)
{
	static SoundSample sample;  // If this isn't static, then the var
	                            // gets discarded before the sample has
                                // finished playing
	boost::filesystem::path samples_path((std::string)DATADIR);
	samples_path = samples_path / PACKAGE_NAME / "data" / World::Instance().get_name() / "sound";

	if (boost::filesystem::exists(samples_path / event->filename)) {
		sample.play((samples_path / event->filename).c_str());
		return true;
	}
	else {
		std::cerr << "ERROR: gameeventhandler.cc: Sample to be played cannot be found: " << (samples_path / event->filename).c_str() << ".\n";
		return false;
	}
}

// TODO: THIS CAN ONLY EVER BE CALLED FROM LEVEL-0 (I.E. GROUND FLOOR) INDOORS MAPS!
//
// Only indoors maps can be left, and only a hexagonal landscape will ever be the target map of a leave event.

bool GameEventHandler::handle_event_leave_map(std::shared_ptr<EventLeaveMap> event, std::shared_ptr<Map> map)
{
	GameControl* gc = &GameControl::Instance();
	Party* party = &Party::Instance();

	std::cout << "INFO: gameeventhandler.cc: leave handling: old_map_name: " << event->get_old_map_name() << ", map_name: " << event->get_map_name() << ".\n";

	// Put animate objects back to their origins, not their last x and y coordinates
	{
		std::vector<MapObj> tempObjs;
		for (auto map_obj_pair = gc->get_arena()->get_map()->objs()->begin(); map_obj_pair != gc->get_arena()->get_map()->objs()->end(); map_obj_pair++) {
			MapObj& map_obj = map_obj_pair->second;

			if (map_obj.get_type() != MAPOBJ_ITEM) {
				unsigned ox, oy;
				map_obj.get_origin(ox, oy);
				if (ox != 0 || oy != 0) {
					map_obj.set_coords(ox, oy);

					// Make guards neutral on reentry
					if (map_obj.id.find("guard") != std::string::npos)
						map_obj.personality = NEUTRAL;

					// TODO: Should we reset the FLEEING flag as well?! I think so...
					// Might need to store original move_mode first in mapobj.
				}
			}

			tempObjs.push_back(map_obj);
		}

		// Now reinsert objects into hash map...
		gc->get_arena()->get_map()->objs()->clear();
		for (auto mobj: tempObjs) {
			unsigned x, y;
			mobj.get_coords(x, y);
			gc->get_arena()->get_map()->objs()->insert(std::make_pair(std::make_pair(x, y), mobj));
		}
	}

	// Before leaving, store map changes in GameState object
	std::shared_ptr<Map> new_map = gc->get_arena()->get_map();
	std::shared_ptr<IndoorsMap> ind_map = std::dynamic_pointer_cast<IndoorsMap>(new_map);
	GameState::Instance().add_map(ind_map);

	// ***************************** TODO *****************************
	// I disabled the following unload call and am now not sure if there's a leak...
	// arena->get_map()->unload_map_data();
	// delete arena;
	// TODO: Should be ok now as we use shared_ptr for map storing.
	gc->set_arena(NULL);
	// ****************************************************************

	// Now change maps over...
	if (event->get_old_map_name() == event->get_map_name()) {
		// The above test is only ever positive, if the game was actually started inside an indoors map as opposed to the outdoors.
		// In this case, we simply find the first best outdoors map (most games will only have one), and use it as the new map.
		for (std::vector<std::shared_ptr<Map>>::iterator map = World::Instance().get_maps()->begin(); map != World::Instance().get_maps()->end(); map++) {
			if (map->get()->is_outdoors()) {
				event->set_old_map_name(map->get()->get_name().c_str());
				party->set_map_name(event->get_old_map_name().c_str());
				break;
			}
		}
	}
	if (World::Instance().get_map(event->get_old_map_name().c_str()).get()->is_outdoors()) {
		gc->set_arena(Arena::create("outdoors", event->get_old_map_name()));
		party->set_indoors(false);
	}
	else {
		gc->set_arena(Arena::create("indoors", event->get_old_map_name()));
		party->set_indoors(true);
	}
	if (!gc->get_arena()->get_map())
		std::cerr << "WARNING: gameeventhandler.cc: gc->get_arena()->get_map == NULL\n";

	gc->get_arena()->get_map()->xml_load_map_data();

	// Restore previously saved state to remember party position, etc. in old map.
	std::pair<int,int> old_coords;
	if (!party->restore_outside_coords()) {
		try {
			old_coords = gc->get_arena()->get_map()->get_initial_coords();
			party->set_coords(old_coords);
			party->set_indoors(false); // One can only leave indoors maps on level 0, such as flat dungeons (not deep ones!), cities, castles, etc.
		}
		catch (...) {
			std::cerr << "ERROR: gameeventhandler.cc: The current map should have initial coordinates defined, but doesn't. Not sure where to put party on map. Bye, bye!\n";
			exit(-1);
		}
	}
	else {
		old_coords.first  = party->x;
		old_coords.second = party->y;
	}

	gc->get_arena()->set_SDLWindow_object(&(SDLWindow::Instance()));
	gc->get_arena()->determine_offsets();
	gc->get_arena()->show_map(gc->get_viewport().first, gc->get_viewport().second);

	gc->set_party(1, 1);
	for (unsigned x = 1; party->get_coords().first < old_coords.first; x++)
		gc->move_party(DIR_RIGHT, true);
	for (unsigned y = 1; party->get_coords().second < old_coords.second; y++)
		gc->move_party(DIR_DOWN, true);

	// TODO: Not sure if this line will always work as opposed to the original, commented out line below. I think, it's equivalent.
	gc->set_party(party->x, party->y);
	// gc->get_arena()->map_to_screen(party->x, party->y, event->get_x(), event->get_y());

	// Stop all sounds when leaving a map
	Playlist::Instance().clear();

	return true;
}

bool GameEventHandler::handle_event_enter_map(std::shared_ptr<EventEnterMap> event, std::shared_ptr<Map> map)
{
	Party* party = &Party::Instance();
	GameControl* gc = &GameControl::Instance();
	MiniWin* mw = &MiniWin::Instance();
	std::string map_long_name = event->get_map_name();

	// Before changing map, when indoors (e.g. climb down a ladder), store state
	if (party->indoors()) {
		std::shared_ptr<Map> new_map = map;
		IndoorsMap tmp_map = *(std::dynamic_pointer_cast<IndoorsMap>(new_map).get()); // Use it to create IndoorsMap (i.e., deep copy of Map())
		std::shared_ptr<IndoorsMap> ind_map = std::make_shared<IndoorsMap>(tmp_map); // Create a shared_ptr of IndoorsMap
		GameState::Instance().add_map(ind_map); // Add to GameState; TODO: What happens when tmp_map falls off the stack? Is the shared_ptr still valid?
	}

	// TODO: It is not nice to create an entire map just to test for a flag, but it works for now...
	// TODO: We're now using this code for a bit more: to find out if there is a longname tag, and if so, use it!
	{
		std::shared_ptr<Map> tmp_map = World::Instance().get_map(event->get_map_name().c_str());
		if (tmp_map->get_longname().length() > 0)
			map_long_name = tmp_map->get_longname();
		IndoorsMap tmp_map2 = *((IndoorsMap*)tmp_map.get()); // Create deep copy of map because otherwise xml_load_data fucks up the map's state
		tmp_map2.xml_load_map_data();
		if (gc->get_map()->is_outdoors() && // Only check guards, when entering a city from the wilderness, not from an indoors map already.
			tmp_map2.guarded_city &&
				(gc->get_clock()->tod() == NIGHT || gc->get_clock()->tod() == EARLY_MORNING || gc->get_clock()->tod() == MIDNIGHT))
		{
			gc->printcon("No entry. At this ungodly hour, " + map_long_name + " is under lock and key.");
			gc->do_turn();
			return true;
		}
	}

	gc->printcon("Entering " + map_long_name);

	boost::filesystem::path tmp_path;
	tmp_path /= tmp_path / (std::string)DATADIR / (std::string)PACKAGE_NAME / "data" / World::Instance().get_name() / "images" / "indoors_city.png";
	mw->save_texture();
	mw->surface_from_file(tmp_path.string());

	if (!party->indoors())
		party->store_outside_coords();

	map->unload_map_data();
	gc->get_arena().reset(); // WAS: gc->get_arena() = NULL;

	// There is only one landscape which can not be entered, but
	// rather an indoors map may be left to it.  So it's safe to
	// assume every enterable map is indoors.
	gc->set_arena(Arena::create("indoors", event->get_map_name()));

	if (gc->get_arena() == NULL) {
		std::cerr << "ERROR: gameeventhandler.cc::action_on_enter(): Arena NULL.\n";
		std::cerr << "ERROR: gameeventhandler.cc::action_on_enter(): Map name: " << event->get_map_name() << ".\n";
		exit(EXIT_FAILURE);
	}
	if (gc->get_arena()->get_map() == NULL)
		std::cerr << "ERROR: gameeventhandler.cc::action_on_enter(): arena->get_map NULL.\n";

	// If a map is stored inside the current game status, it means the player modified it in the past,
	// and we should load it instead of loading it from the original game files.
	boost::filesystem::path dir((std::string(getenv("HOME")) + "/.eureka/" + World::Instance().get_name() + "/maps/"));
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

	gc->get_arena()->set_SDLWindow_object(&(SDLWindow::Instance()));
	gc->get_arena()->determine_offsets();

	// The following is somewhat yucky code around the fact that you cannot place the party
	// just inside an arbitrary location in the map. So I put it on (1,1) and then use move
	// commands in quick succession to the actual location the party should be in.

	party->set_indoors(true);
	// gc->get_arena()->show_map();
	gc->set_party(event->get_x(), event->get_y());
	gc->set_party(1, 1);

	for (unsigned x = 1; x < event->get_x(); x++)
		gc->move_party(DIR_RIGHT, true);
	for (unsigned y = 1; y < event->get_y(); y++)
		gc->move_party(DIR_DOWN, true);

	return true;
}
