// This source file is part of eureka
//
// Copyright (c) 2007-2017  Andreas Bauer <baueran@gmail.com>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
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
#include "edibleshelper.hh"
#include "util.hh"
#include "profession.hh"
#include "soundsample.hh"
#include "outdoorsmap.hh"
#include "indoorsmap.hh"
#include "gamestate.hh"
#include "indoorsicons.hh"
#include "outdoorsicons.hh"
#include "eureka.hh"

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
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_keycode.h>

#include <getopt.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

// ******************************************************************************
// Global variables and definitions
// ******************************************************************************

lua_State* _lua_state = NULL;

/// Create a custom TICK-event, which can then be received in the event loop
/// and reacted upon, e.g., to redraw icons.  grep for TICK, if you want to see
/// how this is used in the game!
Uint32 tick_callback(Uint32 interval, void* param)
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

static std::shared_ptr<Arena> arena;

// ******************************************************************************
// Function definitions
// ******************************************************************************

void get_opts (int, char*[]);
int intro(int, int);
int init_game_env(int,int);
PlayerCharacter create_character();
int start_game();
int setup_dummy_game(); // TODO: Remove this later again!
int create_fresh_game_state(PlayerCharacter);
int recreate_old_game_state();

// ******************************************************************************
// Util
// ******************************************************************************

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
      		  std::cout << "Usage:   " << argv[0] << " [OPTIONS] <WORLDNAME>" << std::endl << std::endl;
      		  std::cout << "Example: " << argv[0] << " Demoworld" << std::endl << std::endl;
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
      		  std::cout << "Copyright (c) 2007 - 2017  Andreas Bauer <baueran@gmail.com>\n\n";
      		  std::cout << "This is free software; see the source for copying conditions.\n"
      				    << "There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n";
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
		exit(EXIT_SUCCESS);
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
		exit(EXIT_SUCCESS);

	// Initialise random number generator
	std::srand(std::time(NULL));

	// Initialise Lua engine
	_lua_state = luaL_newstate();
	luaL_openlibs(_lua_state); /* Load Lua libraries */

	// Make the eureka API visible to Lua
	publicize_api(_lua_state);

	// Init sound and video, show intro
	intro(res_w, res_h);

	// Init screen, sound, world data, Lua, etc.
	init_game_env(res_w, res_h);

	// Set main theme
	boost::filesystem::path path_main_music(DATADIR);
	path_main_music /= PACKAGE_NAME;
	path_main_music /= "data";
	path_main_music /= "main.ogg";

	SoundSample game_music;
//	game_music.play(path_main_music.string(), 1);

	// Character creation?!
	EventManager* em = &EventManager::Instance();
	Charset normalFont;
	bool choice_is_made = false;
	while (!choice_is_made) {
		Console::Instance().print(&normalFont,
				"Current game world loaded is " + conf_world_name + ". Would you like to\n" +
				"(J)ourney onward\n" +
				"(C)reate a new game character, or\n" +
				"(Q)uit game?");

		switch (em->get_key("djcq")) {
		case 'd': // SECRET DEVELOPMENT CHOICE *EVIL LAUGHTER*
			choice_is_made = true;
			setup_dummy_game();
			break;
		case 'q':
			exit(EXIT_SUCCESS);
		case 'c':
			Console::Instance().print(&normalFont, "Creating new game character.\n", false);
			choice_is_made = true;
			create_fresh_game_state(create_character());
			break;
		case 'j':
			if (!boost::filesystem::exists(conf_savegame_path / "party.xml"))
				Console::Instance().print(&normalFont, "You don't seem to have a previously saved game in " + conf_savegame_path.string() + ".\n", false);
			else {
				choice_is_made = true;
				recreate_old_game_state();
				ZtatsWin::Instance().update_player_list();
			}
			break;
		}
	}

	//game_music.stop();
	return start_game();
}

// ******************************************************************************
// Rest of functions...
// ******************************************************************************

int intro(int res_w, int res_h)
{
	SDL_Window* window = NULL;
	SDL_Surface* img = NULL;
	SDL_Texture* texture = NULL;
    SDL_Renderer* renderer = NULL;

    // Initialize the SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {
    	std::cerr << "ERROR: SDL_Init() failed: " << SDL_GetError() << std::endl;
    	exit(EXIT_FAILURE);
    }

    if (!(window = SDL_CreateWindow(PACKAGE_STRING,
							SDL_WINDOWPOS_UNDEFINED,
							SDL_WINDOWPOS_UNDEFINED,
							res_w, res_h, SDL_WINDOW_SHOWN)))
    {
    	std::cerr << "ERROR: SDL_CreateWindow() failed: " << SDL_GetError() << std::endl;
    	exit(EXIT_FAILURE);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

    boost::filesystem::path path_intro_pic((std::string)DATADIR);
    path_intro_pic /= PACKAGE_NAME;
    path_intro_pic /= "data";
    path_intro_pic /= "intro.png";

    if (!(img = IMG_Load(path_intro_pic.string().c_str())))
		std::cerr << "ERROR: Couldn't load frame png: " << IMG_GetError() << std::endl;
	texture = SDL_CreateTextureFromSurface(renderer, img);
	SDL_RenderCopy(renderer, texture, NULL, NULL);

	// if (Mix_OpenAudio(22050,AUDIO_S16SYS,2,640) != 0)
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) != 0)
		std::cerr << "ERROR: Could not initialize audio.\n";
	Mix_AllocateChannels(16);

	boost::filesystem::path path_intro_music_path(DATADIR);
	path_intro_music_path /= PACKAGE_NAME;
	path_intro_music_path /= "data";
	path_intro_music_path /= "intro.ogg";
	SoundSample song_intro;
	song_intro.play(path_intro_music_path.string());

    // Update the display
    SDL_RenderPresent(renderer);

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

    song_intro.stop();

    // Tell the SDL to clean up and shut down
    Mix_CloseAudio();
    SDL_Quit();

    return 0;
}

int init_game_env(int res_w, int res_h)
{
	SDLWindow* win   = &SDLWindow::Instance();

	// Now create shared_ptr from raw pointer
	//  std::shared_ptr<HexArena> arena = std::dynamic_pointer_cast<HexArena>(_arena);

	// Create window
	if (win->init(res_w, res_h, SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) != 0) {
		std::cerr << "ERROR: Cannot initialise SDL.\n";
		exit(EXIT_FAILURE);
	}

	// Important, not to initialise at top, as the renderer of SDLWindow needs to be initialised in above init method.
	// Perhaps, fix that later! TODO?
	EventManager* em = &EventManager::Instance();
	Charset normalFont;

	// 20 x 24 is the IDEAL arena dimension for the wilderness when the
	// resolution of the game is 1024x768.  Since the resolution is kept
	// dynamic, we have to calculate the arena dimension as follows.
	//
	// (Also, creates arena_texture.)
	if (win->draw_frame((int)(20.0*((float)res_w)/1024.0), (int)(24.0*((float)res_h)/768.0)) != 0) {
		std::cerr << "ERROR: Cannot create main window.\n";
		exit(EXIT_FAILURE);
	}

	// Create in-game text console for textual user interaction and game output
	if (win->create_texture_console() != 0) {
		std::cerr << "ERROR: Cannot create console window.\n";
		exit(EXIT_FAILURE);
	}

	// Create surface for ztats display
	if (win->create_texture_ztats() != 0) {
		std::cerr << "ERROR: Cannot create ztats window.\n";
		exit(EXIT_FAILURE);
	}

	// Create surface for mini stats display
	if (win->create_texture_mini_win() != 0) {
		std::cerr << "ERROR: Cannot create mini window.\n";
		exit(EXIT_FAILURE);
	}

	// Create surface for mini stats display
	if (win->create_texture_tiny_win() != 0) {
		std::cerr << "ERROR: Cannot create tiny window.\n";
		exit(EXIT_FAILURE);
	}

	// TODO SDL: Disable or extend default non-delay of keyboard input!
	// SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

	Console::Instance().
			print(&normalFont,
					"Welcome to " + (std::string)PACKAGE_NAME +
					"!\nA game engine (c) Copyright by Andreas Bauer.\nComments to baueran@gmail.com. Thanks!\n\n",
					false);

	// Activate event handling
	SDL_TimerID tick;
	if (!(tick = em->add_event(500, tick_callback, NULL))) {
		std::cerr << "ERROR: Could not initialize timer.\n";
		return -1;
	}

	// Load game data
	if (! World::Instance().xml_load_world_data(conf_world_path.string() + ".xml")) {
		std::cerr << "ERROR: Error loading game data from " << conf_world_path.string() << ".xml"
				  << ". Did you run make install?" << std::endl;
		return -1;
	}

	// Load Lua scripts, basically.
	World::Instance().init_lua_arrays(_lua_state);
	World::Instance().set_spells(World::Instance().load_lua_spells(_lua_state));


	// It is important to do this once!
	if (IndoorsIcons::Instance().convert_icons_to_textures(win->get_renderer()) < 0) {
		std::cerr << "ERROR: Initialisation error. Could not convert indoors icons to textures.\n";
		exit(-1);
	}
	if (OutdoorsIcons::Instance().convert_icons_to_textures(win->get_renderer()) < 0) {
		std::cerr << "ERROR: Initialisation error. Could not convert outdoors icons to textures.\n";
		exit(-1);
	}

	return 0;
}

PlayerCharacter create_character()
{
	Party* party     = &Party::Instance();
	GameControl* gc  = &GameControl::Instance();
	EventManager* em = &EventManager::Instance();
	Charset normalFont;
	Console* cons = &Console::Instance();
	PlayerCharacter player;

	bool cont = false;
	while (!cont) {
		cons->print(&normalFont, "Is character (m)ale or (f)emale?", false);

		switch (em->get_key("mf")) {
		case 'f':
			player.set_sex(false);
			cons->print(&normalFont, "Female chosen.");
			break;
		case 'm':
			player.set_sex(true);
			cons->print(&normalFont, "Male chosen.");
			break;
		}

		cons->print(&normalFont, "Accept choice? (y/n)");
		switch (em->get_key("yn")) {
		case 'y':
			cont = true;
			break;
		}
	}

	cont = false;
	while (!cont) {
		cons->print(&normalFont, "Choose a race:\n1) Human\n2) Elf\n3) Hobbit\n4) Half Elf\n5) Dwarf", false);

		switch (em->get_key("12345")) {
		case '1':
			player.set_race(HUMAN);
			cons->print(&normalFont, "Human chosen.");
			break;
		case '2':
			player.set_race(ELF);
			cons->print(&normalFont, "Elf chosen.");
			break;
		case '3':
			player.set_race(HOBBIT);
			cons->print(&normalFont, "Hobbit chosen.");
			break;
		case '4':
			player.set_race(HALF_ELF);
			cons->print(&normalFont, "Half Elf chosen.");
			break;
		case '5':
			player.set_race(DWARF);
			cons->print(&normalFont, "Dawrf chosen.");
			break;
		}
		cons->print(&normalFont, "Accept choice? (y/n)");
		switch (em->get_key("yn")) {
		case 'y':
			cont = true;
			break;
		}
	}

	cont = false;
	while (!cont) {
		cons->print(&normalFont, "Choose a profession:\n(F)ighter\n(P)aladin\n(T)hief\n(B)ard\n(M)age\n(D)ruid\n(S)hepherd", false);

		switch (em->get_key("fptbmcds")) {
		case 'f':
			player.set_profession(FIGHTER);
			cons->print(&normalFont, "Fighter chosen.");
			break;
		case 'p':
			player.set_profession(PALADIN);
			cons->print(&normalFont, "Paladin chosen.");
			break;
		case 't':
			player.set_profession(THIEF);
			cons->print(&normalFont, "Thief chosen.");
			break;
		case 'b':
			player.set_profession(BARD);
			cons->print(&normalFont, "Bard chosen.");
			break;
		case 'm':
			player.set_profession(MAGE);
			cons->print(&normalFont, "Mage chosen.");
			break;
		case 'd':
			player.set_profession(DRUID);
			cons->print(&normalFont, "Druid chosen.");
			break;
		case 's':
			player.set_profession(SHEPHERD);
			cons->print(&normalFont, "Shepherd chosen.");
			break;
		}
		cons->print(&normalFont, "Accept choice? (y/n)");
		switch (em->get_key("yn")) {
		case 'y':
			cont = true;
			break;
		}
	}

	cont = false;
	int val = 0;
	while (!cont) {
		cons->print(&normalFont, "Randomly generated character values:\n", false);

		val = gc->random(10,20);
		player.set_hpm(val);
		player.set_hp(val);
		cons->print(&normalFont, "HP: " + std::to_string(val), false);

		if (player.is_spell_caster())
			val = gc->random(10,20);
		else
			val = 0;
		player.set_spm(val);
		player.set_sp(val);
		cons->print(&normalFont, "SP: " + std::to_string(val) + "\n", false);

		val = gc->random(3,18);
		player.set_str(val);
		cons->print(&normalFont, "Str: " + std::to_string(val), false);

		val = gc->random(3,18);
		player.set_end(val);
		cons->print(&normalFont, "End: " + std::to_string(val), false);

		val = gc->random(3,18);
		player.set_dxt(val);
		cons->print(&normalFont, "Dxt: " + std::to_string(val), false);

		val = gc->random(3,18);
		player.set_iq(val);
		cons->print(&normalFont, "Int: " + std::to_string(val), false);

		val = gc->random(3,18);
		player.set_luck(val);
		cons->print(&normalFont, "Lck: " + std::to_string(val), false);

		val = gc->random(3,18);
		player.set_wis(val);
		cons->print(&normalFont, "Wis: " + std::to_string(val), false);

		val = gc->random(3,18);
		player.set_char(val);
		cons->print(&normalFont, "Char: " + std::to_string(val), false);

		cons->print(&normalFont, "Accept values? (y/n)");
		switch (em->get_key("yn")) {
		case 'y':
			cont = true;
			break;
		}
	}

	cons->print(&normalFont, "You are almost done. Now choose a name for your player:\n", false);
	player.set_name(cons->gets());

	cons->print(&normalFont, "\nCharacter generation complete. Press any key to start game!\n", false);
	em->get_key();

	party->set_food(300);
	party->set_gold(25);
	party->add_player(player);

	return player;
}

// TODO: For testing, add some party members.  All have an axe - how handy!  Remove it later again.

int setup_dummy_game()
{
	int x = 13, y = 21;
	std::shared_ptr<Map> initial_map;
	Party* party     = &Party::Instance();

	// Determine initial game map
	try {
		initial_map = World::Instance().get_initial_map();
	}
	catch (const MapNotFound& e) {
		std::cerr << "ERROR: eureka.cc: Game world XML-file seems to have no initial map defined." << std::endl;
		exit(EXIT_FAILURE);
	}

	// Create an arena for initial game map
	try {
		if (std::dynamic_pointer_cast<OutdoorsMap>(initial_map)) {
			arena = Arena::create("outdoors", initial_map->get_name());
			std::cout << "INFO: eureka.cc: Starting outdoors game.\n";
		}
		else {
			arena = Arena::create("indoors", initial_map->get_name());
			std::cout << "INFO: eureka.cc: Starting indoors game.\n";
		}
	}
	catch (const MapNotFound& e) {
		std::cerr << "ERROR: eureka.cc: MapNotFound exception for map: " << initial_map->get_name() << std::endl;
		exit(EXIT_FAILURE);
	}

	// Load map data
	if (!arena->get_map()->xml_load_map_data()) {
		std::cerr << "ERROR: eureka.cc: Could not load map data1.\n";
		exit(EXIT_FAILURE);
	}

	// Determine initial game position
	try {
		x = initial_map->get_initial_coords().first;
		y = initial_map->get_initial_coords().second;
	}
	catch (NoInitialCoordsException& e)
	{
		std::cerr << "ERROR: eureka.cc: Exception thrown. No initial coordinates in initial game map defined." << std::endl;
		exit(EXIT_FAILURE);
	}

	PlayerCharacter p1("Bilbo Baggins", 20, 0, 9, 16, 12, 15, 11, 16, 8, true, 1, HOBBIT, THIEF);
	// PlayerCharacter p1("Bilbo Baggins", 2, 0, 9, 16, 12, 15, 11, 16, 8, true, HOBBIT, THIEF);
	party->add_player(p1);
	party->get_player(0)->set_shield(ShieldHelper::createFromLua("small shield", _lua_state));
	PlayerCharacter p2("Gandalf", 12, 18, 10, 15, 12, 18, 16, 18, 12, true, 1, HUMAN, MAGE);
	// PlayerCharacter p2("Gandalf", 1, 18, 10, 15, 12, 18, 16, 18, 12, true, HUMAN, MAGE);
	party->add_player(p2);
	party->get_player(1)->set_weapon(WeaponHelper::createFromLua("sword", _lua_state));
	// PlayerCharacter p3("Aragorn", 2, 0, 17, 13, 13, 11, 14, 13, 15, true, HUMAN, FIGHTER);
	PlayerCharacter p3("Aragorn", 23, 0, 17, 13, 13, 11, 14, 13, 15, true, 1, HUMAN, FIGHTER);
	party->add_player(p3);
	party->get_player(2)->set_weapon(WeaponHelper::createFromLua("axe", _lua_state));
	ZtatsWin::Instance().update_player_list();

	// Add some stuff to the inventory
	party->inventory()->add(WeaponHelper::createFromLua("sword", _lua_state));
	party->inventory()->add(WeaponHelper::createFromLua("sword", _lua_state));
	party->inventory()->add(WeaponHelper::createFromLua("sword", _lua_state));
	party->inventory()->add(WeaponHelper::createFromLua("sword", _lua_state));
	party->inventory()->add(WeaponHelper::createFromLua("axe", _lua_state));
	party->inventory()->add(ShieldHelper::createFromLua("small shield", _lua_state));
	party->inventory()->add(EdiblesHelper::createFromLua("garlic", _lua_state));
	party->inventory()->add(EdiblesHelper::createFromLua("sulphur", _lua_state));
	party->inventory()->add(EdiblesHelper::createFromLua("nightshade", _lua_state));
	party->add_jimmylock();
	party->add_jimmylock();
	party->add_jimmylock();

	party->set_food(300);
	party->set_gold(25);
	party->set_coords(x,y);
	std::cout << "INFO: eureka.cc: Setting initial party coords to " << x << ", " << y << ".\n";

	return 0;
}

int recreate_old_game_state()
{
	GameState* gstate = &GameState::Instance();
	std::shared_ptr<Map> cur_map;

	// Load game state from disk
	if (gstate->load(_lua_state))
		gstate->apply();
	else {
		std::cerr << "ERROR: eureka.cc: Loading of game file failed.\n";
		exit(EXIT_FAILURE);
	}

	// Now load referenced world data from disk
	try {
		cur_map = World::Instance().get_map(gstate->get_cur_map_name().c_str());
		std::cout << "CURRENT MAP NAME: " << gstate->get_cur_map_name() << "\n";
	}
	catch (const MapNotFound& e) {
		std::cerr << "ERROR: eureka.cc: Game world XML-file seems to have no initial map defined." << std::endl;
		exit(EXIT_FAILURE);
	}

	// Create an arena for current game map
	try {
		if (std::dynamic_pointer_cast<OutdoorsMap>(cur_map))
			arena = Arena::create("outdoors", cur_map->get_name());
		else
			arena = Arena::create("indoors", cur_map->get_name());
	}
	catch (const MapNotFound& e) {
		std::cerr << "ERROR: eureka.cc: MapNotFound exception for map: " << cur_map->get_name() << std::endl;
		exit(EXIT_FAILURE);
	}

	// Load current map's data
	if (!arena->get_map()->xml_load_map_data())
		std::cerr << "ERROR: eureka.cc: Could not load map data2.\n";

	return 0;
}

int create_fresh_game_state(PlayerCharacter player)
{
	Party* party     = &Party::Instance();
	std::shared_ptr<Map> initial_map;
	int x = -1; int y = -1;

	// Determine initial game map
	try {
		initial_map = World::Instance().get_initial_map();
	}
	catch (const MapNotFound& e) {
		std::cerr << "ERROR: eureka.cc: Game world XML-file seems to have no initial map defined." << std::endl;
		exit(EXIT_FAILURE);
	}

	// Create an arena for initial game map
	try {
		if (std::dynamic_pointer_cast<OutdoorsMap>(initial_map))
			arena = Arena::create("outdoors", initial_map->get_name());
		else
			arena = Arena::create("indoors", initial_map->get_name());
	}
	catch (const MapNotFound& e) {
		std::cerr << "ERROR: eureka.cc: MapNotFound exception for map: " << initial_map->get_name() << std::endl;
		exit(EXIT_FAILURE);
	}

	// Load map data
	if (!arena->get_map()->xml_load_map_data())
		std::cerr << "ERROR: eureka.cc: Could not load map data3.\n";

	// Determine initial game position
	try {
		x = initial_map->get_initial_coords().first;
		y = initial_map->get_initial_coords().second;
	}
	catch (NoInitialCoordsException& e)
	{
		std::cerr << "ERROR: eureka.cc: Exception thrown. No initial coordinates in initial game map defined." << std::endl;
		exit(EXIT_FAILURE);
	}

	if (x < 0 || y < 0) {
		std::cerr << "ERROR: eureka.cc: No initial coordinates in initial game map defined." << std::endl;
		exit(EXIT_FAILURE);
	}

	party->set_coords(x,y);
	ZtatsWin::Instance().update_player_list();

	return 0;
}

int start_game()
{
	SDLWindow*     win = &SDLWindow::Instance();
	GameControl*    gc = &GameControl::Instance();
	Party*       party = &Party::Instance();
	Charset normalFont;
	unsigned int initial_x = party->x;
	unsigned int initial_y = party->y;

	if (arena == NULL) {
		std::cerr << "ERROR: eureka.cc: arena pointer is NULL.\n";
		exit(EXIT_FAILURE);
	}

	// Draw map
	// Arena can simply use the singleton of SDLWindow itself!
	arena->set_SDLWindow_object(win);
	arena->determine_offsets();

	// Set up game window and game control
	gc->set_arena(arena);

	if (std::dynamic_pointer_cast<OutdoorsMap>(arena->get_map()) != NULL)
		gc->set_outdoors(true);
	else
		gc->set_outdoors(false);

	// Outdoors
	if (gc->is_arena_outdoors()) {
		std::cout << "INFO: eureka.cc: Setting initial party coords to " << 1 << ", " << 1 << ".\n";
		gc->set_party(0,0);// TODO: this 1 is only needed because we have coords in the wild like 42, 71 which is shit. They should be 42,70 and 43,71, etc.

		gc->set_map_name(arena->get_map()->get_name().c_str());
		std::cout << "INFO: eureka.cc: Setting map name to " << arena->get_map()->get_name() << ".\n";

		gc->redraw_graphics_arena();

		std::cout << "INFO: eureka.cc: Moving party quietly to " << initial_x << ", " << initial_y << ".\n";
		for (unsigned x = 0; x < initial_x; x++)
			gc->move_party(DIR_RIGHT, true);
		for (unsigned y = 0; y < initial_y / 2; y++)
			gc->move_party(DIR_DOWN, true);
	}
	// Indoors
	else {
		std::cout << "INFO: eureka.cc: Setting initial party coords to " << 1 << ", " << 1 << ".\n";
		gc->set_party(1,1);// TODO: this 1 is only needed because we have coords in the wild like 42, 71 which is shit. They should be 42,70 and 43,71, etc.

		gc->set_map_name(arena->get_map()->get_name().c_str());
		std::cout << "INFO: eureka.cc: Setting map name to " << arena->get_map()->get_name() << ".\n";

		gc->redraw_graphics_arena();

		for (unsigned x = 0; x < initial_x; x++)
			gc->move_party(DIR_RIGHT, true);
		for (unsigned y = 0; y < initial_y; y++)
			gc->move_party(DIR_DOWN, true);
	}

	gc->redraw_graphics_arena();
	gc->redraw_graphics_status();

	SoundSample game_music;
	game_music.play((conf_world_path / "sound" / "travel.ogg").string());

	gc->set_game_music(&game_music);

	Console::Instance().
			print(&normalFont,
					"\nRemember, this is alpha-status software! Currently supported commands are:\n(a)ttack, (c)ast spell, "
					"(d)rop item, (e)nter, (i)nventory, (l)ook, (m)ix spell, (o)pen, (p)ull/push, (q)uit, (r)eady item, "
					"(t)alk, (u)se item, (y)ield item, (z)tats.\n");

	// Set "start game" flag
	gc->set_game_started(true);

	// Start event handling
	gc->key_event_handler();

	// Close the game
	gc->close_win();
	lua_close(_lua_state);

	return 0;
}
