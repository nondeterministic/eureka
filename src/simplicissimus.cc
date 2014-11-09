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
#include "arena.hh"
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

#include <iostream>
#include <cstdlib>
#include <string>
#include <ctime>
#include <vector>
#include <memory>
#include <boost/filesystem.hpp>
#include <boost/signals.hpp>
#include <boost/unordered_map.hpp>

extern "C" {
#include "SDL.h"
#include <lua.h>
#include <lualib.h>
#include <lualib.h>
#include <lauxlib.h>
}


// TODO: I know, global variables suck, but this makes the integration
// of Lua straightforward at least.

/*
boost::unordered_map< std::string, Weapon > weapons_map;
boost::unordered_map< std::string, Shield > shields_map;
*/
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

int main(int argc, char *argv[])
{
  EventManager* em = &EventManager::Instance();
  Party* party     = &Party::Instance();
  SDLWindow* win   = &SDLWindow::Instance();
  GameControl* gc  = &GameControl::Instance();

  int res_w = 1024, res_h=768;

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
  try { //make_shared<Song>(L"Elton John", L"I'm Still Standing");
	  arena = std::make_shared<Arena>(Arena::create("outdoors", "Landschaft"));
    // arena = Arena::create("outdoors", "Landschaft");
  }
  catch (const MapNotFound& e) {
    std::cerr << "Error: MapNotFound exception." << std::endl;
    exit(EXIT_FAILURE);
  }

  // Create window
  // TODO: Not sure which flags are required.
  win->init(res_w, res_h); // , 32, SDL_HWPALETTE | SDL_HWSURFACE | SDL_DOUBLEBUF);

  // 20 x 24 is the IDEAL arena dimension for the wilderness when the
  // resolution of the game is 1024x768.  Since the resolution is kept
  // dynamic, we have to calculate the arena dimension as follows.
  if (win->draw_frame((int)(20.0*((float)res_w)/1024.0), (int)(24.0*((float)res_h)/768.0)) != 0) 
    {
      std::cerr << "Error: Canot create main window.\n";
      exit(EXIT_FAILURE);
    }
  
  // Create in-game text console for textual user interaction and game
  // output
  if (win->create_console_surface() != 0)
    {
      std::cerr << "Error: Canot create console window.\n";
      exit(EXIT_FAILURE);
    }
 
  // Create surface for ztats display
  if (win->create_ztats_surface() != 0)
    {
      std::cerr << "Error: Canot create ztats window.\n";
      exit(EXIT_FAILURE);
    }
 
  // Create surface for mini stats display
  if (win->create_mini_win_surface() != 0)
    {
      std::cerr << "Error: Canot create mini window.\n";
      exit(EXIT_FAILURE);
    }

  // Create surface for mini stats display
  if (win->create_tiny_win_surface() != 0)
    {
      std::cerr << "Error: Canot create tiny window.\n";
      exit(EXIT_FAILURE);
    }

  // Load map data
  arena->get_map()->xml_load_map_data();

  // Draw map
  arena->set_SDL_surface(win->get_drawing_area_SDL_surface());
  arena->determine_offsets();

  // Set up game window and game control
  gc->set_arena(arena);
  gc->set_party(13, 21);  
  gc->set_outdoors(true);
  gc->set_map_name("Landschaft");
  gc->show_win();
  gc->draw_status();

  SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

  Charset normalFont;
  Console::Instance().
    print(&normalFont, 
	  "Welcome to Simplicissimus!\nA game (c) Copyright by Working Dog Software.\nComments to baueran@gmail.com. Thanks!\n\n",
	  false);

  Console::Instance().
    print(&normalFont, 
	  "This is pre-alpha software! Currently supported commands are:\n(d)rop item, "
	  "(e)nter, (i)nventory, (l)ook, (q)uit, (r)eady item, (t)alk, (y)ield item, (z)tats.\n",
	  false);

  // Activate event handling
  SDL_TimerID tick;
  if (!(tick = em->add_event(500, tick_callback, NULL))) {
    std::cerr << "Could not initialize timer.\n";
    return -1;
  }

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

/*
  std::cout<< "$$$$$$$$$$$$$$$$: Found: " << WeaponHelper::exists("axe") << std::endl;
  std::cout<< "$$$$$$$$$$$$$$$$: Found: " << WeaponHelper::exists("axe2") << std::endl;
  std::cout<< "$$$$$$$$$$$$$$$$: Found: " << WeaponHelper::exists("sword") << std::endl;
  std::cout<< "$$$$$$$$$$$$$$$$: Found: " << ShieldHelper::exists("small shield") << std::endl;
  std::cout<< "$$$$$$$$$$$$$$$$: Found: " << ShieldHelper::exists("small shield") << std::endl;
*/

  // Add some stuff to the inventory
  party->inventory()->add(WeaponHelper::createFromLua("sword"));
  party->inventory()->add(WeaponHelper::createFromLua("sword"));
  party->inventory()->add(WeaponHelper::createFromLua("sword"));
  party->inventory()->add(WeaponHelper::createFromLua("sword"));
  party->inventory()->add(WeaponHelper::createFromLua("axe"));
  party->inventory()->add(ShieldHelper::createFromLua("small shield"));

  party->set_food(200);
  party->set_gold(11);

  // Start event handling
  gc->key_event_handler();

  // Close the game
  gc->close_win();
  lua_close(_lua_state);
  
  return 0;
}
