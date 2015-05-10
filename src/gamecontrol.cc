//
//
// Copyright (c) 2005  Andreas Bauer <baueran@gmail.com>
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

#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <utility>
#include <memory>
#include <cstdlib>

#include <boost/random.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lualib.h>
#include <lauxlib.h>
}

// *******************************************
// *** Lookup keyboard symbols SDL_x here! ***
#include "SDL_keysym.h"
// *******************************************

#include "simplicissimus.hh"
#include "gamecontrol.hh"
#include "clock.hh"
#include "combat.hh"
#include "weapon.hh"
#include "weaponhelper.hh"
#include "shield.hh"
#include "shieldhelper.hh"
#include "creature.hh"
#include "party.hh"
#include "arena.hh"
#include "sdlwindow.hh"
#include "iconprops.hh"
#include "outdoorsicons.hh"
#include "indoorsicons.hh"
#include "console.hh"
#include "eventmanager.hh"
#include "actiononenter.hh"
#include "actiontake.hh"
#include "actionpullpush.hh"
#include "eventermap.hh"
#include "mapobj.hh"
#include "hexarena.hh"
#include "world.hh"
#include "miniwin.hh"
#include "tinywin.hh"
#include "ztatswin.hh"
#include "luaapi.hh"
#include "soundsample.hh"
#include "playlist.hh"
#include "itemfactory.hh"
#include "util.hh"
#include "conversation.hh"
#include "gamestate.hh"
#include "gameeventhandler.hh"
#include "config.h"

using namespace std;

GameControl::GameControl()
{
  em = &EventManager::Instance();
  party = &Party::Instance();
  _turn_passed = 0;
  _turns = 0;
  input = "";
  generator.seed(std::time(NULL)); // seed with the current time
}

GameControl& GameControl::Instance()
{
  static GameControl _inst;
  return _inst;
}

int GameControl::set_party(int x, int y)
{
  party->set_coords(x, y);
  arena->map_to_screen(party->x, party->y, screen_pos_party.first, screen_pos_party.second);
  arena->show_party(screen_pos_party.first, screen_pos_party.second);
  return 0;
}

int GameControl::show_win()
{
  arena->show_map(get_viewport().first, get_viewport().second);
  arena->show_party(screen_pos_party.first, screen_pos_party.second);
  arena->update();
  SDLWindow::Instance().blit_interior();
  return 0;
}

/**
 * If update_status_image is true (which is its default param), then not only the tiny win is updated
 * but also the small status win that displays city images, monsters, etc.
 */

void GameControl::draw_status(bool update_status_image)
{
  MiniWin& mwin = MiniWin::Instance();

  std::stringstream ss;
  __attribute__ ((unused)) int moon_icon = 0;  // TODO
  static std::string filename;
  static std::string filename_old;

  filename_old = filename;

  ss << "Gold: " << party->gold();
  ss << ", Food: " << party->food();
  ss << ", Time: ";
  if (_clock.time().first < 10)
    ss << "0" << _clock.time().first << ":";
  else
    ss << _clock.time().first << ":";
  if (_clock.time().second < 10)
    ss << "0" << _clock.time().second;
  else
    ss << _clock.time().second;
  ss << "h";

  switch (_clock.tod()) {
  case EARLY_MORNING:
    filename = "sky_early_morning.png";
    moon_icon = 425;
    break;
  case MORNING:
    filename = "sky_noon.png";
    moon_icon = 426;
    break;
  case NOON:
    filename = "sky_noon.png";
    moon_icon = 427;
    break;
  case AFTERNOON:
    filename = "sky_noon.png";
    moon_icon = 428;
    break;
  case EVENING:
    filename = "sky_evening.png";
    moon_icon = 429;
    break;
  case NIGHT:
    filename = "sky_night.png";
    moon_icon = 430;
    break;
  case MIDNIGHT:
    filename = "sky_night.png";
    moon_icon = 431;
    break;
  }


  if (update_status_image && !party->indoors()) {
	  static SDL_Surface* _tmp_surf = NULL;

	  if (filename_old != filename) {
		std::string tmp_filename =
		  (std::string)DATADIR + "/" + (std::string)PACKAGE + "/data/" +
		  (std::string)WORLD_NAME + "/images/" + filename;

		if (_tmp_surf != NULL)
		  SDL_FreeSurface(_tmp_surf);

		if ((_tmp_surf = IMG_Load(tmp_filename.c_str())) == NULL)
		  cerr << "Error: miniwin could not load surface.\n";

		// mwin.surface_from_file((std::string)DATADIR + "/" + (std::string)PACKAGE + "/data/" + (std::string)WORLD_NAME + "/images/" + filename);
	  }

	  if (_tmp_surf != NULL)
		SDL_BlitSurface(_tmp_surf, NULL, mwin.get_surface(), NULL);
  }

  TinyWin& twin = TinyWin::Instance();
  twin.clear();
  twin.println(0, ss.str());

  // Print moon symbol
  // twin.printch(twin.get_surface()->w - 16, 0, moon_icon);
  // twin.blit();
}

int GameControl::set_arena(std::shared_ptr<Arena> new_arena)
{
	arena = new_arena;
	return 0;
}

std::shared_ptr<Arena> GameControl::get_arena()
{
	return arena;
}

void GameControl::do_turn()
{
	_turns++;
	_turn_passed = 0;

	// Consume food
	if (is_arena_outdoors()) {
		if (_turns%20 == 0)
			Party::Instance().set_food(Party::Instance().food() - Party::Instance().party_size() * 2);
	}
	else {
		if (_turns%40 == 0)
			Party::Instance().set_food(Party::Instance().food() - Party::Instance().party_size());
	}

	// Check if random combat ensues and handle it in case
	if (is_arena_outdoors()) {
		// Increment clock by 5 minutes every turn when outdoors, time doesn't elapse indoors
		if (_turns % 2 == 0)
			_clock.inc(30);

		Combat combat;
		combat.initiate();
	}
	else {
		if (_turns%20 == 0)
			_clock.inc(30);
	}

	draw_status();

	/////////////////////////////////////////////////////////////////////////////////////////////
	// TODO: If we wanted to, we could garbage collect the Lua stack, say, every 100 turns or so.
	// This code works and was tested before.
	//	lua_close(_lua_state);
	//	_lua_state = luaL_newstate();
	//	luaL_openlibs(_lua_state);
	//	publicize_api(_lua_state);
	//	World::Instance().load_world_elements(_lua_state);
	/////////////////////////////////////////////////////////////////////////////////////////////
}

// TODO: Put this in a separate AI-class of sorts. This method moves objects, e.g., monsters hunting down the party

void GameControl::move_objects()
{
	if (is_arena_outdoors())
		return;

	std::vector<MapObj> moved_objects;

	for (auto map_obj_pair = arena->get_map()->objs()->begin(); map_obj_pair != arena->get_map()->objs()->end(); map_obj_pair++) {
		MapObj* map_obj = &(map_obj_pair->second);
		unsigned obj_x, obj_y;

		map_obj->get_coords(obj_x, obj_y);

		// STATIC
		if (map_obj->move_mode == STATIC) {
			// TODO: Maybe do some hovering around?
		}

		// FOLLOW
		if (map_obj->personality == HOSTILE && abs((int)obj_x - party->x) < 6 && abs((int)obj_y - party->y) < 6 ||
					map_obj->move_mode == FOLLOWING)
		{
			// Only follow each round with 70% probability or the following leaves the player no space to breathe
			if (random(0,100) < 40)
				break;

			PathFinding pf(arena->get_map().get());

			unsigned obj_x, obj_y;
			map_obj->get_coords(obj_x, obj_y);

			std::pair<unsigned,unsigned> new_coords = pf.follow_party(obj_x, obj_y, party->x, party->y);

			if ((obj_x != new_coords.first || obj_y != new_coords.second) &&             // If coordinates changed...
					(new_coords.first != party->x || new_coords.second != party->y))     // If new coordinates aren't those of the party...
			{
				map_obj->set_coords(new_coords.first, new_coords.second);
				moved_objects.push_back(*map_obj);
			}
		}

		// FLEE
		if (map_obj->move_mode == FLEE) {
			// Only flee each round with 70% probability or the fleeing leaves the player no chance to ever catch up
			if (random(0,100) < 40)
				break;

			unsigned obj_x, obj_y;
			map_obj->get_coords(obj_x, obj_y);

			// Get list of furthest away fields from party (as the person flees...)
			int longest_dist = abs(party->x - (int)obj_x) + abs(party->y - (int)obj_y);
			int best_x = 0, best_y = 0;
			for (int x = -1; x < 2; x++) {
				for (int y = -1; y < 2; y++) {
					if ((int)obj_x + x > 0 && (int)obj_x + x < arena->get_map()->width() - 3 &&
							(int)obj_y + y > 0 && (int)obj_y + y < arena->get_map()->height() - 3 &&
								walkable((int)obj_x + x, (int)obj_y + y))
					{
						int new_dist = abs(party->x - (int)obj_x - x) + abs(party->y - (int)obj_y - y);
						if (new_dist >= longest_dist) {
							longest_dist = new_dist;
							best_x = x; best_y = y;
						}
					}
				}
			}

			map_obj->set_coords((int)obj_x + best_x, (int)obj_y + best_y);
			moved_objects.push_back(*map_obj);
		}
	}

	// Now do the actual moving of objects on the map, i.e., remove them and reinsert them
	for (MapObj mo: moved_objects) {
		arena->get_map()->pop_obj(mo.id);
		arena->get_map()->push_obj(mo);
	}
}

int GameControl::tick_event_handler()
{
	Console::Instance().animate_cursor(&normal_font);

	if (!arena->is_moving())
		show_win();

	return 0;
}

int GameControl::tick_event_turn_handler()
{
	if (++_turn_passed%25 == 0) {
		do_turn();
		printcon("Pass");
	}

	return 0;
}

int GameControl::key_event_handler(SDL_Event* remove_this_argument)
{
	ZtatsWin& zwin = ZtatsWin::Instance();
	SDL_Event event;

	while (1) {
		if (SDL_WaitEvent(&event)) {
			if (event.type == SDL_USEREVENT) {
				if (event.user.code == TICK) {
					tick_event_handler();
					tick_event_turn_handler();
				}
			}
			else if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
				case SDLK_LEFT:
				case SDLK_KP4:
					move_party(DIR_LEFT);
					break;
				case SDLK_RIGHT:
				case SDLK_KP6:
					move_party(DIR_RIGHT);
					break;
				case SDLK_DOWN:
				case SDLK_KP2:
					move_party(DIR_DOWN);
					break;
				case SDLK_UP:
				case SDLK_KP8:
					move_party(DIR_UP);
					break;
				case SDLK_KP7:
					move_party(DIR_LUP);
					break;
				case SDLK_KP1:
					move_party(DIR_LDOWN);
					break;
				case SDLK_KP3:
					move_party(DIR_RDOWN);
					break;
				case SDLK_KP9:
					move_party(DIR_RUP);
					break;
				case SDLK_SPACE:
					printcon("Pass");
					do_turn();
					break;
				case SDLK_e: {
					// Check if party is on enterable icon, i.e., if there is an enter-action associated to it.
					std::shared_ptr<Action> act = arena->get_map()->get_action(party->x, party->y);

					if (act != NULL) {
						std::shared_ptr<ActionOnEnter> act_on_enter = std::dynamic_pointer_cast<ActionOnEnter>(act);

						if (act_on_enter == NULL)
							printcon("Nothing to enter");
						else
							action_on_enter(act_on_enter);
					}
					else
						printcon("Nothing to enter");

					break;
				}
				case SDLK_a:
					attack();
					break;
				case SDLK_d:
					drop_items();
					break;
				case SDLK_g:
					get_item();
					break;
				case SDLK_i:
					inventory();
					break;
				case SDLK_l:
					look();
					break;
				case SDLK_p:
					pull_push();
					break;
				case SDLK_q:
					quit();
					break;
				case SDLK_r:
					printcon("Ready item - select player");
					ready_item(zwin.select_player());
					break;
				case SDLK_t:
					talk();
					break;
				case SDLK_y: // yield / unready item
					printcon("Yield (let go of) item - select player");
					yield_item(zwin.select_player());
					break;
				case SDLK_z:
					ztats();
					break;
				default:
					printf("key_handler::default: %d (hex: %x)\n", event.key.keysym.sym, event.key.keysym.sym);
					break;
				}

				// Move objects, e.g., attacking monsters hunting the party
				move_objects();

				// If there are hostile monsters next to the party, they may want to attack now...
				get_attacked();

				// After handling a key stroke it is almost certainly a good idea to update the screen
				arena->show_map(get_viewport().first, get_viewport().second);
				arena->show_party(screen_pos_party.first, screen_pos_party.second);
				arena->update();
				SDLWindow::Instance().blit_interior();
			}
		}
	}
	return 0;
}

void GameControl::quit()
{
	EventManager& em = EventManager::Instance();

	printcon("Save game (y/n)?");
	char save_game = em.get_key("yn");
	printcon(std::string(1, save_game) + " ");

	if (save_game == 'y')
		GameState::Instance().save();

	printcon("Quit game (y/n)?");
	char really_quit = em.get_key("yn");
	printcon(std::string(1, really_quit) + " ");

	if (really_quit == 'y')
		exit(EXIT_SUCCESS);
}

void GameControl::ztats()
{
  MiniWin& mwin = MiniWin::Instance();
  ZtatsWin& zwin = ZtatsWin::Instance();

  printcon("Ztats - select player");

  int selected_player = zwin.select_player();
  if (selected_player != -1) {
    mwin.save_surf();
    mwin.clear();
    mwin.println(0, "Ztats", CENTERALIGN);
    mwin.println(1, "(Scroll up/down/left/right, press q to exit)", CENTERALIGN);

    zwin.ztats_player(selected_player);

    mwin.display_last();
  }
}

void GameControl::inventory()
{
  MiniWin& mwin = MiniWin::Instance();
  ZtatsWin& zwin = ZtatsWin::Instance();

  printcon("Inventory");

  mwin.save_surf();
  mwin.clear();
  mwin.println(0, "Inventory", CENTERALIGN);
  std::stringstream ss;
  ss << "Weight: " << party->inventory()->weight() << (party->inventory()->weight() == 1? " stone" : " stones");
  ss << "   Max. capacity: " << party->max_carrying_capacity() << " stones";
  mwin.println(1, ss.str());

  std::map<std::string, int> tmp = party->inventory()->list_wearables();
  std::vector<line_tuple> tmp2 = Util::to_line_tuples(tmp);
  zwin.set_lines(tmp2);
  zwin.clear();
  zwin.scroll();

  mwin.display_last();
}

std::string GameControl::yield_item(int selected_player)
{
	MiniWin& mwin = MiniWin::Instance();
	ZtatsWin& zwin = ZtatsWin::Instance();

	if (selected_player >= 0) {
		PlayerCharacter* player = party->get_player(selected_player);

		mwin.save_surf();
		mwin.clear();
		mwin.println(0, "Yield (let go of) item", CENTERALIGN);
		mwin.println(1, "(Press space to select, q to exit)", CENTERALIGN);

		std::vector<line_tuple> disp_items;
		Alignment al = Alignment::LEFTALIGN;
		if (player->weapon())
			disp_items.push_back(line_tuple("Weapon: " +  player->weapon()->name(), al));
		else
			disp_items.push_back(line_tuple("Weapon: <none>", al));
		disp_items.push_back(line_tuple("Armour: <none>", al)); // TODO
		if (player->shield())
			disp_items.push_back(line_tuple("Shield: " + player->shield()->name(), al));
		else
			disp_items.push_back(line_tuple("Shield: <none>", al));
		disp_items.push_back(line_tuple("Other:  <none>", al)); // TODO: Rings, torch, etc.
		zwin.set_lines(disp_items);
		zwin.clear();

		std::string selected_item_name = "";
		int selection = zwin.select_item();
		switch (selection) {
		case -1:
			break;
		case 0:
			if (player->weapon()) {
				party->inventory()->add(player->weapon());
				selected_item_name = player->weapon()->name();
			}
			player->set_weapon(NULL);
			break;
		case 1:
			break;
		case 2:
			if (player->shield()) {
				party->inventory()->add(player->shield());
				selected_item_name = player->shield()->name();
			}
			player->set_shield(NULL);
			break;
		case 3:
			break;
		default:
			;
		}

		if (selected_item_name.length() > 0) {
			// After yielding an item, the AC may have changed, for example.
			zwin.update_player_list();
			printcon("Yielded " + selected_item_name);
		}
	}

	printcon("Never mind...");
	return "";
}

std::string GameControl::ready_item(int selected_player)
{
	MiniWin& mwin = MiniWin::Instance();
	ZtatsWin& zwin = ZtatsWin::Instance();

	if (selected_player >= 0) {
		mwin.save_surf();
		mwin.clear();
		mwin.println(0, "Ready item", CENTERALIGN);
		mwin.println(1, "(Press space to select, q to exit)", CENTERALIGN);

		std::map<std::string, int> tmp = party->inventory()->list_wearables();
		std::vector<line_tuple>   tmp2 = Util::to_line_tuples(tmp);
		zwin.set_lines(tmp2);
		zwin.clear();
		int selection = zwin.select_item();

		if (selection >= 0) {
			PlayerCharacter* player = party->get_player(selected_player);
			std::string selected_item_name = party->inventory()->get_item(selection)->name();

			if (WeaponHelper::exists(selected_item_name)) {
				if (player->weapon() != NULL) {
					party->inventory()->add(player->weapon());
				}
				// This first creates a new weapon by reserving memory for it
				Weapon* weapon = WeaponHelper::createFromLua(selected_item_name);
				player->set_weapon(weapon);
				// ...and now we are freeing memory for a weapon with the same name in the inventory.
				// A tad bit complicated, perhaps, but not overly difficult to understand.
				// Besides, it makes it very explicit what is going on, and I like that.
				// TODO: Alternatively, one could create a method Inventory::handOver(std::string weapon_name),
				// which removes the weapon from the inventory list and returns its pointer so that the
				// memory remains allocated and it can be passed on e.g. to a player or elsewhere.
				party->inventory()->remove(weapon->name());
			}
			else if (ShieldHelper::exists(selected_item_name)) {
				if (player->shield() != NULL) {
					party->inventory()->add(player->shield());
				}
				Shield* shield = ShieldHelper::createFromLua(selected_item_name);
				player->set_shield(shield);
				party->inventory()->remove(shield->name());
			}
			else
				std::cerr << "Warning: gamecontrol.cc: readying an item that cannot be recognised. This is serious business.\n";

			// After readying an item, the AC may have changed, for example.
			zwin.update_player_list();
			printcon("Readying " + selected_item_name);

			return selected_item_name;
		}
	}

	mwin.display_last();
	printcon("Never mind...");
	return "";
}

// Displays a movable cursor and returns coordinates of map where the user places it and presses return.
// Returns std::pair(-1,-1) if users cancelled selection.

std::pair<int, int> GameControl::select_coords()
{
  bool _ind = party->indoors();
  EventManager& em = EventManager::Instance();
  int CROSSHAIR = _ind? 16 : 41; // icon no of cross hair

  std::list<SDLKey> cursor_keys =
    { SDLK_LEFT, SDLK_RIGHT, SDLK_UP, SDLK_DOWN, SDLK_RETURN, SDLK_q, SDLK_ESCAPE };

  static int cx, cy;  // Cursor
  int px, py;         // Party

  arena->map_to_screen(party->x, party->y, px, py);
  arena->screen_to_map(px, py, px, py);
  cx = px;
  cy = py;

  int old_x = cx, old_y = cy;
  arena->get_map()->push_icon(cx, cy, CROSSHAIR);
  
  while (1) {
    switch (em.get_generic_key(cursor_keys)) {
    case SDLK_LEFT:
      if (arena->adjacent(cx - 1, cy, px, py) && cx - 1 > 0)
        cx--;
      break;
    case SDLK_RIGHT:
      if (arena->adjacent(cx + 1, cy, px, py) && cx + 1 <= (int)arena->get_map()->width() - 4)
        cx++;
      break;
    case SDLK_UP:
      if (_ind && arena->adjacent(cx, cy - 1, px, py) && cy - 1 > 0)
        cy--;
      else if (!_ind && arena->adjacent(cx, cy - 2, px, py) && cy - 2 > 0)
        cy -= 2;
      break;
    case SDLK_DOWN:
      if (_ind && arena->adjacent(cx, cy + 1, px, py) && cy + 1 <= (int)arena->get_map()->height() - 4)
        cy++;
      else if (!_ind && arena->adjacent(cx, cy + 2, px, py) && cy + 2 <= (int)arena->get_map()->height() - 4)
        cy += 2;
      break;
    case SDLK_RETURN:
      arena->get_map()->pop_obj(old_x, old_y);
      return std::make_pair(cx, cy);
    case SDLK_ESCAPE:
    case SDLK_q:
      arena->get_map()->pop_obj(old_x, old_y);
      return std::make_pair(-1, -1);
    default:
      std::cerr << "Warning: Pressed unhandled key.\n";
    }

    if (! _ind) {
      if ( (cx % 2) == 0 && (cy % 2)  != 0 ) {
        if (arena->adjacent(cx, cy - 1, px, py))
          cy--;
        else {
          cx = old_x;
          cy = old_y;
        }
      }
      else if ( (cx % 2) != 0 && (cy % 2)  == 0 ) {
        if (arena->adjacent(cx, cy + 1, px, py))
          cy++;
        else {
          cx = old_x;
          cy = old_y;
        }
      }
    }

    arena->get_map()->pop_obj(old_x, old_y);
    arena->get_map()->push_icon(cx, cy, CROSSHAIR);
    old_x = cx; old_y = cy;
  }
}

void GameControl::drop_items()
{
	MiniWin& mwin = MiniWin::Instance();
	ZtatsWin& zwin = ZtatsWin::Instance();

	printcon("Drop item - select which one");

	mwin.save_surf();
	mwin.clear();
	mwin.println(0, "Drop item", CENTERALIGN);
	mwin.println(1, "(Press space to drop selected item, q to exit)");

	std::map<std::string, int> items = party->inventory()->list_all();
	std::vector<line_tuple> items_l  = Util::to_line_tuples(items);
	zwin.set_lines(items_l);
	zwin.clear();
	int selection = zwin.select_item();

	if (selection >= 0) {
		std::stringstream ss;
		std::string selected_item_name = party->inventory()->get_item(selection)->name();
		std::string selected_item_plural_name = party->inventory()->get_item(selection)->plural_name();
		std::vector<Item*>* all_tmp_items = party->inventory()->get(selection);
		Item* tmp = (*all_tmp_items)[0];
		// Item* tmp = party->inventory()->get_item(selection);
		int size_tmp_items = all_tmp_items->size();
		int drop_how_many = 0;

		// Determine how many items shall be dropped, in case the inventory has more than 1
		if (size_tmp_items > 1) {
			printcon("How many? (1-" + boost::lexical_cast<std::string>(size_tmp_items) + ")");
			std::string reply = Console::Instance().gets();
			try {
				if (reply.length() == 0)
					drop_how_many = size_tmp_items;
				else {
					drop_how_many = boost::lexical_cast<int>(reply);
					if (!(drop_how_many >= 1 && drop_how_many <= size_tmp_items)) {
						printcon("Huh? Nothing dropped.");
						return;
					}
				}
			} catch( boost::bad_lexical_cast const& ) {
				printcon("Huh? Nothing dropped.");
				return;
			}
		}

		// Create corresponding icon if party is indoors
		if (party->indoors()) {
			MapObj moTmp;
			moTmp.removable = true;
			moTmp.set_coords(party->x, party->y);
			moTmp.set_icon(tmp->icon);
			moTmp.lua_name = tmp->luaName();  // TODO: This can be empty. A problem? Handle this case?
			moTmp.how_many = drop_how_many;

			// Add dropped item to current map
			arena->get_map()->push_obj(moTmp);
		}

		for (int i = 0; i < max(drop_how_many, 1); i++)
			party->inventory()->remove(tmp->name());

		if (size_tmp_items == 1)
			ss << "Dropped a" << (Util::vowel(selected_item_name[0])? "n " : " ") << selected_item_name << ".";
		else
			ss << "Dropped " << drop_how_many << " " << selected_item_plural_name << ".";
		printcon(ss.str());
	}

	mwin.display_last();
}

// Makes all guards of a town turn hostile (e.g., after committing a crime), or neutral, etc.

void GameControl::make_guards(PERSONALITY pers)
{
	for (auto map_obj_pair = arena->get_map()->objs()->begin(); map_obj_pair != arena->get_map()->objs()->end(); map_obj_pair++) {
		MapObj* map_obj = &(map_obj_pair->second);

		// If MapObj ID contains the string "guard", it is a guard and will be set to hostile
		if (map_obj->id.find("guard") != std::string::npos)
			map_obj->personality = pers;
	}
}

// The "opposite" of attack(), so to speak.
// Lets those hostile objects attack the party if next to it.

void GameControl::get_attacked()
{
	for (auto map_obj_pair = arena->get_map()->objs()->begin(); map_obj_pair != arena->get_map()->objs()->end(); map_obj_pair++) {
		MapObj* map_obj = &(map_obj_pair->second);
		unsigned obj_x, obj_y;
		map_obj->get_coords(obj_x, obj_y);

		if (map_obj->personality == HOSTILE) {
			if (abs(party->x - (int)obj_x) <= 1 && abs(party->y - (int)obj_y) <= 1) {
				if (map_obj->get_type() == MAPOBJ_PERSON) {
					if (map_obj->get_init_script_path().length() > 0) {
						Combat combat;
						combat.create_monsters_from_init_path(map_obj->get_init_script_path());
						if (combat.initiate())
							get_map()->pop_obj(map_obj->id);
						return;
					}
				}
				else if (map_obj->get_type() == MAPOBJ_MONSTER) {
					// We have foes from previous attack left, so do not initiate fresh combat
					if (map_obj->get_foes().size() > 0) {
						Combat combat;
						combat.set_foes(map_obj->get_foes());
						if (combat.initiate()) {
							get_map()->pop_obj(map_obj->id);
							return;
						}
						map_obj->set_foes(combat.get_foes());
						return;
					}
					else if (map_obj->get_combat_script_path().length() > 0) {
						Combat combat;
						combat.create_monsters_from_combat_path(map_obj->get_combat_script_path());
						if (combat.initiate()) {
							get_map()->pop_obj(map_obj->id);
							return;
						}
						map_obj->set_foes(combat.get_foes());
						return;
					}
				}
			}
		}
	}
}

// If the user pressed (a)...
// This attack is only called executed when INDOORS, cf. first if statement!

void GameControl::attack()
{
	if (!party->indoors()) {
		printcon("Attack - no one is around.");
		return;
	}

	printcon("Attack - in which direction?");
	std::pair<int,int> coords = select_coords();

	auto avail_objects = arena->get_map()->objs()->equal_range(coords);

	// Strictly speaking this loop should not be necessary as we don't want monsters/ppl to walk over
	// objects, but in case we're changing our minds later, we're looking for them in a list of objects
	// rather than checking the first one only.

	if (avail_objects.first != avail_objects.second) {
		for (auto curr_obj = avail_objects.first; curr_obj != avail_objects.second; curr_obj++) {
			MapObj& the_obj = curr_obj->second;

			// Indoor monsters either have a combat_script OR a init_script attribute but *NEVER* both!
			// Init script usually means the player can talk to them...
			if (the_obj.get_type() == MAPOBJ_PERSON) {
				// We initiate fresh combat
				if (the_obj.get_init_script_path().length() > 0) {
					Combat combat;
					combat.create_monsters_from_init_path(the_obj.get_init_script_path());

					// If a town folk is attacked, the guards are alerted, and folks flee afterwards
					the_obj.move_mode = FLEE;
					make_guards(HOSTILE);

					if (combat.initiate())
						get_map()->pop_obj(the_obj.id);
					return;
				}
				else {
					printcon("You use your charms, but there is no response");
					return;
				}
			}
			else if (the_obj.get_type() == MAPOBJ_MONSTER) {
				// We initiate fresh combat
				if (the_obj.get_combat_script_path().length() > 0) {
					Combat combat;
					combat.create_monsters_from_combat_path(the_obj.get_combat_script_path());
					if (combat.initiate()) {
						get_map()->pop_obj(the_obj.id);
						return;
					}
					the_obj.set_foes(combat.get_foes());
					return;
				}
				// We have foes from previous attack left, so do not initiate fresh combat
				else if (the_obj.get_foes().size() > 0) {
					Combat combat;
					combat.set_foes(the_obj.get_foes());
					if (combat.initiate()) {
						get_map()->pop_obj(the_obj.id);
						return;
					}
					the_obj.set_foes(combat.get_foes());
					return;
				}
				else {
					printcon("You use your charms, but there is no response");
					return;
				}
			}
			else if (the_obj.get_type() == MAPOBJ_ANIMAL) {
				the_obj.move_mode = FLEE;
				printcon("Should you really attack an innocent animal?");
				return;
			}
		}
	}
	printcon("No one there to attack. Try taking a deep breath instead.");
}

void GameControl::talk()
{
	MiniWin& mwin = MiniWin::Instance();
	ZtatsWin& zwin = ZtatsWin::Instance();

	if (!party->indoors()) {
		printcon("Talk - sorry, no one is around");
		return;
	}

	printcon("Talk - in which direction?");
	std::pair<int,int> coords = select_coords();

	auto avail_objects = arena->get_map()->objs()->equal_range(coords);

	// Strictly speaking this loop should not be necessary as we don't want monsters/ppl to walk over
	// objects, but in case we're changing our minds later, we're looking for them in a list of objects
	// rather than checking the first one only.

	if (avail_objects.first != avail_objects.second) {
		for (auto curr_obj = avail_objects.first; curr_obj != avail_objects.second; curr_obj++) {
			MapObj& the_obj = curr_obj->second;

			if (the_obj.get_type() == MAPOBJ_PERSON) {
				if (the_obj.get_init_script_path().length() > 0) {
					Conversation conv(the_obj);
					conv.initiate();
					return;
				}
				else {
					printcon("You use your charms, but there is no response");
					return;
				}
			}
		}
	}
	printcon("No around to talk to");
}

std::shared_ptr<Map> GameControl::get_map()
{
	return arena->get_map();
}

void GameControl::get_item()
{
	printcon("Get - from which direction?");
	std::pair<int, int> coords = select_coords();

	// Range of available MapObjects. These are not the actual items!
	auto avail_objects = arena->get_map()->objs()->equal_range(coords);

	if (avail_objects.first != avail_objects.second) {
		int size = 0; // Determine actual number of MapObjects
		for (auto curr_obj = avail_objects.first; curr_obj != avail_objects.second; curr_obj++, size++);

		MapObj* curr_obj = 0;
		std::vector<MapObj> map_objs; // Convenience only: For storing and quicker looking up the various map objects in a given location.

		// TODO: If there are multiple items, check which one needs to be picked up (or all)
		if (size > 1) {
			printcon("Select item from the list");

			MiniWin& mwin = MiniWin::Instance();
			ZtatsWin& zwin = ZtatsWin::Instance();

			mwin.save_surf();
			mwin.clear();
			mwin.println(0, "Get item", CENTERALIGN);
			mwin.println(1, "(Press space to get selected item, q to exit)");

			std::map<std::string, int> items;
			for (auto obj_itr = avail_objects.first; obj_itr != avail_objects.second; obj_itr++) {
				MapObj the_obj = obj_itr->second;

				// Only offer removable items to be picked up. Non-removables also may not
				// have a lua_name, and we don't want to be checking for that all the time
				// either...
				if (the_obj.removable) {
					map_objs.push_back(the_obj);
					// We generate an item according to its lua name in order to get the properties
					// for the list of items to get.  Not sure if this is necessary or if one should
					// simply dissect the lua name.  But this way, we keep the mapping bewteen icons
					// and the actual item inside the respective factory functions, centrally. So it
					// probably is a good idea to do it as it is done now.
					Item* item = ItemFactory::create(the_obj.lua_name);
					if (the_obj.how_many > 1)
						items.insert(std::pair<std::string, int>(item->plural_name(), the_obj.how_many));
					else
						items.insert(std::pair<std::string, int>(item->name(), 1));
					delete item;
				}
			}

			// If there are multiple items in a location, but only one of them is removable, skip selection dialog.
			if (map_objs.size() == 1) {
				curr_obj = &(map_objs[0]);
			}
			else {
				std::vector<line_tuple> items_l = Util::to_line_tuples(items);
				zwin.set_lines(items_l);
				zwin.clear();
				int selection = zwin.select_item();

				mwin.display_last();

				if (selection >= 0)
					curr_obj = &(map_objs[selection]);
				else
					return;
			}
		}
		else
			curr_obj = &(avail_objects.first->second);

		GameEventHandler gh;

		if (curr_obj->removable) {
			if (curr_obj->lua_name.length() > 0) {
				// Depending on the name the MapObj has, we look up in the Lua list of items, and create one accordingly for pick up.
				Item* item = ItemFactory::create(curr_obj->lua_name);

				// Picking up more than 1 of the same item
				if (curr_obj->how_many > 1) {
					printcon("How many? (1-" + boost::lexical_cast<std::string>(curr_obj->how_many) + ")");
					int taking = 0;

					try {
						std::string input = Console::Instance().gets();
						if (input.length() == 0) // Simply pressing return means user wants to take all available items
							taking = curr_obj->how_many;
						else
							taking = boost::lexical_cast<int>(input);
					} catch( boost::bad_lexical_cast const& ) {
						printcon("Huh? Nothing taken");
						return;
					}

					if (taking > 0 && taking <= curr_obj->how_many)
						printcon("Taking " + boost::lexical_cast<std::string>(taking) + " " + item->plural_name());
					else {
						printcon("Huh? Nothing taken");
						return;
					}

					// Let's now create the n new items to be taken individually via a factory...
					for (int i = 0; i < taking; i++) {
						party->inventory()->add(ItemFactory::create(curr_obj->lua_name));

						// Perform action events
						for (auto action = curr_obj->actions()->begin(); action != curr_obj->actions()->end(); action++) {
							if ((*action)->name() == "ACT_ON_TAKE") {
								for (auto curr_ev = (*action)->events_begin(); curr_ev != (*action)->events_end(); curr_ev++)
									gh.handle(*curr_ev, arena->get_map());
							}
							else
								std::cerr << "WARNING: Couldn't perform action on take: " << (*action)->name() << ".\n";
						}
					}
					// See if some items are leftover after taking...
					if (curr_obj->how_many - taking == 0)
						arena->get_map()->pop_obj(coords.first, coords.second);
					else
						curr_obj->how_many -= taking;
				}
				// Picking up exactly 1 item
				else {
					printcon("Taking " + item->name());
					party->inventory()->add(item);

					// Perform action events
					for (auto action = curr_obj->actions()->begin(); action != curr_obj->actions()->end(); action++) {
						std::cout << "Trying to perform action...\n";
						if ((*action)->name() == "ACT_ON_TAKE") {
							for (auto curr_ev = (*action)->events_begin(); curr_ev != (*action)->events_end(); curr_ev++)
								gh.handle(*curr_ev, arena->get_map());
						}
						else
							std::cerr << "WARNING: Couldn't perform action on take: " << (*action)->name() << ".\n";
					}

					arena->get_map()->pop_obj(coords.first, coords.second);
				}
			}
			else {
				printcon("Sorry taking of this item not yet implemented (no lua_name). "
						 "Buy the author a beer or two and he might implement it for you.");
			}
		}
		else
			printcon("Nice try");
	}
	else
		printcon("Nothing to get here");
}

void GameControl::look()
{
	printcon("Look - which direction?");
	std::pair<int, int> coords = select_coords();
	int icon_no = arena->get_map()->get_tile(coords.first, coords.second);

	// Check out of map bounds
	if (icon_no >= 0) {
		printcon("You see " + IndoorsIcons::Instance().get_props(icon_no)->get_name());

		// Return range of found objects at given location
		auto found_obj = arena->get_map()->objs()->equal_range(coords);

		if (found_obj.first != found_obj.second) {
			std::stringstream ss;
			ss << "Besides, there" << (found_obj.first->second.how_many > 1 ? " are " : " is ");

			// Now draw the objects, if there are any
			for (auto curr_obj = found_obj.first; curr_obj != found_obj.second;) { // curr_obj++) {
				MapObj map_obj = curr_obj->second;

				// First, determine the displayed tile of the object, lua_name overrides the icon's name inside the xml-file
				if (map_obj.how_many > 1) {
					// There can only be multiple items if they have a lua correspondence.
					// So instead of using the icon_name, we use the map_obj lua_name to
					// temporarily create that item just in order to get its properties.
					Item* item = ItemFactory::create(map_obj.lua_name);
					ss << map_obj.how_many << " " << item->plural_name();
					delete item;
				}
				else {
					int obj_icon_no = map_obj.get_icon();
					std::string icon_name = IndoorsIcons::Instance().get_props(obj_icon_no)->get_name();
					ss << (Util::vowel(icon_name[0])? "an " : "a ") << icon_name;
				}

				if (++curr_obj != found_obj.second)
					ss << ", ";
			}
			printcon(ss.str());
		}
	}
	else
		printcon("There is nothing to see");
}

/**
 *  Returns true if the current arena is an outdoors arena, false otherwise.
 */

bool GameControl::is_arena_outdoors()
{
  return std::dynamic_pointer_cast<HexArena>(arena) != NULL;
}

bool GameControl::walk_fullspeed(int x, int y)
{
  bool return_value = false;

  if (is_arena_outdoors())
    return_value = OutdoorsIcons::Instance().get_props(arena->get_map()->get_tile(x, y))->flags() & WALK_FULLSPEED;
  else
    return_value = IndoorsIcons::Instance().get_props(arena->get_map()->get_tile(x, y))->flags() & WALK_FULLSPEED;

  return return_value;
}

bool GameControl::walkable(int x, int y)
{
	if (x >= (int)(arena->get_map()->width()) || x < 0)
		return false;
	if (y >= (int)(arena->get_map()->height()) || y < 0)
		return false;

	bool return_value = false;

	if (is_arena_outdoors())
		return_value = OutdoorsIcons::Instance().get_props(arena->get_map()->get_tile(x, y))->flags() & WALK_NOT;
	else {
		return_value = IndoorsIcons::Instance().get_props(arena->get_map()->get_tile(x, y))->flags() & WALK_NOT;

		// Don't walk over monsters...
		if (!return_value) {
			auto found_obj = arena->get_map()->objs()->equal_range(std::make_pair(x,y));
			if (found_obj.first != found_obj.second) {
				for (auto curr_obj = found_obj.first; curr_obj != found_obj.second; curr_obj++) {
					MapObj& map_obj = curr_obj->second;

					if (map_obj.get_type() == MAPOBJ_MONSTER)
						return false;
					else if (map_obj.get_type() == MAPOBJ_ANIMAL)
						return false;
					else if (map_obj.get_type() == MAPOBJ_PERSON)
						return false;
				}
			}
		}
	}

	return !return_value;
}

void GameControl::move_party(LDIR dir)
{
	MiniWin& mwin = MiniWin::Instance();

	int x_diff = 0, y_diff = 0;
	static SoundSample sample;  // If this isn't static, then the var
	                            // gets discarded before the sample has
	                            // finished playing

	// Determine center of arena
	int screen_center_x, screen_center_y;
	arena->get_center_coords(screen_center_x, screen_center_y);

	// Indoors
	if (!is_arena_outdoors()) {
		switch (dir) {
		case DIR_LEFT:
			if (walkable(party->x - 1, party->y)) {
				arena->moving(true);
				printcon("West");
				sample.play(WALK);
				x_diff = -1;
				if (party->x <= 1) {
					if (leave_map()) {
						mwin.display_last();
						return;
					}
					x_diff = 0;
				}
				if (screen_pos_party.first < screen_center_x)
					arena->move(DIR_LEFT);
			}
			else {
				printcon("Blocked");
				sample.play(HIT);
			}
			break;
		case DIR_RIGHT:
			if (walkable(party->x + 1, party->y)) {
				arena->moving(true);
				printcon("East");
				sample.play(WALK);
				x_diff = 1;
				// See comment below!
				if ((unsigned)party->x >= arena->get_map()->width() - 4) {
					if (leave_map()) {
						mwin.display_last();
						return;
					}
					x_diff = 0;
				}
				if (screen_pos_party.first > screen_center_x) {
					arena->move(DIR_RIGHT);
				}
			}
			else {
				printcon("Blocked");
				sample.play(HIT);
			}
			break;
		case DIR_DOWN:
			if (walkable(party->x, party->y + 1)) {
				arena->moving(true);
				printcon("South");
				sample.play(WALK);
				y_diff = 1;
				// TODO: The -4 is necessary as show_map() creates some kind of
				// border around the map to be displayed.  Might be able to
				// reduce this border to 1, as 4 seems somewhat random.
				if ((unsigned)party->y >= arena->get_map()->height() - 4) {
					if (leave_map()) {
						mwin.display_last();
						return;
					}
					y_diff = 0;
				}
				if (screen_pos_party.second > screen_center_y) {
					arena->move(DIR_DOWN);
				}
			}
			else {
				printcon("Blocked");
				sample.play(HIT);
			}
			break;
		case DIR_UP:
			if (walkable(party->x, party->y - 1)) {
				arena->moving(true);
				printcon("North");
				sample.play(WALK);
				y_diff = -1;
				if (party->y <= 1) {
					if (leave_map()) {
						mwin.display_last();
						return;
					}
					y_diff = 0;
				}
				if (screen_pos_party.second < screen_center_y) {
					arena->move(DIR_UP);
				}
			}
			else {
				printcon("Blocked");
				sample.play(HIT);
			}
			break;
		default:
			;
			break;
		}
	}
	// Outdoors
	else {
		switch (dir) {
		case DIR_LEFT:
			if (walkable(party->x - 1, party->y)) {
				if (party->x % 2 == 0)
					printcon("South-West");
				else
					printcon("North-West");
				sample.play(WALK);

				x_diff = -1;
				if (screen_pos_party.first < screen_center_x) {
					arena->move(DIR_LEFT);
					arena->move(DIR_LEFT);
				}
				break;
		case DIR_RIGHT:
			if (walkable(party->x + 1, party->y)) {
				if (party->x % 2 == 0)
					printcon("South-East");
				else
					printcon("North-East");
				sample.play(WALK);

				x_diff = 1;
				if (screen_pos_party.first > screen_center_x) {
					arena->move(DIR_RIGHT);
					arena->move(DIR_RIGHT);
				}
			}
			break;
		case DIR_DOWN:
			if (walkable(party->x, party->y + 2)) {
				printcon("South");
				sample.play(WALK);
				y_diff = 2;
				if (screen_pos_party.second > screen_center_y)
					arena->move(DIR_DOWN);
			}
			break;
		case DIR_UP:
			if (walkable(party->x, party->y -2)) {
				printcon("North");
				sample.play(WALK);
				y_diff = -2;
				if (screen_pos_party.second < screen_center_y)
					arena->move(DIR_UP);
			}
			break;
		case DIR_RDOWN:
			if (walkable(party->x + 1, party->y + (party->x % 2 == 0? 0 : 2))) {
				printcon("South-East");
				sample.play(WALK);
				y_diff = party->x % 2 == 0? 0 : 2;
				x_diff = 1;
				if (screen_pos_party.first > screen_center_x) {
					arena->move(DIR_RIGHT);
					arena->move(DIR_RIGHT);
				}
				if (screen_pos_party.second > screen_center_y) {
					arena->move(DIR_DOWN);
				}
			}
			break;
		case DIR_RUP:
			if (walkable(party->x + 1, party->y - (party->x % 2 == 0? 2 : 0))) {
				printcon("North-East");
				sample.play(WALK);
				y_diff = -(party->x % 2 == 0? 2 : 0);
				x_diff = 1;
				if (screen_pos_party.first > screen_center_x) {
					arena->move(DIR_RIGHT);
					arena->move(DIR_RIGHT);
				}
				if (screen_pos_party.second < screen_center_y) {
					arena->move(DIR_UP);
				}
			}
			break;
		case DIR_LUP:
			if (walkable(party->x - 1, party->y - (party->x % 2 == 0? 2 : 0))) {
				printcon("North-West");
				sample.play(WALK);
				y_diff = -(party->x % 2 == 0? 2 : 0);
				x_diff = -1;
				if (screen_pos_party.first < screen_center_x) {
					arena->move(DIR_LEFT);
					arena->move(DIR_LEFT);
				}
				if (screen_pos_party.second < screen_center_y) {
					arena->move(DIR_UP);
				}
			}
			break;
		case DIR_LDOWN:
			if (walkable(party->x - 1, party->y + (party->x % 2 == 0? 0 : 2))) {
				printcon("South-West");
				sample.play(WALK);
				y_diff = party->x % 2 == 0? 0 : 2;
				x_diff = -1;
				if (screen_pos_party.first < screen_center_x) {
					arena->move(DIR_LEFT);
					arena->move(DIR_LEFT);
				}
				if (screen_pos_party.second > screen_center_y) {
					arena->move(DIR_DOWN);
				}
			}
			break;
			}
		case DIR_NONE:
			; // TODO
			break;
		}
	}

	party->x += x_diff;
	party->y += y_diff;
	arena->map_to_screen(party->x, party->y, screen_pos_party.first, screen_pos_party.second);

	std::cout << "Info: Party: " << party->x << ", " << party->y << "\n";

	do_turn();
}

/**
 * Return viewport size for night, torches, etc.
 */

std::pair<int, int> GameControl::get_viewport()
{
	int x = 0; // Default: maximum viewport, bright as day!

	if (arena->get_map()->is_dungeon) {
		int rad = max(2, party->current_light_source_radius());
		return std::make_pair(rad,rad);
	}

	switch (_clock.tod()) {
	case EARLY_MORNING:
		x = 8 + (_clock.time().first%2 == 0? _clock.time().first : _clock.time().first + 1);
		break;
	case MORNING:
		x = 16 + (_clock.time().first%2 == 0? _clock.time().first : _clock.time().first + 1);
		break;
	case NOON:
		break;
	case AFTERNOON:
		break;
	case EVENING:
	case NIGHT:
		x = max(6, 24 - (_clock.time().first%2 == 0? _clock.time().first : _clock.time().first + 1) + 10);
		break;
	case MIDNIGHT:
		x = 4;
		break;
	}

	// It's a square view!

	return std::make_pair(x,x);
}

void GameControl::pull_push()
{
	printcon("Pull/push - which direction?");

	std::pair<int, int> coords = select_coords();
	std::shared_ptr<Action> act = arena->get_map()->get_action(coords.first, coords.second);

	if (act == NULL) {
		printcon("Nothing to pull or push here");
		return;
	}

	std::shared_ptr<ActionPullPush> action = std::dynamic_pointer_cast<ActionPullPush>(act);

	if (action == NULL) {
		printcon("Nothing to pull or push here");
		return;
	}

	GameEventHandler gh;

	for (auto curr_ev = action->events_begin(); curr_ev != action->events_end(); curr_ev++)
		gh.handle(*curr_ev, arena->get_map());
}

void GameControl::action_on_enter(std::shared_ptr<ActionOnEnter> action)
{
	GameEventHandler gh;

	for (auto curr_ev = action->events_begin(); curr_ev != action->events_end(); curr_ev++)
		gh.handle(*curr_ev, arena->get_map());
}

// TODO: THIS CAN ONLY EVER BE CALLED FROM LEVEL-0 (I.E. GROUND FLOOR) INDOORS MAPS!
//
// Returns true if the player entered "Y" to the question of whether she
// would like to leave a map.  Otherwise false is returned.

bool GameControl::leave_map()
{
	printcon("Do you wish to leave? (y/n)");

	switch (em->get_key("yn")) {
	case 'y': {
		// Put animate objects back to their origins, not their last x and y coordinates
		{
			std::vector<MapObj> tempObjs;
			for (auto map_obj_pair = arena->get_map()->objs()->begin(); map_obj_pair != arena->get_map()->objs()->end(); map_obj_pair++) {
				MapObj& map_obj = map_obj_pair->second;

				if (map_obj.get_type() != MAPOBJ_ITEM) {
					unsigned ox, oy;
					map_obj.get_origin(ox, oy);
					if (ox != 0 || oy != 0) {
						cout << "Resetting coords for " << map_obj.id << "\n";
						map_obj.set_coords(ox, oy);

						// Make guards neutral on reentry
						if (map_obj.id.find("guard") != std::string::npos)
							map_obj.personality = NEUTRAL;

						// TODO: Should we reset the FLEEING flag as well?! I think so...
					}
				}

				tempObjs.push_back(map_obj);
			}

			// Now reinsert objects into hash map...
			arena->get_map()->objs()->clear();
			for (auto mobj: tempObjs) {
				unsigned x, y;
				mobj.get_coords(x, y);
				arena->get_map()->objs()->insert(std::make_pair(std::make_pair(x, y), mobj));
			}
		}

		// Before leaving, store map changes in GameState object
		std::shared_ptr<Map> new_map = arena->get_map();
		std::shared_ptr<IndoorsMap> ind_map = std::dynamic_pointer_cast<IndoorsMap>(new_map);
		GameState::Instance().add_map(ind_map);

		// ***************************** TODO *****************************
		// I disabled the following unload call and am now not sure if there's a leak...
		// arena->get_map()->unload_map_data();
		// delete arena;
		// TODO: Should be ok now as we use shared_ptr for map storing.
		arena = NULL;
		// ****************************************************************

		// Restore previously saved state to remember party position, etc. in old map.
		party->restore_outside_coords();
		party->set_indoors(false); // One can only leave indoors maps on level 0, such as flat dungeons (not deep ones!), cities, castles, etc.

		// Now change maps over...
		set_arena(Arena::create((party->indoors()? "indoors" : "outdoors"), party->map_name()));
		if (!arena->get_map())
			std::cout << "Warning arena->get_map NULL\n";

		arena->get_map()->xml_load_map_data();
		arena->set_SDL_surface(SDLWindow::Instance().get_drawing_area_SDL_surface());
		arena->determine_offsets();
		arena->show_map(get_viewport().first, get_viewport().second);
		arena->map_to_screen(party->x, party->y, screen_pos_party.first, screen_pos_party.second);

		// Stop sound
		Playlist::Instance().clear();

		return true;
	}
	case 'n':
		return false;
	}

	return false;
}

int GameControl::close_win()
{
  SDLWindow::Instance().close();
  return 0;
}

void GameControl::set_outdoors(bool mode)
{
  party->set_indoors(!mode);
}

void GameControl::set_map_name(const char* new_name)
{
  party->set_map_name(new_name);
}

int GameControl::random(int min, int max)
{
  NumberDistribution distribution(min, max);
  Generator numberGenerator(generator, distribution);
  return numberGenerator();
}

void GameControl::printcon(const std::string s, bool wait)
{
  Console::Instance().print(&normal_font, s, wait);
}

Clock* GameControl::get_clock()
{
	return &_clock;
}
