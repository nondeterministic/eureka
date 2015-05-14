/* *********************************************************************
 * This file is part of Simplicissimus/Leibniz.
 *
 * Copyright (c) Andreas Bauer <baueran@gmail.com>
 *
 * Simplicissimus/Leibniz is free software: you can redistribute it
 * and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * Simplicissimus/Leibniz is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Simplicissimus/Leibniz.  If not, see 
 * <http://www.gnu.org/licenses/>. 
 * ********************************************************************* */

#include "config.h"
#include "charset.hh"
#include "world.hh"
#include "arena.hh"
#include "hexarena.hh"
#include "party.hh"
#include "playercharacter.hh"
#include "ztatswin.hh"
#include "gamecontrol.hh"
#include "console.hh"
#include "eventmanager.hh"
#include "luaapi.hh"
#include "weapon.hh"
#include "shield.hh"
#include "weaponhelper.hh"
#include "shieldhelper.hh"
#include "util.hh"
#include "profession.hh"
#include "simplicissimus.hh"

#include <iostream>
#include <cstdlib>
#include <string>
#include <ctime>
#include <vector>
#include <memory>

#include <boost/filesystem.hpp>
#include <boost/signals.hpp>
#include <boost/unordered_map.hpp>
#include <boost/algorithm/string.hpp>

#include <libxml++/libxml++.h>
#include <libxml++/parsers/textreader.h>

extern "C" {
#include "SDL.h"
#include <lua.h>
#include <lualib.h>
#include <lualib.h>
#include <lauxlib.h>
}

// ******************************************************************************
// Global variables
// ******************************************************************************

lua_State* _lua_state = NULL;

Uint32 tick_callback(Uint32 interval, void *param)
{
  SDL_Event event;
 
  event.type = SDL_USEREVENT;
  event.user.code = TICK;
  event.user.data1 = NULL;
  event.user.data2 = NULL;

  SDL_PushEvent(&event);
  return(interval);
}

// ******************************************************************************
// Main
// ******************************************************************************

int main(int argc, char *argv[])
{
  EventManager* em = &EventManager::Instance();
  Party* party     = &Party::Instance();
  SDLWindow* win   = &SDLWindow::Instance();
  GameControl* gc  = &GameControl::Instance();
  Charset normalFont;

  int res_w = 1024, res_h=768;

  int x = 13, y = 21; // Some default values for starting position in wilderness

  // Initialise random number generator
  std::srand(std::time(NULL));

  // TODO make proper argument handling happen via getopt.  For now I
  // just need a switch to change resolution w/o having to recompile.
  if (argc == 2) {
    // TODO 800x600 is not possible due to various graphic ratios.
    // 800x577 works though and should be a fall-back for small
    // displays such as EeePcs or other small, embedded devices.  The
    // game is intended to be played in 1024x768 anyway.
    res_w = 800; res_h=577;
  }

  // Load game data
  if (! World::Instance().xml_load_world_data(((std::string)DATADIR + 
					       "/simplicissimus/data/" + 
					       (std::string)WORLD_NAME + ".xml").c_str()))
    {
      std::cerr << "Error loading game data. Did you run make install?" << std::endl;
      return -1;
    }

  // Initialise Lua engine
  _lua_state = luaL_newstate();
  luaL_openlibs(_lua_state); /* Load Lua libraries */

  // Make the simplicissimus API visible to Lua
  publicize_api(_lua_state);

  // Load Lua scripts, basically.
  World::Instance().load_world_elements(_lua_state);

  // Create an arena and load a map
  std::shared_ptr<Arena> arena;
  try {
    arena = Arena::create("outdoors", "Landschaft");
  }
  catch (const MapNotFound& e) {
    std::cerr << "Error: MapNotFound exception." << std::endl;
    exit(EXIT_FAILURE);
  }

  // Now create shared_ptr from raw pointer
  //  std::shared_ptr<HexArena> arena = std::dynamic_pointer_cast<HexArena>(_arena);

  // Create window
  // TODO: Not sure which flags are required.
  win->init(res_w, res_h); // , 32, SDL_HWPALETTE | SDL_HWSURFACE | SDL_DOUBLEBUF);

  // 20 x 24 is the IDEAL arena dimension for the wilderness when the
  // resolution of the game is 1024x768.  Since the resolution is kept
  // dynamic, we have to calculate the arena dimension as follows.
  if (win->draw_frame((int)(20.0*((float)res_w)/1024.0), (int)(24.0*((float)res_h)/768.0)) != 0) {
      std::cerr << "Error: Canot create main window.\n";
      exit(EXIT_FAILURE);
  }
  
  // Create in-game text console for textual user interaction and game
  // output
  if (win->create_console_surface() != 0) {
      std::cerr << "Error: Canot create console window.\n";
      exit(EXIT_FAILURE);
  }

  // Create surface for ztats display
  if (win->create_ztats_surface() != 0) {
      std::cerr << "Error: Canot create ztats window.\n";
      exit(EXIT_FAILURE);
  }
 
  // Create surface for mini stats display
  if (win->create_mini_win_surface() != 0) {
      std::cerr << "Error: Canot create mini window.\n";
      exit(EXIT_FAILURE);
  }

  // Create surface for mini stats display
  if (win->create_tiny_win_surface() != 0) {
      std::cerr << "Error: Canot create tiny window.\n";
      exit(EXIT_FAILURE);
  }

  // Path to saved game
  boost::filesystem::path dir(std::string(getenv("HOME")) + "/.simplicissimus/");

  // Check if there's a saved game to return to and load it, if there is.
  if (boost::filesystem::exists(dir)) {
	  Console::Instance().print(&normalFont, "Restoring previous game state.", false);
	  Console::Instance().print(&normalFont, "", false);

	  dir /= "party.xml";
	  xmlpp::TextReader reader(dir.string());
      std::cout << "INFO: Loading game data from file " << dir.string() << std::endl;

      while (reader.read()) {
		  if (reader.get_node_type() != xmlpp::TextReader::xmlNodeType::EndElement && !reader.is_empty_element()) {
			  if (reader.get_name() == "x")
				  x = std::stoi(reader.read_string());
			  else if (reader.get_name() == "y")
				  y = std::stoi(reader.read_string());
			  else if (reader.get_name() == "map")
				  gc->set_map_name(reader.read_string().c_str());
			  else if (reader.get_name() == "indoors")
				  gc->set_outdoors(reader.read_string() == "0"? true : false);
			  else if (reader.get_name() == "gold")
				  party->set_gold(std::stoi(reader.read_string().c_str()));
			  else if (reader.get_name() == "food")
				  party->set_food(std::stoi(reader.read_string().c_str()));
			  // Inventory
			  else if (reader.get_name() == "inventory") {
				  while (reader.read() && reader.get_name() != "inventory") {
					  if (reader.get_node_type() != xmlpp::TextReader::xmlNodeType::EndElement && !reader.is_empty_element()) {
						  if (reader.get_name() == "item") {
							  reader.move_to_next_attribute();
							  int how_many = std::atoi(reader.get_value().c_str());

							  reader.move_to_element();
							  std::string item_name = reader.read_string();
							  std::string short_name = item_name.substr(item_name.find("::") + 2);
							  bool is_weapon = item_name.substr(0, item_name.find("::")) == "weapons";

							  // TODO: At the moment the party can only carry weapons, no herbs, food dishes, etc.  Fix this later!
							  if (is_weapon) {
								  for (int i = 0; i < how_many; i++)
									  party->inventory()->add(WeaponHelper::createFromLua(short_name));
							  }
							  else {
								  for (int i = 0; i < how_many; i++)
									  party->inventory()->add(ShieldHelper::createFromLua(short_name));
							  }

							  std::cout << "Items: " << reader.read_string() << how_many << std::endl;
						  }
					  }
				  }
			  }
			  // Players
			  else if (reader.get_name() == "players") {
				  while (reader.read() && reader.get_name() != "players") {
					  if (reader.get_name() == "player") {
						  PlayerCharacter player;

						  // Set player name from attribute
						  reader.move_to_next_attribute();
						  player.set_name(reader.get_value());

						  // Parse properties of player until </player> tag is found
						  while (reader.read() && reader.get_name() != "player") {
							  if (reader.get_node_type() != xmlpp::TextReader::xmlNodeType::EndElement && !reader.is_empty_element()) {
								  if (reader.get_name() == "profession") {
									  std::string prof = reader.read_string();
									  player.set_profession(stringToProfession.at(prof));
								  }
								  else if (reader.get_name() == "ep")
									  player.inc_ep(std::atoi(reader.read_string().c_str()));
								  else if (reader.get_name() == "hp")
									  player.set_hp(std::atoi(reader.read_string().c_str()));
								  else if (reader.get_name() == "hpm")
									  player.set_hpm(std::atoi(reader.read_string().c_str()));
								  else if (reader.get_name() == "sp")
									  player.set_sp(std::atoi(reader.read_string().c_str()));
								  else if (reader.get_name() == "spm")
									  player.set_spm(std::atoi(reader.read_string().c_str()));
								  else if (reader.get_name() == "str")
									  player.set_str(std::atoi(reader.read_string().c_str()));
								  else if (reader.get_name() == "luck")
									  player.set_luck(std::atoi(reader.read_string().c_str()));
								  else if (reader.get_name() == "dxt")
									  player.set_dxt(std::atoi(reader.read_string().c_str()));
								  else if (reader.get_name() == "wis")
									  player.set_wis(std::atoi(reader.read_string().c_str()));
								  else if (reader.get_name() == "charr")
									  player.set_char(std::atoi(reader.read_string().c_str()));
								  else if (reader.get_name() == "iq")
									  player.set_iq(std::atoi(reader.read_string().c_str()));
								  else if (reader.get_name() == "end")
									  player.set_end(std::atoi(reader.read_string().c_str()));
								  else if (reader.get_name() == "sex") {
									  int sex = std::atoi(reader.read_string().c_str())? 1 : 0;
									  player.set_sex(sex);
								  }
								  else if (reader.get_name() == "level")
									  player.set_level(std::atoi(reader.read_string().c_str()));
								  else if (reader.get_name() == "race")
									  player.set_race(static_cast<RACE>(std::atoi(reader.read_string().c_str())));
								  else if (reader.get_name() == "weapon") {
									  std::string weap_name = reader.read_string();
									  std::string short_name = weap_name.substr(weap_name.find("::") + 2);
									  player.set_weapon(WeaponHelper::createFromLua(short_name));
								  }
								  else if (reader.get_name() == "shield") {
									  std::string shield_name = reader.read_string();
									  std::string short_name = shield_name.substr(shield_name.find("::") + 2);
									  player.set_shield(ShieldHelper::createFromLua(short_name));
								  }
							  }
						  } // player-while-end

						  party->add_player(player);
					  }
				  } // players-while-end
			  } // players-else-if-end
		  }
      }

      ZtatsWin::Instance().update_player_list();
  }
  // Start fresh game with some dummy values
  else {
	  // TODO: For testing, add some party members.  All have an axe - how handy!
	  PlayerCharacter p1("Bilbo Baggins", 20, 11, 9, 16, 12, 15, 11, 16, 8, true, HOBBIT, THIEF);
	  party->add_player(p1);
	  party->get_player(0)->set_shield(ShieldHelper::createFromLua("small shield"));
	  PlayerCharacter p2("Gandalf", 12, 18, 10, 15, 12, 18, 16, 18, 12, true, HUMAN, MAGE);
	  party->add_player(p2);
	  party->get_player(1)->set_weapon(WeaponHelper::createFromLua("sword"));
	  PlayerCharacter p3("Aragorn", 23, 0, 17, 13, 13, 11, 14, 13, 15, true, HUMAN, FIGHTER);
	  party->add_player(p3);
	  party->get_player(2)->set_weapon(WeaponHelper::createFromLua("axe"));
	  ZtatsWin::Instance().update_player_list();

	  // Add some stuff to the inventory
	  party->inventory()->add(WeaponHelper::createFromLua("sword"));
	  party->inventory()->add(WeaponHelper::createFromLua("sword"));
	  party->inventory()->add(WeaponHelper::createFromLua("sword"));
	  party->inventory()->add(WeaponHelper::createFromLua("sword"));
	  party->inventory()->add(WeaponHelper::createFromLua("axe"));
	  party->inventory()->add(ShieldHelper::createFromLua("small shield"));

	  party->set_food(200);
	  party->set_gold(11);
  }

  // Load map data
  arena->get_map()->xml_load_map_data();

  // Draw map
  arena->set_SDL_surface(win->get_drawing_area_SDL_surface());
  arena->determine_offsets();

  // Set up game window and game control
  gc->set_arena(arena);
  gc->set_party(x,y);
  gc->set_outdoors(true);
  gc->set_map_name("Landschaft");
  gc->show_win();
  gc->draw_status();

  SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

  Console::Instance().
    print(&normalFont,
	  "Welcome to Simplicissimus!\nA game (c) Copyright by Working Dog Software.\nComments to baueran@gmail.com. Thanks!\n\n",
	  false);

  Console::Instance().
    print(&normalFont,
	  "This is pre-alpha software! Currently supported commands are:\n(a)ttack, (d)rop item, "
	  "(e)nter, (i)nventory, (l)ook, (p)pull/push, (q)uit, (r)eady item, (t)alk, (y)ield item, (z)tats.\n",
	  false);

  // Activate event handling
  SDL_TimerID tick;
  if (!(tick = em->add_event(500, tick_callback, NULL))) {
    std::cerr << "Could not initialize timer.\n";
    return -1;
  }

  // Start event handling
  gc->key_event_handler();

  // Close the game
  gc->close_win();
  lua_close(_lua_state);

  return 0;
}
