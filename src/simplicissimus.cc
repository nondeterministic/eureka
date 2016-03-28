// This source file is part of Simplicissimus
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
#include "map.hh"
#include "jimmylock.hh"
#include "weaponhelper.hh"
#include "shieldhelper.hh"
#include "util.hh"
#include "profession.hh"
#include "soundsample.hh"
#include "simplicissimus.hh"

#include <iostream>
#include <cstdlib>
#include <string>
#include <ctime>
#include <vector>
#include <memory>

#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/signals.hpp>
#include <boost/unordered_map.hpp>
#include <boost/algorithm/string.hpp>

#include <libxml++/libxml++.h>
#include <libxml++/parsers/textreader.h>

extern "C" {
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include <getopt.h>

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

static struct option long_options[] =
{
	{"help",         no_argument,       NULL, 1},
	{"width",        required_argument, NULL, 2},
	{"height",       required_argument, NULL, 3},
	{"version",      no_argument,       NULL, 4},
	{"datapath",     required_argument, NULL, 5},
	{"savegamepath", required_argument, NULL, 6},
	{"show-config",  no_argument,       NULL, 7},
	{0, 0, 0, 0}
};

int res_w = 1024, res_h=768;

std::string conf_world_name;
boost::filesystem::path conf_world_path;
boost::filesystem::path conf_data_path;
boost::filesystem::path conf_savegame_path;

// Only used for proper handling of getops
bool _getops_exit_after_print = false;
bool _getops_print_config = false;

// ******************************************************************************
// Function definitions
// ******************************************************************************

void get_opts (int, char*[]);
int start_game(int,int);
int intro(int, int);

// ******************************************************************************
// Main
// ******************************************************************************

int main(int argc, char *argv[])
{
	res_w = 1024;
	res_h = 768;
	// TODO 800x600 is not possible due to various graphic ratios.
	// 800x577 works though and should be a fall-back for small
	// displays such as EeePcs or other small, embedded devices.  The
	// game is intended to be played in 1024x768 anyway.
	// res_w = 800; res_h=577;

	// Read the user's command line options.
	if (argc > 1)
		get_opts (argc, argv);
	else {
		std::cerr << "ERROR: No world name given as command-line argument. Minimal call pattern is '"
				  << argv[0]
				  << " <WORLDNAME>'.\nTry '" << argv[0] << " --help' for further information.\n";
		exit(0);
	}

	// Remaining command line arguments (not options). i.e., worldname
	if (optind < argc) {
		while (optind < argc) {
			conf_world_name = argv[optind++];
			break;
		}
	}

	// Set some standard configurations
	conf_savegame_path = boost::filesystem::path((std::string)getenv("HOME"));
	conf_savegame_path /= ("." + (std::string)PACKAGE_NAME);

	conf_data_path = boost::filesystem::path((std::string)(DATADIR));
	conf_data_path /= (std::string)PACKAGE_NAME;
	conf_data_path /= "data";

	conf_world_path = boost::filesystem::path(conf_data_path / conf_world_name);

	// Finish up with getops
	if (_getops_print_config) {
		std::cout << "Current data path:                 " << conf_data_path << "\n";
		std::cout << "Current savegame path:             " << conf_savegame_path << "\n";
		std::cout << "Current world name (may be empty): " << conf_world_name << "\n";
		std::cout << "Current world path (may be empty): " << conf_world_path << "\n";
		std::cout << "Current game resolution:           " << res_w << "x" << res_h << "\n";
	}

	if (_getops_exit_after_print)
		exit(0);

	// Initialise random number generator
	std::srand(std::time(NULL));

	// Initialise Lua engine
	_lua_state = luaL_newstate();
	luaL_openlibs(_lua_state); /* Load Lua libraries */

	// Make the simplicissimus API visible to Lua
	publicize_api(_lua_state);

	// Show intro
	intro(res_w, res_h);

	// Start game
	return start_game(res_w, res_h);
}

// Evaluate command line arguments

void get_opts (int argc, char* argv[])
{
  int option = 0;
  int option_index = 0;

  while ((option = getopt_long(argc, argv, "chx:y:v", long_options, &option_index)) != -1) {
      switch (option) {
      	  case 'h':
      	  case 1:
      		  std::cout << PACKAGE_NAME << " - ye olde roleplaying game engine.\n";
      		  std::cout << "Lets you play games that have been designed for " << PACKAGE_NAME << "." << std::endl << std::endl;
      		  std::cout << "Usage:   " << argv[0] << " [OPTIONS] <WORLDNAME>" << std::endl;
      		  std::cout << "Example: " << argv[0] << " MyExcitingGameWorld" << std::endl << std::endl;
              std::cout << "If a long option shows an argument as mandatory, then it is mandatory for the equivalent short option also.\n\n";
              std::cout << "Options:" << std::endl;
              std::cout << "  -x <ARG>, --width=<ARG>        ";
              std::cout << "Set width of game window (default is 1024)\n";
              std::cout << "  -y <ARG>, --height=<ARG>       ";
              std::cout << "Set height of game window (default is 768)\n";
              std::cout << "            --datapath=<ARG>     ";
              std::cout << "Set data directory, which is also where all the available game worlds are stored (the default is where 'make install' put them)\n";
              std::cout << "  -c        --show-config        ";
              std::cout << "Show the current default configuration of " << PACKAGE_NAME << "\n";
              std::cout << "            --savegamepath=<ARG> ";
              std::cout << "Set the directory, where the saved games will end up in (default is $HOME/." << PACKAGE_NAME << "/)\n";
              std::cout << "  -h,       --help               ";
              std::cout << "Display this help information\n";
              std::cout << "  -v,       --version            ";
              std::cout << "Show version information\n\n";
              std::cout << "Report bugs to <baueran@gmail.com>.\n";
              _getops_exit_after_print = true;
              break;
      	  case 'x':
      	  case 2:
      		  res_w = std::stoi(optarg);
      		  break;
      	  case 'y':
      	  case 3:
      		  res_h = std::stoi(optarg);
      		  break;
      	  case 'v':
      	  case 4:
      		  std::cout << PACKAGE_NAME << " " << PACKAGE_VERSION << std::endl << std::endl;
      		  std::cout << "Copyright (c) 2007 - 2016  Andreas Bauer <baueran@gmail.com>\n\n";
      		  std::cout << "This is free software; see the source for copying conditions. There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n";
              _getops_exit_after_print = true;
              break;
      	  case 5:
      		  conf_data_path = boost::filesystem::path(optarg);
      		  break;
      	  case 6:
      		  conf_savegame_path = boost::filesystem::path(optarg);
      		  break;
      	  case 'c':
      	  case 7:
      		  _getops_print_config = true;
      		  _getops_exit_after_print = true;
      		  break;
      }
  }
}

int intro(int res_w, int res_h)
{
    SDL_Surface* img = NULL;

    boost::filesystem::path path_intro_pic((std::string)DATADIR);
    path_intro_pic /= PACKAGE_NAME;
    path_intro_pic /= "data";
    path_intro_pic /= "intro.png";

    if (!(img = IMG_Load(path_intro_pic.string().c_str())))
		std::cerr << "ERROR: Couldn't load frame png: " << IMG_GetError() << std::endl;

    // Initialize the SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
    	std::cerr << "ERROR: SDL_Init() Failed: " << SDL_GetError() << std::endl;
    	exit(1);
    }

	if (Mix_OpenAudio(22050,AUDIO_S16SYS,2,640) != 0)
		std::cerr << "ERROR: Could not initialize audio.\n";

	boost::filesystem::path path_intro_music(conf_world_path.string());
	path_intro_music /= "sound";
	path_intro_music /= "LocusIste.ogg";
	SoundSample sample_intro;
	sample_intro.set_channel(4711);
	sample_intro.set_volume(128);
	sample_intro.play(path_intro_music.string(), 1);

    // Set the video mode
    SDL_Surface* display = SDL_SetVideoMode(res_w, res_h, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    if (display == NULL) {
    	std::cerr << "ERROR: SDL_SetVideoMode() Failed: " << SDL_GetError() << std::endl;
    	exit(1);
    }

    if (SDL_BlitSurface(img, NULL, display, NULL) != 0) {
    	std::cerr << "ERROR: SDL_BlitSurface() Failed: " << SDL_GetError() << std::endl;
    	exit(1);
    }

    //Update the display
    SDL_Flip(display);

    // Main loop
    SDL_Event event;
    while(1) {
    	// Check for messages
    	if (SDL_PollEvent(&event)) {
    		// Check for the quit message
    		if (event.type == SDL_KEYUP) {
    			// Quit the program
    			break;
    		}
    	}
    }

    // Tell the SDL to clean up and shut down
    SDL_Quit();
    Mix_CloseAudio();

    return 0;
}

int start_game(int res_w, int res_h)
{
	EventManager* em = &EventManager::Instance();
	Party* party     = &Party::Instance();
	SDLWindow* win   = &SDLWindow::Instance();
	GameControl* gc  = &GameControl::Instance();
	Charset normalFont;

	int x = 13, y = 21; // Some default values for starting position in wilderness

	// Load game data
	if (! World::Instance().xml_load_world_data(conf_world_path.string() + ".xml")) {
		std::cerr << "ERROR: Error loading game data from " << conf_world_path.string() << ".xml" << ". Did you run make install?" << std::endl;
		return -1;
	}

	// Load Lua scripts, basically.
	World::Instance().load_world_elements(_lua_state);

	// Create an arena and load a map
	std::shared_ptr<Arena> arena;
	try {
		arena = Arena::create("outdoors", "Landschaft");
	}
	catch (const MapNotFound& e) {
		std::cerr << "ERROR: MapNotFound exception." << std::endl;
		exit(EXIT_FAILURE);
	}

	// Now create shared_ptr from raw pointer
	//  std::shared_ptr<HexArena> arena = std::dynamic_pointer_cast<HexArena>(_arena);

	// Create window
	// TODO: Not sure which flags are required.
	win->init(res_w, res_h); // , 32, SDL_HWPALETTE | SDL_HWSURFACE | SDL_DOUBLEBUF);

	// TODO
	SoundSample game_music;
	game_music.set_channel(4711);
	game_music.set_volume(128);
	game_music.play((conf_world_path / "sound" / "travel.ogg").string(), 1);

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
	// boost::filesystem::path dir(std::string(getenv("HOME")) + "/." + PACKAGE_NAME + "/");

	// Check if there's a saved game to return to and load it, if there is.
	if (boost::filesystem::exists(conf_savegame_path)) {
		Console::Instance().print(&normalFont, "Restoring previous game state.", false);
		Console::Instance().print(&normalFont, "", false);

		xmlpp::TextReader reader((conf_savegame_path / "party.xml").string());
		std::cout << "INFO: Loading game data from file " << (conf_savegame_path / "party.xml").string() << std::endl;

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
				else if (reader.get_name() == "jimmylocks") {
					int locks = std::stoi(reader.read_string().c_str());
					for (int l = 0; l < locks; l++)
						party->add_jimmylock();
				}
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
		PlayerCharacter p1("Bilbo Baggins", 20, 0, 9, 16, 12, 15, 11, 16, 8, true, 1, HOBBIT, THIEF);
		// PlayerCharacter p1("Bilbo Baggins", 2, 0, 9, 16, 12, 15, 11, 16, 8, true, HOBBIT, THIEF);
		party->add_player(p1);
		party->get_player(0)->set_shield(ShieldHelper::createFromLua("small shield"));
		PlayerCharacter p2("Gandalf", 12, 18, 10, 15, 12, 18, 16, 18, 12, true, 1, HUMAN, MAGE);
		// PlayerCharacter p2("Gandalf", 1, 18, 10, 15, 12, 18, 16, 18, 12, true, HUMAN, MAGE);
		party->add_player(p2);
		party->get_player(1)->set_weapon(WeaponHelper::createFromLua("sword"));
		// PlayerCharacter p3("Aragorn", 2, 0, 17, 13, 13, 11, 14, 13, 15, true, HUMAN, FIGHTER);
		PlayerCharacter p3("Aragorn", 23, 0, 17, 13, 13, 11, 14, 13, 15, true, 1, HUMAN, FIGHTER);
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
		party->add_jimmylock();
		party->add_jimmylock();
		party->add_jimmylock();

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
	gc->set_game_music(&game_music);

	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

	Console::Instance().
			print(&normalFont,
					"Welcome to " + (std::string)PACKAGE_NAME + "!\nA game engine (c) Copyright by Andreas Bauer.\nComments to baueran@gmail.com. Thanks!\n\n",
					false);

	Console::Instance().
			print(&normalFont,
					"This is alpha-status software! Currently supported commands are:\n(a)ttack, (c)ast spell, "
					"(d)rop item, (e)nter, (i)nventory, (l)ook, (o)pen, (p)pull/push, (q)uit, (r)eady item, "
					"(t)alk, (u)use item, (y)ield item, (z)tats.\n",
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
