// This source file is part of eureka
//
// Copyright (c) 2007-2018 Andreas Bauer <a@pspace.org>
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
#include <algorithm>

#include <boost/random.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lualib.h>
#include <lauxlib.h>
}

// *******************************************
// *** Lookup keyboard symbols SDL_x here! ***
#include <SDL2/SDL_keycode.h>
// *******************************************

#include "eureka.hh"
#include "gamecontrol.hh"
#include "clock.hh"
#include "combat.hh"
#include "weapon.hh"
#include "weaponhelper.hh"
#include "edible.hh"
#include "edibleshelper.hh"
#include "armour.hh"
#include "armourhelper.hh"
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
#include "actionopened.hh"
#include "actiontake.hh"
#include "actionpullpush.hh"
#include "eventermap.hh"
#include "mapobj.hh"
#include "maphelper.hh"
#include "hexarena.hh"
#include "world.hh"
#include "miniwin.hh"
#include "tinywin.hh"
#include "ztatswincontentprovider.hh"
#include "ztatswin.hh"
#include "luaapi.hh"
#include "luawrapper.hh"
#include "soundsample.hh"
#include "itemfactory.hh"
#include "item.hh"
#include "miscitem.hh"
#include "util.hh"
#include "conversation.hh"
#include "gamestate.hh"
#include "gameeventhandler.hh"
#include "spell.hh"
#include "spellcasthelper.hh"
#include "potion.hh"
#include "potionshelper.hh"
#include "objectmover.hh"
#include "config.h"

using namespace std;

GameControl::GameControl()
{
	_normal_font = &Charset::Instance();

	_game_music = NULL;
	_em = &EventManager::Instance();
	_party = &Party::Instance();
	_turn_passed = 0;
	_turns = 0;
	_generator.seed(std::time(NULL)); // seed with the current time
	_game_is_started = false;
}

GameControl& GameControl::Instance()
{
  static GameControl _inst;
  return _inst;
}

void GameControl::set_game_started(bool value)
{
	_game_is_started = value;
}

bool GameControl::get_game_started()
{
	return _game_is_started;
}

void GameControl::set_game_music(SoundSample* gm)
{
	_game_music = gm;
}

int GameControl::set_party(int x, int y)
{
	_party->set_coords(x, y);
	_arena->map_to_screen(_party->x, _party->y, screen_pos_party.first, screen_pos_party.second);
	_arena->show_party(screen_pos_party.first, screen_pos_party.second);
	return 0;
}

/**
 * Returns 0 on success, a negative value if no map can be drawn for whatever reason
 */

int GameControl::redraw_graphics_arena()
{
	if (_arena != NULL && _arena->get_map() != NULL) {
		_arena->show_map(get_viewport().first, get_viewport().second);
		_arena->show_party(screen_pos_party.first, screen_pos_party.second);
		_arena->blit();
	    return 0;
	}

	// If the above fails, this is only an error, if the game is running.
	// Otherwise, it may be quite OK, not to have a map loaded, etc.
	if (get_game_started()) {
		std::cerr << "WARNING: gamecontrol.cc: show_win() failed. Arena or map is null.\n";
		return -1;
	}
	else
		return 0;
}

/**
 * If update_status_image is true (which is its default param), then not only the tiny win is updated
 * but also the small status win that displays city images, monsters, etc.
 */

void GameControl::redraw_graphics_status(bool update_status_image)
{
	MiniWin& mwin = MiniWin::Instance();

	std::stringstream ss;
	__attribute__ ((unused)) int moon_icon = 0;  // TODO
	static std::string filename;
	static std::string filename_old;

	filename_old = filename;

	ss << "Gold: " << _party->gold();
	ss << ", Food: " << _party->food();
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

	if (update_status_image) {
		boost::filesystem::path tmp_path = conf_world_path / "images";

		if (_party->indoors()) {
			if (get_map()->is_dungeon)
				filename = "dungeon.png";
			else
				filename = "indoors_city.png";
		}

		// To avoid reloading image on each step...
		// if (filename_old != filename)
		mwin.surface_from_file((tmp_path / filename).c_str());
	}
	TinyWin& twin = TinyWin::Instance();
	twin.clear();
	twin.println(0, ss.str());
	twin.blit();

	// Print moon symbol
	// twin.printch(twin.get_surface()->w - 16, 0, moon_icon);
	// twin.blit();
}

int GameControl::set_arena(std::shared_ptr<Arena> new_arena)
{
	_arena = new_arena;
	return 0;
}

std::shared_ptr<Arena> GameControl::get_arena()
{
	return _arena;
}

bool GameControl::game_won()
{
	LuaWrapper lua(global_lua_state);

	if (lua.check_item_prop_is_nilornone(std::vector<std::string> { "game_won" })) {
		if (luaL_dofile(global_lua_state, (conf_world_path / "game_won.lua").c_str())) {
			std::cerr << "WARNING: gamecontrol.cc: Couldn't execute Lua file: " << lua_tostring(global_lua_state, -1) << ". Game not properly installed or incomplete?\n";
			return false;
		}
	}

	return lua.call_fn<bool>("game_won");
}

void GameControl::do_turn(Resting resting)
{
	if (game_won()) {
		printcon("CONGRAULATIONS! YOU HAVE WON THE GAME. PRESS SPACE TO EXIT.");
		_em->get_key(" ");
		exit(EXIT_SUCCESS);
	}

	ZtatsWin& zwin = ZtatsWin::Instance();

	_turns++;
	_turn_passed = 0;

	// Consume food when not resting
	if (resting == No) {
		if (_party->food() == 0) {
			if (is_arena_outdoors()) {
				if (_turns%20 == 0) {
					bool starving = false;
					for (int i = 0; i < _party->size(); i++) {
						PlayerCharacter* pl = _party->get_player(i);
						pl->set_hp(max(0, pl->hp() - 1));
						starving = true;
					}
					if (starving) {
						_sample.play_predef(HIT);
						printcon("Your party is starving.");
					}
					zwin.update_player_list();
				}
			}
			else {
				if (_turns%40 == 0) {
					bool starving = false;
					for (int i = 0; i < _party->size(); i++) {
						PlayerCharacter* pl = _party->get_player(i);
						pl->set_hp(max(0, pl->hp() - 2));
						starving = true;
					}
					if (starving) {
						_sample.play_predef(HIT);
						printcon("Your party is starving.");
					}
					zwin.update_player_list();
				}
			}
		}
		else {
			if (is_arena_outdoors()) {
				if (_turns%20 == 0) {
					_party->set_food(max(0, _party->food() - _party->size() * 2));
					zwin.update_player_list();
				}
			}
			else {
				if (_turns%40 == 0) {
					_party->set_food(max(0, _party->food() - _party->size()));
					zwin.update_player_list();
				}
			}
		}
	}

	// Decrease duration of ongoing spells
	_party->decrease_spells(global_lua_state);
	for (int i = 0; i < _party->size(); i++) {
		PlayerCharacter* pl = _party->get_player(i);
		pl->decrease_spells(global_lua_state, pl->name());
	}

	// Restore spell points
	if (is_arena_outdoors()) {
		if (_turns%20 == 0) {
			for (int i = 0; i < _party->size(); i++) {
				PlayerCharacter* pl = _party->get_player(i);
				if (pl->is_spell_caster() && pl->sp() < pl->spm()) {
					pl->set_sp(pl->sp() + 1);
					zwin.update_player_list();
				}
			}
		}
	}
	else {
		if (_turns%40 == 0) {
			for (int i = 0; i < _party->size(); i++) {
				PlayerCharacter* pl = _party->get_player(i);
				if (pl->is_spell_caster() && pl->sp() < pl->spm()) {
					pl->set_sp(pl->sp() + 1);
					zwin.update_player_list();
				}
			}
		}
	}

	// Is party starved to death?
	if (_party->party_alive() == 0)
		game_over();

	// Check if random combat ensues and handle it in case
	if (is_arena_outdoors()) {
		// Increment clock by 5 minutes every turn when outdoors, time doesn't elapse indoors
		if (_turns % 10 == 0)
			_clock.inc(30);

		// Significantly lower the chances of unwanted combat when party is resting
		if ((resting == Yes && random(1,6) == 1) || resting == No) {
			Combat combat;
			Combat_Return_Codes combat_result = combat.initiate();
			if (combat_result == Combat_Return_Codes::VICTORY ||
				combat_result == Combat_Return_Codes::FLED ||
				combat_result == Combat_Return_Codes::OTHER)
			{
				redraw_graphics_status(true);
			}
			else
				redraw_graphics_status();
		}
	}
	else {
		if (_turns%35 == 0)
			_clock.inc(30);
	}

	// Check if torches, etc. need to be destroyed
	for (int i = 0; i < _party->size(); i++) {
		PlayerCharacter* pl = _party->get_player(i);
		if (pl->weapon() != NULL) {
			if (pl->weapon()->destroy_after() == 1) {
				printcon(pl->name() + " throws away the " + pl->weapon()->name() + " as it no longer fulfills its purpose. (PRESS SPACE BAR TO CONFIRM!)");
				_em->get_key(" ");
				printcon("OK, burnt down torch is gone. (Beware: " + pl->name() + "is now empty-handed...)");

				// Now delete memory of item
				Weapon* wep = pl->weapon();
				pl->set_weapon(NULL);
				delete wep;
			}
			else if (pl->weapon()->destroy_after() > 1)
				pl->weapon()->destroy_after(pl->weapon()->destroy_after() - 1);
		}
	}

	// Check intoxication
	if (_party->rounds_intoxicated > 0)
		_party->rounds_intoxicated--;

	// Reduce immunised from (magic/poison/etc) fields in each round.
	if (_party->immune_from_fields() > 0)
		if (_party->decrease_immunity_from_fields() == 0)
			printcon("Your party feels somehow less protected again...");

	// Check poisoned status
	if (_turns % 2 == 0 && random(1,2) == 2) {
		for (int i = 0; i < _party->size(); i++) {
			PlayerCharacter* pl = _party->get_player(i);

			if (pl->condition() == POISONED) {
				pl->set_hp(max(0, pl->hp() - 1));
				if (pl->hp() == 0)
					pl->set_condition(DEAD);
				_sample.play_predef(HIT);
				zwin.update_player_list();
			}
		}
	}

	// Has party been poisoned to death?
	if (_party->party_alive() == 0)
		game_over();

	redraw_graphics_status();

	/////////////////////////////////////////////////////////////////////////////////////////////
	// TODO: If we wanted to, we could garbage collect the Lua stack, say, every 100 turns or so.
	// This code works and was tested before.
	//	lua_close(_lua_state);
	//	_lua_state = luaL_newstate();
	//	luaL_openlibs(_lua_state);
	//	publicize_api(_lua_state);
	//  World::Instance().init_lua_arrays(_lua_state);
	/////////////////////////////////////////////////////////////////////////////////////////////
}

int GameControl::tick_event_handler()
{
	Console::Instance().animate_cursor(_normal_font);

	if (!_arena->is_moving())
		return redraw_graphics_arena();

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

	while (true) {
		// SDL_Delay(1000/25); // If frames were 30, wait 33 ms before running the loop again
	    // SDL_RenderPresent(SDLWindow::Instance().get_renderer());

	    if (SDL_WaitEvent(&event)) {
			if (event.type == SDL_USEREVENT) {
				if (event.user.code == TICK) {
					tick_event_handler();
					tick_event_turn_handler();
				}
			}
			else if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
				switch (event.key.keysym.sym) {
				case SDLK_LEFT:
				case SDLK_KP_4:
					keypress_move_party(DIR_LEFT);
					break;
				case SDLK_RIGHT:
				case SDLK_KP_6:
					keypress_move_party(DIR_RIGHT);
					break;
				case SDLK_DOWN:
				case SDLK_KP_2:
					keypress_move_party(DIR_DOWN);
					break;
				case SDLK_UP:
				case SDLK_KP_8:
					keypress_move_party(DIR_UP);
					break;
				case SDLK_KP_7:
					keypress_move_party(DIR_LUP);
					break;
				case SDLK_KP_1:
					keypress_move_party(DIR_LDOWN);
					break;
				case SDLK_KP_3:
					keypress_move_party(DIR_RDOWN);
					break;
				case SDLK_KP_9:
					keypress_move_party(DIR_RUP);
					break;
				case SDLK_SPACE:
					printcon("Pass");
					do_turn();
					break;
				case SDLK_a:
					keypress_attack();
					break;
				case SDLK_c:
					if (event.key.keysym.mod == KMOD_RCTRL || event.key.keysym.mod == KMOD_LCTRL)
						std::cout << "INFO: gamecontrol.cc: Party-coords: " << _party->x << ", " << _party->y << "\n";
					else
						keypress_cast();
					break;
				case SDLK_d:
					keypress_drop_items();
					break;
				case SDLK_e:
					keypress_enter();
					break;
				case SDLK_g:
					keypress_get_item();
					break;
				case SDLK_h:
					keypress_hole_up();
					break;
				case SDLK_i:
					keypress_inventory();
					break;
				case SDLK_l:
					keypress_look();
					break;
				case SDLK_m:
					keypress_mix_reagents();
					break;
				case SDLK_o:
					keypress_open_act();
					break;
				case SDLK_p:
					keypress_pull_push();
					break;
				case SDLK_q:
					keypress_quit();
					break;
				case SDLK_r:
					printcon("Ready item - select player");
					keypress_ready_item(zwin.select_player());
					break;
				case SDLK_s:
					keypress_sort();
					break;
				case SDLK_t:
					keypress_talk();
					break;
				case SDLK_EQUALS:
					printcon("Toggling music");
					_game_music->toggle();
					break;
				case SDLK_u:
					keypress_use();
					break;
				case SDLK_w:
					keypress_wave_goodbye();
					break;
				case SDLK_x:
					keypress_xit();
					break;
				case SDLK_y: // yield / unready item
					printcon("Yield (let go of) item - select player");
					keypress_yield_item(zwin.select_player());
					break;
				case SDLK_z:
					keypress_ztats();
					break;
				default:
					printf("INFO: gamecontrol.cc: key_handler::default: %d (hex: %x)\n", event.key.keysym.sym, event.key.keysym.sym);
					break;
				}

				// Move objects, e.g., attacking monsters hunting the party
				ObjectMover obj_mover;
				obj_mover.move();

				// If there are hostile monsters next to the party, they may want to attack now...
				get_attacked();

				// Create random monsters in dungeons
				create_random_monsters_in_dungeon();

				// After handling a key stroke it is almost certainly a good idea to update the screen
				_arena->show_map(get_viewport().first, get_viewport().second);
				_arena->show_party(screen_pos_party.first, screen_pos_party.second);

			    if (SDLWindow::Instance().blit_all() < 0)
			    	std::cerr << "WARNING: gamecontrol.cc: blit_all failed.\n";
			}
		}
	}
	return 0;
}

void GameControl::keypress_sort()
{
	ZtatsWin& zwin = ZtatsWin::Instance();
	printcon("Sort party - select player");
	zwin.rearrange();
}

void GameControl::keypress_cast()
{
	ZtatsWin& zwin = ZtatsWin::Instance();

	printcon("Cast spell - select player");
	int cplayer = zwin.select_player();

	if (cplayer >= 0 && !_party->get_player(cplayer)->is_npc()) {
		PlayerCharacter* player = _party->get_player(cplayer);

		if (player->condition() == DEAD) {
			printcon("Next time try picking an alive party member.");
			return;
		}

		if (!player->is_spell_caster()) {
			printcon(player->name() + " does not have magic abilities.");
			return;
		}

		std::string spell_file_path = select_spell(cplayer);

		if (spell_file_path.length() > 0)
			cast_spell(cplayer, Spell::spell_from_file_path(spell_file_path, global_lua_state));
		else
			printcon("Never mind.");
	}
	else if (cplayer >= 0 && _party->get_player(cplayer)->is_npc())
		printcon("Cannot control NPCs.");
	else
		printcon("Never mind.");
}

/// "Civilian" use of magic during non-combat...

void GameControl::cast_spell(int player_no, Spell spell)
{
	PlayerCharacter* player = _party->get_player(player_no);

	if (player->sp() < spell.sp)
		printcon(player->name() + " does not have enough spell points.");
	else if (player->level() < spell.level)
		printcon("A player with level " + std::to_string(player->level()) + " cannot cast spells that require level " + std::to_string(spell.level) + ".");
	else {
		SpellCastHelper sch(player_no, global_lua_state);
		sch.set_spell_path(spell.full_file_path);

		if (sch.is_attack_spell_only()) {
			printcon("Your party is not engaged in battle.");
			return;
		}

		// The basic spell casting pattern...
		sch.init();
		try {
			sch.choose(); // Not all spells have a choose function, e.g., light.  Heal has a choose function for selecting the party member.
		}
		catch(const NoChooseFunctionException& ex) {
			std::cout << "INFO: gamecontrol.cc: Civilian use of spell without choose() function.\n";
		}
		sch.execute();
	}
}

/// Returns the full file path of the chosen spell, otherwise "" if no spell was selected.
///
/// PRECONDITION: Assumes that player_no refers to a magic user!  Otherwise the string will be empty!
///               So check the magic use before calling this!

std::string GameControl::select_spell(unsigned player_no)
{
	MiniWin& mwin = MiniWin::Instance();
	ZtatsWin& zwin = ZtatsWin::Instance();
	PlayerCharacter* player = _party->get_player(player_no);

	std::shared_ptr<ZtatsWinContentSelectionProvider<Spell>> content_selection_provider = player->create_spells_content_selection_provider();

	// This should never happen!  See precondition comment on top of function!
	// (But just in case... The program won't crash at least.  It will just think
	// that the player cancelled the spell selection - which is odd behaviour.)
	if (content_selection_provider->get_page().size() == 0)
		return "";

	printcon("Cast - select a spell");

	mwin.save_texture();
	mwin.clear();
	mwin.println(0, "Cast spell (SP: " + std::to_string(player->sp()) + "/" + std::to_string(player->spm()) + ")", CENTERALIGN);
	mwin.println(1, "(Press space to cast selected spell, q to exit)");

	std::vector<Spell> chosen_spells = zwin.execute(content_selection_provider.get(), SelectionMode::SingleItem);

	if (chosen_spells.size() == 0)
		return "";
	return chosen_spells[0].full_file_path;
}

void GameControl::keypress_quit()
{
	EventManager& em = EventManager::Instance();

	// TODO: Enable save game indoors as well!!! Check GameState for missing functionality in this regard!
	if (!_party->indoors()) {
		printcon("Save game? (y/n)");
		char save_game = em.get_key("yn");
		printcon(std::string(1, save_game) + " ");

		if (save_game == 'y')
			GameState::Instance().save();
	}
	else
		printcon("Cannot save game when indoors.");

	printcon("Quit game? (y/n)");
	char really_quit = em.get_key("yn");
	printcon(std::string(1, really_quit) + " ");

	if (really_quit == 'y')
		exit(EXIT_SUCCESS);
}

void GameControl::keypress_mix_reagents()
{
	MiniWin& mwin = MiniWin::Instance();
	ZtatsWin& zwin = ZtatsWin::Instance();

	std::shared_ptr<ZtatsWinContentSelectionProvider<Item*>> ztatswin_contentprovider =
			_party->inventory()->create_content_selection_provider(InventoryType::MagicHerbs);

	if (ztatswin_contentprovider->get_page().size() == 0) {
		printcon("You have no reagents to mix.");
		return;
	}

	printcon("Mix reagents for magic potion - select player");

	int selected_player = zwin.select_player();

	if (selected_player == -1) {
		printcon("Changed your mind then, I guess?!");
		return;
	}

	if (_party->get_player(selected_player)->is_npc()) {
		printcon("Cannot control NPCs");
		return;
	}

	if (!_party->get_player(selected_player)->is_spell_caster()) {
		printcon("Chosen party member does not possess the required magic abilities.");
		return;
	}

	mwin.save_texture();
	mwin.clear();
	mwin.println(0, "Mix for magic potion", CENTERALIGN);
	mwin.println(1, "(Scroll up/down/left/right, press q to exit)", CENTERALIGN);

	std::vector<Item*> ingredients = zwin.execute(ztatswin_contentprovider.get(), SelectionMode::MultipleItems);

	if (ingredients.size() == 0) {
		printcon("Nothing selected, therefore nothing mixed.");
		return;
	}

	std::vector<std::string> ingredient_names;
	std::vector<Potion*> all_potions = PotionsHelper::get_loaded_lua_potions(global_lua_state); // Don't forget to delete those temporary potions later!

	for (Item* item: ingredients)
		ingredient_names.push_back(item->name());

	bool success = false;
	for (Potion* potion: all_potions) {
		if (potion->consists_of(ingredient_names)) {
			_party->inventory()->add(potion);  // Adding potion to party inventory.
			printcon("Successfully mixed one " + potion->name() + ".");
			success = true;

			// TODO: Remove ingredients from inventory...
			// ...
		}
		else
			delete potion; // Deleting temp. potion.
	}

	if (!success)
		printcon("Unfortunately, this combination of ingredients yielded nothing.");

	mwin.display_last();
}

void GameControl::keypress_ztats()
{
	ZtatsWin& zwin = ZtatsWin::Instance();
	printcon("Ztats - select player");

	int selected_player = zwin.select_player();
	if (selected_player != -1) {
		MiniWin& mwin = MiniWin::Instance();

		mwin.save_texture();
		mwin.clear();
		mwin.println(0, "Ztats", CENTERALIGN);
		mwin.println(1, "(Scroll up/down/left/right, press q to exit)", CENTERALIGN);

		std::shared_ptr<ZtatsWinContentProvider> ztatswin_contentprovider = _party->create_party_content_provider();
		zwin.execute(ztatswin_contentprovider.get(), selected_player);

		mwin.display_last();
	}
}

void GameControl::keypress_wave_goodbye()
{
	ZtatsWin& zwin = ZtatsWin::Instance();
	printcon("Wave good-bye - select NPC");

	int chosen_player = zwin.select_player();
	if (chosen_player < 0) {
		printcon("Changed your mind then?");
		return;
	}

	auto player = _party->get_player(chosen_player);
	if (player->is_npc()) {
		printcon("Are you sure you want to let " + player->name() + " go? This is irreversible! (y/n)");

		switch (_em->get_key("yn")) {
		case 'n':
			return;
		case 'y':
			_party->rm_npc(chosen_player);
			zwin.update_player_list();
		}
	}
	else
		printcon("Sorry, this is only applicable to NPCs.");
}

void GameControl::keypress_xit()
{
	if (!_party->is_entered()) {
		printcon("Nothing to exit from.");
		return;
	}

	std::cout <<_party->get_entered_icon() <<"\n";
	IconProps* icon_props = IndoorsIcons::Instance().get_props(_party->get_entered_icon());
	printcon("Leaving " + icon_props->get_name());
	_party->set_entered(-1);
}

void GameControl::keypress_enter()
{
	// Check if party is on an enterable object, e.g., a ship
	auto found_objs = _arena->get_map()->objs()->equal_range(std::make_pair(_party->x,_party->y));
	for (auto curr_obj = found_objs.first; curr_obj != found_objs.second; curr_obj++) {
		MapObj& map_obj = curr_obj->second;
		IconProps* icon_props = IndoorsIcons::Instance().get_props(map_obj.get_icon());

		if (icon_props->is_enterable()) {
			printcon("Entering " + icon_props->get_name());
			_party->set_entered(icon_props->get_icon());
			return;
		}
	}

	// Check if party is on enterable map icon, i.e., if there is an enter-action associated to it.
	std::vector<std::shared_ptr<Action>> acts = _arena->get_map()->get_actions(_party->x, _party->y);

	if (acts.size() > 0) {
		bool entered = false;
		for (auto act : acts) {
			if (std::dynamic_pointer_cast<ActionOnEnter>(act)) {
				action_on_enter(std::dynamic_pointer_cast<ActionOnEnter>(act));
				entered = true;
			}
		}
		if (!entered) {
			printcon("Nothing to enter");
			return;
		}
	}

	printcon("Nothing to enter");
}

void GameControl::keypress_inventory()
{
	if (_party->inventory()->size() == 0) {
		printcon("Inventory is empty. You are presently not carrying any special items.");
		return;
	}

	MiniWin& mwin = MiniWin::Instance();
	ZtatsWin& zwin = ZtatsWin::Instance();

	printcon("Inventory");

	mwin.save_texture();
	mwin.clear();
	mwin.println(0, "Inventory", CENTERALIGN);
	std::stringstream ss;
	ss << "Weight: " << _party->inventory()->weight() << (_party->inventory()->weight() == 1? " stone" : " stones");
	ss << "   Max. capacity: " << _party->max_carrying_capacity() << " stones";
	mwin.println(1, ss.str());

	std::shared_ptr<ZtatsWinContentProvider> ztatswin_contentprovider = _party->inventory()->create_content_provider(InventoryType::Anything);
	zwin.execute(ztatswin_contentprovider.get());

	mwin.display_last();
}

/// Let go of item and put it back to inventory.

void GameControl::keypress_yield_item(int selected_player)
{
	MiniWin& mwin = MiniWin::Instance();
	ZtatsWin& zwin = ZtatsWin::Instance();

	if (selected_player >= 0 && !_party->get_player(selected_player)->is_npc()) {
		PlayerCharacter* player = _party->get_player(selected_player);

		mwin.save_texture();
		mwin.clear();
		mwin.println(0, "Yield (let go of) item", CENTERALIGN);
		mwin.println(1, "(Press space to select, q to exit)", CENTERALIGN);

		// Create ContentSelectionProvider...
		ZtatsWinContentSelectionProvider<Item*> ztatswincontentselectionprovider;
		std::vector<pair<StringAlignmentTuple, Item*>> content_page;
		const Alignment AL = Alignment::LEFTALIGN;

		if (player->weapon())
			content_page.push_back(std::pair<StringAlignmentTuple,Item*>(StringAlignmentTuple("Weapon: " +  player->weapon()->name(), AL), player->weapon()));
		else
			content_page.push_back(std::pair<StringAlignmentTuple,Item*>(StringAlignmentTuple("Weapon: <none>", AL), NULL));

		if (player->armour())
			content_page.push_back(std::pair<StringAlignmentTuple,Item*>(StringAlignmentTuple("Armour: " + player->armour()->name(), AL), player->armour()));
		else
			content_page.push_back(std::pair<StringAlignmentTuple,Item*>(StringAlignmentTuple("Armour: <none>", AL), NULL));

		if (player->shield())
			content_page.push_back(std::pair<StringAlignmentTuple,Item*>(StringAlignmentTuple("Shield: " + player->shield()->name(), AL), player->shield()));
		else
			content_page.push_back(std::pair<StringAlignmentTuple,Item*>(StringAlignmentTuple("Shield: <none>", AL), NULL));

		if (player->armour_head())
			content_page.push_back(std::pair<StringAlignmentTuple,Item*>(StringAlignmentTuple("Head: " + player->armour_head()->name(), AL), player->armour_head()));
		else
			content_page.push_back(std::pair<StringAlignmentTuple,Item*>(StringAlignmentTuple("Head: <none>", AL), NULL));

		if (player->armour_feet())
			content_page.push_back(std::pair<StringAlignmentTuple,Item*>(StringAlignmentTuple("Feet: " + player->armour_feet()->name(), AL), player->armour_feet()));
		else
			content_page.push_back(std::pair<StringAlignmentTuple,Item*>(StringAlignmentTuple("Feet: <none>", AL), NULL));

		if (player->armour_hands())
			content_page.push_back(std::pair<StringAlignmentTuple,Item*>(StringAlignmentTuple("Hands: " + player->armour_hands()->name(), AL), player->armour_hands()));
		else
			content_page.push_back(std::pair<StringAlignmentTuple,Item*>(StringAlignmentTuple("Hands: <none>", AL), NULL));

		content_page.push_back(std::pair<StringAlignmentTuple,Item*>(StringAlignmentTuple("Other:  <none>", AL), NULL)); // TODO: Rings, torches, etc.

		// Now execute selection provider...
		ztatswincontentselectionprovider.add_content_page(content_page);
		std::vector<Item*> selected_items = zwin.execute(&ztatswincontentselectionprovider, SelectionMode::SingleItem);

		if (selected_items.size() == 0 || selected_items[0] == NULL) {
			printcon("Never mind...");
			return;
		}

		Item* selected_item = selected_items[0];

		if (dynamic_cast<Weapon*>(selected_item)) {
			if (player->weapon())
				_party->inventory()->add(player->weapon());
			player->set_weapon(NULL);
		}
		else if (dynamic_cast<Shield*>(selected_item)) {
			if (player->shield())
				_party->inventory()->add(player->shield());
			player->set_shield(NULL);
		}
		else if (dynamic_cast<Armour*>(selected_item)) {
			Armour* armour = dynamic_cast<Armour*>(selected_item);

			if (armour->is_gloves()) {
				if (player->armour_hands())
					_party->inventory()->add(player->armour_hands());
				player->set_armour_hands(NULL);
			}
			else if (armour->is_helmet()) {
				if (player->armour_head())
					_party->inventory()->add(player->armour_head());
				player->set_armour_head(NULL);
			}
			else if (armour->is_shoes()) {
				if (player->armour_feet())
					_party->inventory()->add(player->armour_feet());
				player->set_armour_feet(NULL);
			}
			else {
				if (player->armour())
					_party->inventory()->add(player->armour());
				player->set_armour(NULL);
			}
		}

		// After yielding an item, the AC may have changed, for example.
		zwin.update_player_list();
		printcon("Yielded " + selected_item->name());
		return;
	}
	else if (selected_player >= 0 && _party->get_player(selected_player)->is_npc()) {
		printcon("Cannot control NPCs.");
		return;
	}

	printcon("Never mind...");
	return;
}

/// Rest party

void GameControl::keypress_hole_up()
{
	MiniWin& mwin = MiniWin::Instance();
	ZtatsWin& zwin = ZtatsWin::Instance();

	mwin.save_texture();
	mwin.clear();

	printcon("Hole up and camp. For how many hours? (0-9)");
	char chours = _em->get_key("0123456789");
	int hours = atoi(&chours);
	printcon(std::to_string(hours));

	if (hours == 0) {
		printcon("Changed your mind, huh?");
		redraw_graphics_status(true);
		return;
	}
	else if (hours > 9) {
		printcon("More than 9 hours of sleep isn't healthy.");
		redraw_graphics_status(true);
		return;
	}

	printcon("Do you want to set up a guard? (y/n)");
	int selected_player = -1;

	switch (_em->get_key("yn")) {
	case 'n':
		printcon("n");
		break;
	case 'y':
		printcon("y");
		printcon("Select a guard");
		selected_player = zwin.select_player();
		break;
	}

	// If player was chosen, set guard
	PlayerCharacter* guard = NULL;
	if ((guard = _party->get_player(selected_player)) != NULL) {
		if (guard->condition() == PlayerCondition::DEAD)
			printcon("Cannot choose dead player. No one will stand guard.");
		else if (guard->is_npc())
			printcon("Cannot choose NPC. No one will stand guard.");
		else {
			printcon(_party->get_player(selected_player)->name() + " will stand guard.");
			_party->set_guard(selected_player);
		}
	}
	else
		printcon("No guard - at your own risk!");

	_party->is_resting = true;

	pair<int,int> old_time = _clock.time();
	int rounds = 0;
	do {
		do_turn(Resting::Yes);

		for (int i = 0; i < _party->size(); i++) {
			PlayerCharacter* pl = _party->get_player(i);

			// Do actual party healing and spell point recharging
			if (pl->condition() != DEAD) {
				if (pl->hp() < pl->hpm()) {
					if (is_arena_outdoors() && rounds % 3 == 0) {
						pl->set_hp(pl->hp() + 1);
						zwin.update_player_list();
					}
					else if (!is_arena_outdoors() && rounds % 10 == 0) {
						pl->set_hp(pl->hp() + 1);
						zwin.update_player_list();
					}
				}
				if (pl->is_spell_caster() && pl->sp() < pl->spm()) {
					if (is_arena_outdoors() && rounds % 3 == 0) {
						pl->set_sp(pl->sp() + 1);
						zwin.update_player_list();
					}
					else if (!is_arena_outdoors() && rounds % 10 == 0) {
						pl->set_sp(pl->sp() + 1);
						zwin.update_player_list();
					}
				}
			}
		}

		SDLWindow::Instance().blit_entire_window_texture();
		rounds++;
	} while (_clock.time().first != (old_time.first + hours) % 24);

	// Unset guard again
	_party->unset_guard();
	_party->is_resting = false;

	redraw_graphics_status(true);
	zwin.update_player_list();
}

void GameControl::keypress_open_act()
{
	printcon("Open - in which direction?");
	std::pair<int,int> coords = select_coords();

	auto avail_objects = _arena->get_map()->objs()->equal_range(coords);

	// If the <actions> block of the_obj has an open-action defined, execute it and return true!
	// Otherwise, this Lambda returns false.
	auto execute_object_actions = [](MapObj& the_obj, std::shared_ptr<Arena> arena) {
		for (auto curr_act = the_obj.actions()->begin(); curr_act != the_obj.actions()->end(); curr_act++) {
			ActionOpened* action_opened = dynamic_cast<ActionOpened*>((*curr_act).get());

			if (action_opened != NULL) {
				GameEventHandler gh;
				for (auto curr_ev: action_opened->get_events())
					gh.handle(curr_ev, arena->get_map(), &the_obj);
				return true; // Handled openeing-action successfully.
			}
		}
		return false; // No opening-action found.
	};


	// TODO: If there are more than one openable item in one place, all are opened in succession.
	// However, I can't think of a scenario, where this would occur/be a problem.

	if (avail_objects.first != avail_objects.second) {
		for (auto curr_obj = avail_objects.first; curr_obj != avail_objects.second; curr_obj++) {
			MapObj& the_obj = curr_obj->second;
			int icon = the_obj.get_icon();

			// If the object has the "locked" property defined...
			if (the_obj.openable) {
				if (the_obj.lock_type == NORMAL_LOCK || the_obj.lock_type == MAGIC_LOCK) {
					printcon("You try, but it seems locked.");
					return;
				}
				else {
					std::cout << "INFO: gamecontrol.cc: Executing actions for opening an item...\n";
					execute_object_actions(the_obj, _arena);
					return;
				}
			}
			// We implement some default behaviour for doors here: if they're not explicitly set in their object
			// properties as locked, magic, etc. we let the user simply open them.
			else if (IndoorsIcons::Instance().get_props(icon)->get_name().find("a closed door") != std::string::npos) {
				std::cout << "INFO: gamecontrol.cc: Default object-delete-event for doors triggered.\n";
				GameEventHandler gh;
				gh.handle_event_delete_object(_arena->get_map(), &the_obj);
				return;
			}
			else if (IndoorsIcons::Instance().get_props(icon)->get_name().find("closed") != std::string::npos &&
						IndoorsIcons::Instance().get_props(icon)->get_name().find("chest") != std::string::npos)
			{
				std::cout << "INFO: gamecontrol.cc: Default object-delete-event for chests triggered.\n";

				// Either do the map-defined actions (see Lambda above!) or default-delete chest icon and add opened chest icon...
				if (execute_object_actions(the_obj, _arena) == false) {
					GameEventHandler gh;
					gh.handle_event_delete_object(_arena->get_map(), &the_obj);
					MapObj open_chest;
					open_chest.set_icon(489);
					open_chest.set_coords(coords.first, coords.second);
					gh.handle_event_add_object(_arena->get_map(), open_chest);
				}
				return;
			}
		}
	}

	printcon("Nothing to open here.");
	return;
}

bool GameControl::unlock_item()
{
	if (!_party->indoors()) {
		printcon("Unlock - nothing to unlock here.");
		return false;
	}

	ZtatsWin& zwin = ZtatsWin::Instance();
	printcon("Unlock - in which direction?");
	std::pair<int,int> coords = select_coords();

	printcon("Choose a player to attempt the unlocking of item.");
	int chosen_player = zwin.select_player();
	if (chosen_player < 0) {
		printcon("Changed your mind then?");
		return false;
	}

	if (_party->get_player(chosen_player)->is_npc()) {
		printcon("Cannot control NPCs.");
		return false;
	}

	auto avail_objects = _arena->get_map()->objs()->equal_range(coords);
	if (avail_objects.first != avail_objects.second) {
		for (auto curr_obj = avail_objects.first; curr_obj != avail_objects.second; curr_obj++) {
			MapObj& the_obj = curr_obj->second;
			// IconProps* props = IndoorsIcons::Instance().get_props(the_obj.get_icon());

			if (the_obj.openable) {
				if (the_obj.lock_type == NORMAL_LOCK) {
					printcon("Wow, you did it!");
					the_obj.lock_type = UNLOCKED;
					return true;
				}
				else if (the_obj.lock_type == MAGIC_LOCK) {
					printcon("It seems, this needs more than just a jimmy lock.");
					return false;
				}
				else {
					printcon("Not locked. Don't waste a perfectly good jimmy lock on it.");
					return false;
				}
			}
			else {
				printcon("Nothing to open here.");
				return false;
			}
		}
	}

	return false;
}

void GameControl::keypress_use()
{
	MiniWin& mwin = MiniWin::Instance();
	ZtatsWin& zwin = ZtatsWin::Instance();

	mwin.save_texture();
	mwin.clear();
	mwin.println(0, "Use item", CENTERALIGN);
	mwin.println(1, "(Press space to select, q to exit)", CENTERALIGN);

	std::shared_ptr<ZtatsWinContentSelectionProvider<Item*>> zwin_content_selection_provider = _party->inventory()->create_content_selection_provider(InventoryType::Anything);
	std::vector<Item*> selected_items = zwin.execute(zwin_content_selection_provider.get(), SelectionMode::SingleItem);

	// User can either select exactly one item, or will have aborted the dialogue.
	if (selected_items.size() == 1) {
		Item* selected_item = selected_items[0];
		printcon(selected_item->name());

		if (WeaponHelper::existsInLua(selected_item->name(), global_lua_state))
			printcon("Try to (r)eady a weapon instead.");
		else if (ShieldHelper::existsInLua(selected_item->name(), global_lua_state))
			printcon("Try to (r)eady a shield instead.");
		else if (ArmourHelper::existsInLua(selected_item->name(), global_lua_state))
			printcon("Try to (r)eady an armour instead.");
		else if (selected_item->name() == "jimmy lock" || selected_item->name() == "key") {
			if (unlock_item())
				_party->rm_jimmylock();
		}
		else if (PotionsHelper::existsInLua(selected_item->name(), global_lua_state)) {
			PotionsHelper potions_helper;
			printcon("Drink potion - select player");
			potions_helper.drink((Potion*)selected_item, global_lua_state);

			// Remove potion from inventory
			_party->inventory()->remove(selected_item->name(), selected_item->description());
		}
		else if (EdiblesHelper::existsInLua(selected_item->name(), global_lua_state)) {
			EdiblesHelper edibles_helper;
			edibles_helper.eat_party((Edible*)selected_item);

			// Remove one such edible item from inventory
			_party->inventory()->remove(selected_item->name(), selected_item->description());
		}
		else
			printcon("You cannot use that.");
	}
	else
		printcon("Changed your mind, huh?");

	redraw_graphics_status(true);
}

void GameControl::game_over()
{
	printcon("GAME OVER. ALL ARE DEAD. PRESS SPACE TO EXIT.");
	_em->get_key(" ");
	exit(EXIT_SUCCESS);
}

std::string GameControl::keypress_ready_item(int selected_player)
{
	MiniWin& mwin = MiniWin::Instance();
	ZtatsWin& zwin = ZtatsWin::Instance();

	if (selected_player >= 0 && !_party->get_player(selected_player)->is_npc()) {
		mwin.save_texture();
		mwin.clear();
		mwin.println(0, "Ready item", CENTERALIGN);
		mwin.println(1, "(Press space to select, q to exit)", CENTERALIGN);

		std::shared_ptr<ZtatsWinContentSelectionProvider<Item*>> content_provider_selection = _party->inventory()->create_content_selection_provider(InventoryType::Wearables);

		if (content_provider_selection->get_page().size() > 0) {
			std::vector<Item*> selected_items = zwin.execute(content_provider_selection.get(), SelectionMode::SingleItem);

			if (selected_items.size() > 0) {
				PlayerCharacter* player = _party->get_player(selected_player);
				Item* selected_item = selected_items[0];
				std::string selected_item_name = selected_item->name();

				if (WeaponHelper::existsInLua(selected_item_name, global_lua_state)) {
					Weapon* weapon = WeaponHelper::createFromLua(selected_item_name, global_lua_state);

					if (player->weapon() != NULL) {
						printcon("Yielding " + player->weapon()->name());
						_party->inventory()->add(player->weapon());
					}

					if (weapon->hands() == 2 && player->shield() != NULL) {
						printcon("Yielding " + player->shield()->name());
						_party->inventory()->add(player->shield());
						player->set_shield(NULL);
					}

					// ...and now we are freeing memory for a weapon with the same name in the inventory.
					// A tad bit complicated, perhaps, but not overly difficult to understand.
					// Besides, it makes it very explicit what is going on, and I like that.
					// TODO: Alternatively, one could create a method Inventory::handOver(std::string weapon_name),
					// which removes the weapon from the inventory list and returns its pointer so that the
					// memory remains allocated and it can be passed on e.g. to a player or elsewhere.
					_party->inventory()->remove(weapon->name(), weapon->description());
					player->set_weapon(weapon);
				}
				else if (ShieldHelper::existsInLua(selected_item_name, global_lua_state)) {
					Shield* shield = ShieldHelper::createFromLua(selected_item_name, global_lua_state);

					if (player->shield() != NULL) {
						printcon("Yielding " + player->shield()->name());
						_party->inventory()->add(player->shield());
					}

					else if (player->weapon()->hands() == 2 && player->weapon() != NULL) {
						printcon("Yielding " + player->weapon()->name());
						_party->inventory()->add(player->weapon());
						player->set_weapon(NULL);
					}

					_party->inventory()->remove(shield->name(), shield->description());
					player->set_shield(shield);
				}
				else if (ArmourHelper::existsInLua(selected_item_name, global_lua_state)) {
					Armour* armour = ArmourHelper::createFromLua(selected_item_name, global_lua_state);

					if (armour->is_gloves()) {
						if (player->armour_hands() != NULL)
							_party->inventory()->add(player->armour());
						player->set_armour_hands(armour);
					}
					else if (armour->is_helmet()) {
						if (player->armour_head() != NULL)
							_party->inventory()->add(player->armour());
						player->set_armour_head(armour);
					}
					else if (armour->is_shoes()) {
						if (player->armour_feet() != NULL)
							_party->inventory()->add(player->armour());
						player->set_armour_feet(armour);
					}
					else {
						if (player->armour() != NULL)
							_party->inventory()->add(player->armour());
						player->set_armour(armour);
					}

					_party->inventory()->remove(armour->name(), armour->description());
				}
				else
					std::cerr << "WARNING: gamecontrol.cc: readying an item that cannot be recognised. This is serious business.\n";

				// After readying an item, the AC may have changed, for example.
				zwin.update_player_list();
				printcon("Readying " + selected_item_name);
				redraw_graphics_status(true);

				return selected_item_name;
			}
		}
	}
	else if (selected_player >= 0 && _party->get_player(selected_player)->is_npc()) {
		printcon("Cannot control NPCs.");
		return "";
	}
	else
		std::cerr << "INFO: gamecontrol.cc: Contentprovider was empty, while your inventory probably wasn't? Smells like a (harmless) program error.\n";

	redraw_graphics_status(true);
	printcon("Never mind...");
	return "";
}

/// Displays a movable cursor and returns coordinates of map where the user places it and presses return.
/// Returns std::pair(-1,-1) if users cancelled selection.

std::pair<int, int> GameControl::select_coords()
{
	EventManager& em = EventManager::Instance();
	const int CROSSHAIR_ICON = (_party->indoors()? CROSSHAIR_ICON_INDOORS : CROSSHAIR_ICON_OUTDOORS);

	std::list<SDL_Keycode> cursor_keys =
		{ SDLK_LEFT, SDLK_RIGHT, SDLK_UP, SDLK_DOWN, SDLK_RETURN, SDLK_q, SDLK_ESCAPE, SDLK_KP_4,
		  SDLK_KP_6, SDLK_KP_8, SDLK_KP_2 /*, SDLK_KP_1, SDLK_KP_3, SDLK_KP_7, SDLK_KP_9 */ };

	static int cx, cy;  // Cursor
	int px, py;         // Party

	_arena->map_to_screen(_party->x, _party->y, px, py);
	_arena->screen_to_map(px, py, px, py);
	cx = px;
	cy = py;

	int old_x = cx, old_y = cy;

	MapObj crosshair_tmp_obj;
	crosshair_tmp_obj.set_coords(cx,cy);
	crosshair_tmp_obj.lua_name = CROSSHAIR_ICON_LUA_NAME;
	crosshair_tmp_obj.set_icon(CROSSHAIR_ICON);

	_arena->get_map()->push_obj(crosshair_tmp_obj);

	while (1) {
	    // If frames were 30, wait 33 ms before running the loop again
	    SDL_Delay(1000/25);
		SDL_RenderPresent(SDLWindow::Instance().get_renderer());

		switch (em.get_generic_key(cursor_keys)) {
		case SDLK_LEFT:
		case SDLK_KP_4:
			if (_arena->adjacent(cx - 1, cy, px, py) && cx - 1 > 0)
				cx--;
			break;
		case SDLK_RIGHT:
		case SDLK_KP_6:
			if (_arena->adjacent(cx + 1, cy, px, py) && cx + 1 <= (int)_arena->get_map()->width() - 4)
				cx++;
			break;
		case SDLK_UP:
		case SDLK_KP_8:
			if (_party->indoors() && _arena->adjacent(cx, cy - 1, px, py) && cy - 1 > 0)
				cy--;
			else if (!_party->indoors() && _arena->adjacent(cx, cy - 2, px, py) && cy - 2 > 0)
				cy -= 2;
			break;
		case SDLK_DOWN:
		case SDLK_KP_2:
			if (_party->indoors() && _arena->adjacent(cx, cy + 1, px, py) &&
					cy + 1 <= (int)_arena->get_map()->height() - 4)
				cy++;
			else if (! _party->indoors() && _arena->adjacent(cx, cy + 2, px, py) &&
					cy + 2 <= (int)_arena->max_y_coordinate() - 2)
				cy += 2;
			break;
		case SDLK_RETURN:
			_arena->get_map()->rm_obj(&crosshair_tmp_obj);
			return std::make_pair(cx, cy);
		case SDLK_ESCAPE:
		case SDLK_q:
			_arena->get_map()->rm_obj(&crosshair_tmp_obj);
			return std::make_pair(-1, -1);
		default:
			std::cout << "INFO: gamecontrol.cc: Pressed unhandled key.\n";
		}

		if (! _party->indoors()) {
			if ( (cx % 2) == 0 && (cy % 2)  != 0 ) {
				if (_arena->adjacent(cx, cy - 1, px, py))
					cy--;
				else {
					cx = old_x;
					cy = old_y;
				}
			}
			else if ( (cx % 2) != 0 && (cy % 2)  == 0 ) {
				if (_arena->adjacent(cx, cy + 1, px, py))
					cy++;
				else {
					cx = old_x;
					cy = old_y;
				}
			}
		}

		_arena->get_map()->rm_obj(&crosshair_tmp_obj);
		crosshair_tmp_obj.set_coords(cx, cy);
		_arena->get_map()->push_obj(crosshair_tmp_obj);
		old_x = cx; old_y = cy;
	}
}

void GameControl::keypress_drop_items()
{
	if (_party->inventory()->size() == 0) {
		printcon("Inventory is empty. You are presently not carrying any special items.");
		return;
	}

	MiniWin& mwin = MiniWin::Instance();
	ZtatsWin& zwin = ZtatsWin::Instance();

	printcon("Drop item - select which one");

	mwin.save_texture();
	mwin.clear();
	mwin.println(0, "Drop item", CENTERALIGN);
	mwin.println(1, "(Press space to drop selected item, q to exit)");

	auto zwin_content_selection_provider = _party->inventory()->create_content_selection_provider(InventoryType::Anything);
	std::vector<Item*> selected_items = zwin.execute(zwin_content_selection_provider.get(), SelectionMode::SingleItem);

	// User can either select exactly one item, or will have aborted the dialogue.
	if (selected_items.size() == 1) {
		Item* selected_item = selected_items[0];
		unsigned selected_item_size = _party->inventory()->how_many_of(selected_item->name(), selected_item->description());
		unsigned drop_how_many = 1;

		if (selected_item_size > 1) {
			printcon("How many? (1-" + std::to_string(selected_item_size) + ")");
			std::string reply = Console::Instance().gets();
			try {
				if (reply.length() == 0)
					drop_how_many = selected_item_size;
				else {
					drop_how_many = std::stoi(reply);
					if (!(drop_how_many >= 1 && drop_how_many <= selected_item_size)) {
						printcon("Huh? Nothing dropped.");
						return;
					}
				}
			}
			catch (...) {
				printcon("Huh? Nothing dropped.");
				return;
			}
		}

		// Create corresponding icon if party is indoors
		if (_party->indoors()) {
			MapObj moTmp;

			// Some MiscItems have a MapObj associated with them, e.g., nonmagicscroll.
			if (dynamic_cast<MiscItem*>(selected_item)) {
				MiscItem* tmp_item = dynamic_cast<MiscItem*>(selected_item);
				try {
					if (tmp_item != NULL)
						moTmp = tmp_item->get_obj();
					else
						std::cerr << "ERROR: gamecontrol.cc: Drop item dynamic cast failed.\n";
				}
				catch (...) {
					; // Do nothing, this simply means, the tmp_item had no MapObj associated with it, which is OK! See MiscItem.cc for details.
				}
			}

			moTmp.set_coords(_party->x, _party->y);
			moTmp.set_icon(selected_item->icon);
			moTmp.set_description(selected_item->description());
			if (moTmp.description().length() == 0)
				if ((moTmp.lua_name = selected_item->get_lua_name()).length() == 0)
					std::cerr << "ERROR: gamecontrol.cc: Dropped an item with no Lua name. You will not be able to pick it up again!\n";
			moTmp.how_many = drop_how_many;

			// Add dropped item to current map
			_arena->get_map()->push_obj(moTmp);
		}

		std::stringstream ss;
		if (drop_how_many == 1)
			ss << "Dropped a" << (Util::vowel(selected_item->name()[0])? "n " : " ") << selected_item->name() << ".";
		else
			ss << "Dropped " << drop_how_many << " " << selected_item->plural_name() << ".";
		printcon(ss.str());

		for (int i = 0; i < max((int)drop_how_many, 1); i++)
			_party->inventory()->remove(selected_item->name(), selected_item->description());
	}

	mwin.display_last();
}

/// Makes all guards of a town turn hostile (e.g., after committing a crime), or neutral, etc.

void GameControl::make_guards(PERSONALITY pers)
{
	for (auto map_obj_pair = _arena->get_map()->objs()->begin(); map_obj_pair != _arena->get_map()->objs()->end(); map_obj_pair++) {
		MapObj* map_obj = &(map_obj_pair->second);

		// If MapObj ID contains the string "guard", it is a guard and will be set to hostile
		if (map_obj->id.find("guard") != std::string::npos)
			map_obj->personality = pers;
	}
}

/// Random monsters in dungeons work differently to outdoors: they don't simply appear, but are placed as objects
/// randomly near the party on the map and stay there, are saved as objects, etc.
///
/// (I could have added this to indoorsmap.cc but then leibniz has yet another few nasty dependencies and I would
///  have to do a few type casts here that wouldn't be so nice. So we simply create the monsters directly here.)

void GameControl::create_random_monsters_in_dungeon()
{
	if (!_arena->get_map()->is_dungeon)
		return;

	int rand_monster_count = 0;

	// If there are already too many random monsters, don't generate more!
	for (auto map_obj_pair = _arena->get_map()->objs()->begin(); map_obj_pair != _arena->get_map()->objs()->end(); map_obj_pair++) {
		MapObj* map_obj = &(map_obj_pair->second);
		if (map_obj->is_random_monster)
			rand_monster_count++;
	}

	if (rand_monster_count >= 2)
		return;

	const int min_distance_to_party = 7; // Monsters should not directly pop up next to the party
	for (int xoff = -20; xoff < 20; xoff++) {
		for (int yoff = -20; yoff < 20; yoff++) {
			int monst_x = _party->x + xoff;
			int monst_y = _party->y + yoff;

			// Monsters should not directly pop up next to the party
			if (abs(_party->x - monst_x) >= min_distance_to_party || abs(_party->y - monst_y) < min_distance_to_party)
				continue;

			if (!walkable(monst_x, monst_y))
				continue;

			MapObj monster;
			monster.set_origin(monst_x, monst_y);
			monster.set_coords((unsigned)monst_x, (unsigned)monst_y);
			monster.is_random_monster = true;

			// Determine type of monster using Lua
			LuaWrapper lua(global_lua_state);
			lua.push_fn_arg(std::string("dungeon"));
			int stack_change = lua.call_fn_leave_ret_alone(std::vector<std::string> { "rand_encounter" });

			// Iterate through result table (see also combat.cc for where I originally copied this more or less from)
			lua_pushnil(global_lua_state);
			while (lua_next(global_lua_state, -2) != 0) {
				lua_pushnil(global_lua_state);
				while (lua_next(global_lua_state, -2) != 0) {
					// Only create first monster, in case bestiary/defs.lua spits out a whole array of monsters...
					// So as soon as combat script is defined, skip the rest of the result set.
					if (monster.get_combat_script_path().length() == 0) {
						string __key = lua_tostring(global_lua_state, -2);

						// Name of monster
						if (__key == "__name") {
							std::string __name = lua_tostring(global_lua_state, -1);

							// ***********************************************************************************
							// TODO: When switching to Boost 3, use boost::filesystem::relative instead!
							// http://www.boost.org/doc/libs/1_61_0/libs/filesystem/doc/reference.html#op-relative
							monster.set_combat_script_path("bestiary/" + boost::to_lower_copy(Util::spaces_to_underscore(__name)) + ".lua");
							// ***********************************************************************************

							monster.move_mode = FOLLOWING;
							monster.personality = HOSTILE;
							monster.set_type(MAPOBJ_MONSTER);

							lua.push_fn_arg(boost::to_lower_copy(__name));
							monster.set_icon(lua.call_fn<double>("get_default_icon"));

							_arena->get_map()->push_obj(monster);

							// TODO: Can we simply do lua_settop(L, 0); instead
							// as suggested here: http://stackoverflow.com/questions/13404810/how-to-pop-clean-lua-call-stack-from-c
							lua_settop(global_lua_state, 0);

							return;
						}
					}
					lua_pop(global_lua_state, 1);
				}
				lua_pop(global_lua_state, 1);
			}

			lua_pop(global_lua_state, stack_change);
		}
	}
}

// The "opposite" of attack(), so to speak: lets those hostile objects attack the party if next to it.

void GameControl::get_attacked()
{
	for (auto map_obj_pair = _arena->get_map()->objs()->begin(); map_obj_pair != _arena->get_map()->objs()->end(); map_obj_pair++) {
		MapObj* map_obj = &(map_obj_pair->second);
		unsigned obj_x, obj_y;
		map_obj->get_coords(obj_x, obj_y);

		if (map_obj->personality == HOSTILE) {
			if (abs(_party->x - (int)obj_x) <= 1 && abs(_party->y - (int)obj_y) <= 1) {
				if (map_obj->get_type() == MAPOBJ_PERSON) {
					if (map_obj->get_init_script_path().length() > 0) {
						Combat combat;
						combat.create_monsters_from_init_path(map_obj->get_init_script_path());
						if (combat.initiate() == Combat_Return_Codes::VICTORY)
							get_map()->pop_obj_animate(map_obj->get_coords());
						redraw_graphics_status(true);
						return;
					}
					else
						std::cerr << "WARNING: gamecontrol.cc: I would attack, had I got an init script defined in the Lua script: '" << map_obj->id << "'.\n";
				}
				else if (map_obj->get_type() == MAPOBJ_MONSTER) {
					// We have foes from previous attack left, so do not initiate fresh combat
					if (map_obj->get_foes().size() > 0) {
						Combat combat;
						combat.set_foes(map_obj->get_foes());
						if (combat.initiate() == Combat_Return_Codes::VICTORY) {
							get_map()->pop_obj_animate(map_obj->get_coords());
							redraw_graphics_status(true);
							return;
						}
						redraw_graphics_status(true);
						map_obj->set_foes(combat.get_foes());
						return;
					}
					else if (map_obj->get_combat_script_path().length() > 0) {
						Combat combat;
						combat.create_monsters_from_combat_path(map_obj->get_combat_script_path());
						if (combat.initiate() == Combat_Return_Codes::VICTORY) {
							get_map()->pop_obj_animate(map_obj->get_coords());
							redraw_graphics_status(true);
							return;
						}
						redraw_graphics_status(true);
						map_obj->set_foes(combat.get_foes());
						return;
					}
				}
				else
					std::cerr << "WARNING: gamecontrol.cc: Unexpected case in get_attacked(): '" << map_obj->id << "'.\n";
			}
		}
	}
}

// If the user pressed (a)...
// This attack is only called executed when INDOORS, cf. first if statement!

void GameControl::keypress_attack()
{
	if (!_party->indoors()) {
		printcon("Attack - no one is around.");
		return;
	}

	printcon("Attack - in which direction?");
	std::pair<int,int> coords = select_coords();

	auto avail_objects = _arena->get_map()->objs()->equal_range(coords);

	// Strictly speaking this loop should not be necessary as we don't want monsters/ppl to walk over
	// objects, but in case we're changing our minds later, we're looking for them in a list of objects
	// rather than checking the first one only.

	if (avail_objects.first != avail_objects.second) {
		for (auto curr_obj = avail_objects.first; curr_obj != avail_objects.second; curr_obj++) {
			MapObj& the_obj = curr_obj->second;

			// Indoor monsters either have a combat_script OR a init_script attribute but *NEVER* both!
			// Init script usually means the player can keypress_talk to them...
			if (the_obj.get_type() == MAPOBJ_PERSON) {
				// We initiate fresh combat
				if (the_obj.get_init_script_path().length() > 0) {
					Combat combat;
					combat.create_monsters_from_init_path(the_obj.get_init_script_path());

					// If a town folk is attacked, the guards are alerted, and folks flee afterwards
					the_obj.move_mode = FLEE;
					make_guards(HOSTILE);

					if (combat.initiate() == Combat_Return_Codes::VICTORY) {
						get_map()->pop_obj_animate(the_obj.get_coords());
						redraw_graphics_status(true);
						return;
					}
					return;
				}
				else {
					printcon("You attack, but there is no response. Are you sure, you want to do this?");
					return;
				}
			}
			else if (the_obj.get_type() == MAPOBJ_MONSTER) {
				// We have foes from previous attack left, so do not initiate fresh combat
				if (the_obj.get_foes().size() > 0) {
					Combat combat;
					combat.set_foes(the_obj.get_foes());
					if (combat.initiate() == Combat_Return_Codes::VICTORY) {
						get_map()->pop_obj_animate(the_obj.get_coords());
						redraw_graphics_status(true);
						return;
					}
					// get_map()->get_objs(the_obj.get_coords())[0]->set_foes(combat.get_foes());
					the_obj.set_foes(combat.get_foes());
					return;
				}
				// We initiate fresh combat
				else if (the_obj.get_combat_script_path().length() > 0) {
					Combat combat;
					combat.create_monsters_from_combat_path(the_obj.get_combat_script_path());
					if (combat.initiate() == Combat_Return_Codes::VICTORY) {
						get_map()->pop_obj_animate(the_obj.get_coords());
						redraw_graphics_status(true);
						return;
					}
					// get_map()->get_objs(the_obj.get_coords())[0]->set_foes(combat.get_foes());
					the_obj.set_foes(combat.get_foes());
					return;
				}
				else {
					printcon("You attack, but there is no response. Are you sure, you want to do this?");
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

void GameControl::keypress_talk()
{
	if (!_party->indoors()) {
		printcon("Talk - sorry, no one is around");
		return;
	}

	printcon("Talk - in which direction?");
	std::pair<int,int> coords = select_coords();

	auto avail_objects = _arena->get_map()->objs()->equal_range(coords);

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
					printcon("You use your charms, but there is no response.");
					return;
				}
			}
			else if (the_obj.get_type() == MAPOBJ_ANIMAL) {
				if (the_obj.get_init_script_path().length() > 0) {
					Conversation conv(the_obj);
					conv.initiate_with_animal(get_map());
					return;
				}
				else {
					printcon("There's something soothing in talking to animals...");
					return;
				}
			}
			else if (the_obj.get_type() == MAPOBJ_MONSTER) {
				printcon("Is that a good idea?");
				return;
			}
		}
	}
	printcon("No one around to talk to");
}

std::shared_ptr<Map> GameControl::get_map()
{
	return _arena->get_map();
}

void GameControl::keypress_get_item()
{
	MapHelper map_helper(_arena->get_map().get()); // Not sure, if MapHelper should also take a shared_ptr, so that references are kept alive?!
	printcon("Get - from which direction?");
	std::pair<int, int> coords = select_coords();

	// Determine what lies at the given coordinates on the map...
	std::shared_ptr<ZtatsWinContentSelectionProvider<MapObj>> ztatswin_content_selection_provider =
			map_helper.create_ztatswin_content_selection_provider_for_coords(coords.first, coords.second, ItemPickup::RemovableOnly);

	MapObj picked_up_mapobj;

	// If there's only exactly one mapobj, pick it up.
	if (ztatswin_content_selection_provider->get_page().size() == 1) {
		picked_up_mapobj = ztatswin_content_selection_provider->get_page()[0].second;
	}
	else if (ztatswin_content_selection_provider->get_page().size() == 0) {
		printcon("Nothing to get here.");
		return;
	}
	else {
		printcon("Select item from the list");

		MiniWin& mwin = MiniWin::Instance();
		ZtatsWin& zwin = ZtatsWin::Instance();

		mwin.save_texture();
		mwin.clear();
		mwin.println(0, "Get item", CENTERALIGN);
		mwin.println(1, "(Press space to get selected item, q to exit)");

		std::vector<MapObj> selected_mapobjs = zwin.execute(ztatswin_content_selection_provider.get(), SelectionMode::SingleItem);

		if (selected_mapobjs.size() > 0) {
			picked_up_mapobj = selected_mapobjs[0];
		}
		else {
			printcon("Huh? Nothing taken.");
			return;
		}

		mwin.display_last();
	}

	// Not sure, if at this point, this check is still necessary?!
	if (!picked_up_mapobj.is_removeable()) {
		printcon("Nice try.");
		return;
	}

	// After selection of mapobj(s) was done, now add to inventory and remove from map...
	if (picked_up_mapobj.lua_name.length() > 0) {
		// Depending on the name the MapObj has, we look up in the Lua list of items, and create one accordingly for pick up.
		Item* picked_up_item = NULL;
		GameEventHandler gh;

		try {
			picked_up_item = ItemFactory::create(picked_up_mapobj.lua_name, &picked_up_mapobj);
		}
		catch (std::exception const& e) {
			std::cerr << "EXCEPTION: gamecontrol.cc: " << e.what() << "\n";
			std::cerr << "ERROR: gamecontrol.cc: Aborting get_item() due to earlier errors.\n";
			return;
		}

		// Picking up more than 1 of the same item
		if (picked_up_mapobj.how_many > 1) {
			int taking = 0;

			try {
				printcon("How many? (1-" + boost::lexical_cast<std::string>(picked_up_mapobj.how_many) + ")");
				std::string input = Console::Instance().gets();
				if (input.length() == 0) // Simply pressing return means user wants to take all available items
					taking = picked_up_mapobj.how_many;
				else
					taking = boost::lexical_cast<int>(input);
			}
			catch (boost::bad_lexical_cast const&) {
				printcon("Huh? Nothing taken.");
				return;
			}

			if (taking > 0 && taking <= picked_up_mapobj.how_many)
				printcon("Taking " + boost::lexical_cast<std::string>(taking) + " " + picked_up_item->plural_name());
			else {
				printcon("Huh? Nothing taken.");
				return;
			}

			// Let's now create the n new items to be taken individually via a factory...
			for (int i = 0; i < taking; i++) {
				try {
					_party->inventory()->add(ItemFactory::create(picked_up_mapobj.lua_name, &picked_up_mapobj));
				}
				catch (std::exception const& e) {
					std::cerr << "EXCEPTION: gamecontrol.cc: " << e.what() << "\n";
					std::cerr << "ERROR: gamecontrol.cc: Aborting adding item to inventory due to earlier errors.\n";
					continue;
				}

				// Perform action events
				for (auto action = picked_up_mapobj.actions()->begin(); action != picked_up_mapobj.actions()->end(); action++) {
					if ((*action)->name() == "ACT_ON_TAKE") {
						for (auto curr_ev = (*action)->events_begin(); curr_ev != (*action)->events_end(); curr_ev++)
							gh.handle(*curr_ev, _arena->get_map());
					}
				}
			}
			redraw_graphics_status();

			// See if some items are leftover after taking...
			if (picked_up_mapobj.how_many - taking == 0)
				_arena->get_map()->rm_obj(&picked_up_mapobj);
				//				arena->get_map()->pop_obj(coords.first, coords.second, picked_up_mapobj.lua_name);
			else {
				_arena->get_map()->rm_obj(&picked_up_mapobj);
				// ...then decrease the how_many counter...
				picked_up_mapobj.how_many -= taking;
				// ...and finally add it again with decreased how_many_counter
				_arena->get_map()->push_obj(picked_up_mapobj);
			}
		}
		// Picking up exactly 1 item
		else {
			printcon("Taking " + picked_up_item->name());
			_party->inventory()->add(picked_up_item);

			// Perform action events
			for (auto action = picked_up_mapobj.actions()->begin(); action != picked_up_mapobj.actions()->end(); action++) {
				if ((*action)->name() == "ACT_ON_TAKE") {
					for (auto curr_ev = (*action)->events_begin(); curr_ev != (*action)->events_end(); curr_ev++)
						gh.handle(*curr_ev, _arena->get_map());
				}
			}

			_arena->get_map()->rm_obj(&picked_up_mapobj);
		}
	}
	else {
		printcon("Sorry taking of this item not yet implemented (no lua_name). "
				 "Buy the author a beer or two and he might implement it for you.");
	}
}

void GameControl::keypress_look()
{
	GameEventHandler gh;
	printcon("Look - which direction?");
	std::pair<int, int> coords = select_coords();
	int icon_no = _arena->get_map()->get_tile(coords.first, coords.second);

	// Check out of map bounds
	if (icon_no >= 0) {
		std::stringstream lookstr;
		lookstr << "You see ";

		if (_arena->get_map()->is_outdoors()) {
			lookstr << OutdoorsIcons::Instance().get_props(icon_no)->get_name();
			printcon(lookstr.str());
			return;
		}

		lookstr << IndoorsIcons::Instance().get_props(icon_no)->get_name();
		printcon(lookstr.str());

		// Return range of found objects at given location
		auto found_obj = _arena->get_map()->objs()->equal_range(coords);

		if (found_obj.first != found_obj.second) {
			std::stringstream ss;
			ss << "Besides, there" << (found_obj.first->second.how_many > 1 ? " are " : " is ");

			// Now draw the objects, if there are any
			for (auto curr_obj = found_obj.first; curr_obj != found_obj.second;) {
				MapObj map_obj = curr_obj->second;

				// First, determine the displayed tile of the object, lua_name overrides the icon's name inside the xml-file
				if (map_obj.how_many > 1) {
					// There can only be multiple items if they have a lua correspondence.
					// So instead of using the icon_name, we use the map_obj lua_name to
					// temporarily create that item just in order to get its properties.
					Item* item = NULL;
					try {
						item = ItemFactory::create(map_obj.lua_name);
					}
					catch (std::exception const& e) {
						// This case here isn't actually an error: it merely means there is no Lua object for the tile looked at.
						// TODO: However, we should really have a Lua correspondence for all objects that can lie around in many...
						ss << map_obj.how_many << " " << IndoorsIcons::Instance().get_props(map_obj.get_icon())->get_name();
						if (++curr_obj != found_obj.second)
							ss << ", ";
					    continue;
					}

					ss << map_obj.how_many << " " << item->plural_name();
					if (item != NULL)
						delete item;
				}
				else {
					int obj_icon_no = map_obj.get_icon();
					std::string icon_name = IndoorsIcons::Instance().get_props(obj_icon_no)->get_name();
					// ss << (Util::vowel(icon_name[0])? "an " : "a ") << icon_name;
					ss << icon_name;
				}

				if (++curr_obj != found_obj.second)
					ss << ", ";
			}
			ss << ".";
			printcon(ss.str());
		}

		// Perform action events for objects at the given coordinates
		for (auto curr_obj = found_obj.first; curr_obj != found_obj.second; curr_obj++) {
			unsigned int tmp_x, tmp_y;
			MapObj map_obj = curr_obj->second;
			map_obj.get_coords(tmp_x, tmp_y);

			if ((int)tmp_x == coords.first && (int)tmp_y == coords.second) {
				for (auto action = map_obj.actions()->begin(); action != map_obj.actions()->end(); action++) {
					if ((*action)->name() == "ACT_ON_LOOK") {
						for (auto curr_ev = (*action)->events_begin(); curr_ev != (*action)->events_end(); curr_ev++)
							gh.handle(*curr_ev, _arena->get_map());
					}
				}
			}
		}

		// Perform look actions that are associated with the given coordinates rather than objects
		for (auto action : _arena->get_map()->get_actions(coords.first, coords.second)) {
			if (action->name() == "ACT_ON_LOOK") {
				for (auto curr_ev = action->events_begin(); curr_ev != action->events_end(); curr_ev++)
					gh.handle(*curr_ev, _arena->get_map());
			}
		}
	}
	else
		printcon("There is nothing to see");
}

/**
 *  Returns true if the current arena is an outdoors arena, false otherwise.
 */

bool GameControl::is_arena_outdoors() const
{
	return std::dynamic_pointer_cast<HexArena>(_arena) != NULL;
}

bool GameControl::walkable(int x, int y) const
{
	return check_walkable(x, y, Individual_Game_Character);
}

bool GameControl::walkable_for_party(int x, int y) const
{
	return check_walkable(x, y, Whole_Party);
}

// Internal method, that checks whether a field can be walked upon either by the party or some other, ordinary game character.
// Call from the outside instead walkable() or walkable_for_party().

bool GameControl::check_walkable(int x, int y, Walking the_walker) const
{
	// TODO: These bounds only work indoors, as outdoors we have a jump of 2 between hex.  Do we need to check the boundaries outdoors at all?
	if (!is_arena_outdoors()) {
		if (x >= (int)(_arena->get_map()->width()) || x < 0)
			return false;
		if (y >= (int)(_arena->get_map()->height()) || y < 0)
			return false;
	}

	bool icon_is_walkable = false;

	if (is_arena_outdoors())
		icon_is_walkable = OutdoorsIcons::Instance().get_props(_arena->get_map()->get_tile(x, y))->_is_walkable != PropertyStrength::None;
	else {
		int tile = _arena->get_map()->get_tile(x, y);
		icon_is_walkable = IndoorsIcons::Instance().get_props(tile)->_is_walkable != PropertyStrength::None;

		// But don't walk over monsters...
		auto found_objs = _arena->get_map()->objs()->equal_range(std::make_pair(x,y));
		if (icon_is_walkable) {
			for (auto curr_obj = found_objs.first; curr_obj != found_objs.second; curr_obj++) {
				MapObj& map_obj = curr_obj->second;
				IconProps* icon_props = IndoorsIcons::Instance().get_props(map_obj.get_icon());

				if (icon_props->_is_walkable == PropertyStrength::None)
					return false;

				if (map_obj.get_type() == MAPOBJ_MONSTER)
					return false;
				else if (map_obj.get_type() == MAPOBJ_ANIMAL)
					return false;
				else if (map_obj.get_type() == MAPOBJ_PERSON)
					return false;
			}
		}
		else if (the_walker == Whole_Party) {
			// Spells may get the party to be able to walk over water, through fire, etc.
			vector<int> additional_walkable_icons = _party->get_additional_walkable_icons();

			// If the tile in question is in the list of additionally walkable icons...
			if (std::find(additional_walkable_icons.begin(), additional_walkable_icons.end(), tile) != additional_walkable_icons.end())
				return true;

			// The opposite of the above case: if the background icon is non-walkable, but there
			// is a walkable AND ENTERABLE object, e.g., a ship in the water, then we can walk on
			// that icon.
			for (auto curr_obj = found_objs.first; curr_obj != found_objs.second; curr_obj++) {
				MapObj& map_obj = curr_obj->second;
				IconProps* icon_props = IndoorsIcons::Instance().get_props(map_obj.get_icon());
				if (icon_props->_is_walkable != PropertyStrength::None && icon_props->is_enterable())
					return true;
			}
		}
	}

	return icon_is_walkable;
}

/// Return None, if there's no force field at x,y.  Otherwise, returns strength of force/magic field.

PropertyStrength GameControl::get_forcefieldstrength(int x, int y)
{
	if (is_arena_outdoors())
		return PropertyStrength::None;

	// Check icon
	int tile = _arena->get_map()->get_tile(x, y);
	if (IndoorsIcons::Instance().get_props(tile)->_magical_force_field != PropertyStrength::None)
		return IndoorsIcons::Instance().get_props(tile)->_magical_force_field;

	// Check objects
	auto found_obj = _arena->get_map()->objs()->equal_range(std::make_pair(x,y));
	if (found_obj.first != found_obj.second) {
		for (auto curr_obj = found_obj.first; curr_obj != found_obj.second; curr_obj++) {
			MapObj& map_obj = curr_obj->second;
			IconProps* icon_props = IndoorsIcons::Instance().get_props(map_obj.get_icon());

			if (icon_props->_magical_force_field != PropertyStrength::None)
				return icon_props->_magical_force_field;
		}
	}

	return PropertyStrength::None;
}

/// Moves the party.
/// If ignore_walkable is set, then the party is moved, even through unpassable terrain.
/// @returns true, if party actually moved (instead of running into something, for example); false otherwise.

bool GameControl::move_party(LDIR dir, bool ignore_walkable)
{
	// Right now, when party has entered an object, e.g., a ship, it can't move.
	if (_party->is_entered()) {
		printcon("You do not know how to move this thing.");
		std::cout << "INFO: gamecontrol.cc: Party-coords: " << _party->x << ", " << _party->y << "\n";
		return false;
	}

	ZtatsWin& zwin = ZtatsWin::Instance();
	int x_diff = 0, y_diff = 0;

	// Determine center of arena
	int screen_center_x, screen_center_y;
	_arena->get_center_coords(screen_center_x, screen_center_y);

	switch (dir) {
	case DIR_LEFT:
		if (is_arena_outdoors()) {
			x_diff = -1;
			y_diff = _party->x % 2 == 0? 1 : -1;
		}
		else
			x_diff = -1;
		break;
	case DIR_RIGHT:
		if (is_arena_outdoors()) {
			x_diff = 1;
			y_diff = _party->x % 2 == 0? 1 : -1;
		}
		else
			x_diff = 1;
		break;
	case DIR_UP:
		if (is_arena_outdoors())
			y_diff = -2;
		else
			y_diff = -1;
		break;
	case DIR_DOWN:
		if (is_arena_outdoors())
			y_diff = 2;
		else
			y_diff = 1;
		break;
	case DIR_RDOWN:
		if (is_arena_outdoors()) {
			y_diff = 1;
			x_diff = 1;
			break;
		}
	case DIR_RUP:
		if (is_arena_outdoors()) {
			y_diff = -1;
			x_diff = 1;
			break;
		}
	case DIR_LUP:
		if (is_arena_outdoors()) {
			y_diff = -1;
			x_diff = -1;
			break;
		}
	case DIR_LDOWN:
		if (is_arena_outdoors()) {
			y_diff = 1;
			x_diff = -1;
			break;
		}
	default:
		std::cout << "INFO: gamecontrol.cc: Party-coords: " << _party->x << ", " << _party->y << "\n";
		return false;
	}

	int tile = _arena->get_map()->get_tile(_party->x + x_diff, _party->y + y_diff);

	// Indoors
	if (!is_arena_outdoors()) {
		// Check if exiting map!
		if (!_arena->get_map()->is_within_visible_bounds(_party->x + x_diff, _party->y + y_diff)) {
			if (leave_map())
				redraw_graphics_status(true);
			return false;
		}

		if (!ignore_walkable && !walkable_for_party(_party->x + x_diff, _party->y + y_diff))
			return false;

		// If we're ignoring walkability, or we don't and the map is walkable, do it!
		if (ignore_walkable || walkable_for_party(_party->x + x_diff, _party->y + y_diff)) {
			IconProps* tile_props = IndoorsIcons::Instance().get_props(tile);

			// Slow walking means to drop every third step.
			if (!ignore_walkable && tile_props->_is_walkable == PropertyStrength::Some) {
				static int every_third_step = 0;
				printcon("Slow.");
				if (every_third_step == 0) {
					// std::cout << "INFO: gamecontrol.cc: Party-coords: " << party->x << ", " << party->y << "\n";
					every_third_step++;
					return false;
				}
				else if (every_third_step == 1)
					every_third_step++;
				else // if (every_third_step == 2)
					every_third_step = 0;
			}

			_arena->moving(true);

			if (screen_pos_party.first  + x_diff < screen_center_x)
				_arena->move(DIR_LEFT);
			if (screen_pos_party.first  + x_diff > screen_center_x)
				_arena->move(DIR_RIGHT);
			if (screen_pos_party.second + y_diff > screen_center_y)
				_arena->move(DIR_DOWN);
			if (screen_pos_party.second + y_diff < screen_center_y)
				_arena->move(DIR_UP);

			set_party(_party->x + x_diff, _party->y + y_diff);
			_arena->map_to_screen(_party->x, _party->y, screen_pos_party.first, screen_pos_party.second);

			_arena->moving(false);

			// Check if poison, magic field, fire, etc. was entered and act accordingly.
			bool somebody_hurt = _party->walk_through_magic_field(get_forcefieldstrength(_party->x, _party->y));
			somebody_hurt = somebody_hurt || _party->walk_through_poison_field(tile_props->_poisonous);
			if (somebody_hurt) {
				_sample.play_predef(HIT);
				zwin.update_player_list();
			}

			std::cout << "INFO: gamecontrol.cc: Party-coords: " << _party->x << ", " << _party->y << "\n";
			return true;
		}
	}
	// Outdoors
	else {
		// Note to self:
		// In theory, what's here could move into the above switch stmt., but if we ever want a more generic
		// handling of outdoor movements, then it would be nice to load it off right here, instead of blowing
		// up that switch statement, which handles both indoors and outdoors coordinate changes.
		if (ignore_walkable || walkable_for_party(_party->x + x_diff, _party->y + y_diff)) {
			IconProps* tile_props = OutdoorsIcons::Instance().get_props(tile);

			// Slow walking means to drop every third step.
			if (!ignore_walkable && tile_props->_is_walkable == PropertyStrength::Some) {
				static int every_third_step = 0;
				printcon("Slow.");
				if (every_third_step == 0) {
					// std::cout << "INFO: gamecontrol.cc: Party-coords: " << party->x << ", " << party->y << "\n";
					every_third_step++;
					return false;
				}
				else if (every_third_step == 1)
					every_third_step++;
				else // if (every_third_step == 2)
					every_third_step = 0;
			}

			switch (dir) {
			case DIR_LEFT:
				if (screen_pos_party.first < screen_center_x) {
					_arena->move(DIR_LEFT);
					_arena->move(DIR_LEFT);
				}
				break;
			case DIR_RIGHT:
				if (screen_pos_party.first > screen_center_x) {
					_arena->move(DIR_RIGHT);
					_arena->move(DIR_RIGHT);
				}
				break;
			case DIR_DOWN:
				if (screen_pos_party.second > screen_center_y)
					_arena->move(DIR_DOWN);
				break;
			case DIR_UP:
				if (screen_pos_party.second < screen_center_y)
					_arena->move(DIR_UP);
				break;
			case DIR_RDOWN:
				if (screen_pos_party.first > screen_center_x) {
					_arena->move(DIR_RIGHT);
					_arena->move(DIR_RIGHT);
				}
				if (screen_pos_party.second > screen_center_y)
					_arena->move(DIR_DOWN);
				break;
			case DIR_RUP:
				if (screen_pos_party.first > screen_center_x) {
					_arena->move(DIR_RIGHT);
					_arena->move(DIR_RIGHT);
				}
				if (screen_pos_party.second < screen_center_y)
					_arena->move(DIR_UP);
				break;
			case DIR_LUP:
				if (screen_pos_party.first < screen_center_x) {
					_arena->move(DIR_LEFT);
					_arena->move(DIR_LEFT);
				}
				if (screen_pos_party.second < screen_center_y)
					_arena->move(DIR_UP);
				break;
			default: // case DIR_LDOWN:
				if (screen_pos_party.first < screen_center_x) {
					_arena->move(DIR_LEFT);
					_arena->move(DIR_LEFT);
				}
				if (screen_pos_party.second > screen_center_y)
					_arena->move(DIR_DOWN);
				break;
			}

			_party->set_coords(_party->x + x_diff, _party->y + y_diff);
			_arena->map_to_screen(_party->x, _party->y, screen_pos_party.first, screen_pos_party.second);
			// std::cout << "INFO: gamecontrol.cc: Party-coords: " << party->x << ", " << party->y << "\n";
			return true;
		}
	}

	// std::cout << "INFO: gamecontrol.cc: Party-coords: " << party->x << ", " << party->y << "\n";
	std::cerr << "WARNING: gamecontrol.cc: move_party() failed.\n";

	_sample.play_predef(HIT);
	printcon("Blocked.");

	return false;
}

void GameControl::keypress_move_party(LDIR dir)
{
	if (_party->rounds_intoxicated > 0) {
		bool move_random = random(0,10) >= 7;

		if (move_random) {
			LDIR newDir = LDIR(rand() % 3);
			if (newDir != dir)
				printcon("Ooer...");
			dir = newDir;
		}
	}

	if (move_party(dir)) {
		_sample.play_predef(WALK);
		printcon(ldirToString.at(dir) + ".");
	}
	// else party did not move.  Handled inside move_party().

	do_turn();
}

/**
 * Return viewport size for night, torches, etc.
 */

std::pair<int, int> GameControl::get_viewport()
{
	int x = 0; // Default: maximum viewport, bright as day!

	if (_arena->get_map()->is_dungeon) {
		int rad = max(2, _party->light_radius());
		return std::make_pair(rad,rad);
	}

	switch (_clock.tod()) {
	case EARLY_MORNING:
		x = 8 + (_clock.time().first%2 == 0? _clock.time().first : _clock.time().first + 1);
		break;
	case MORNING:
	case NOON:
	case AFTERNOON:
		x = 16 + (_clock.time().first%2 == 0? _clock.time().first : _clock.time().first + 1);
		break;
	case EVENING:
	case NIGHT:
		x = max(6, 24 - (_clock.time().first%2 == 0? _clock.time().first : _clock.time().first + 1) + 10);
		break;
	case MIDNIGHT:
		x = 4;
		break;
	}

	x = max(x, _party->light_radius() + 2);

	// It's a square view!
	return std::make_pair(x,x);
}

void GameControl::keypress_pull_push()
{
	printcon("Pull/push - which direction?");
	std::pair<int, int> coords = select_coords();

	// First go through objects which may have a pull/push action
	bool has_paction = false;
	for (auto obj: _arena->get_map()->get_objs(coords.first, coords.second)) {
		for (auto act: *(obj->actions())) {
			if (std::dynamic_pointer_cast<ActionPullPush>(act)) {
				GameEventHandler gh;
				has_paction = true;
				for (auto curr_ev = act->events_begin(); curr_ev != act->events_end(); curr_ev++)
					gh.handle(*curr_ev, _arena->get_map());
			}
		}
	}

	// Now go through map actions which are not associated with an object
	std::vector<std::shared_ptr<Action>> acts = _arena->get_map()->get_actions(coords.first, coords.second);
	if (acts.size() == 0 && !has_paction) {
		printcon("Nothing to pull or push here");
		return;
	}

	has_paction = false;
	for (auto action: acts) {
		if (std::dynamic_pointer_cast<ActionPullPush>(action)) {
			GameEventHandler gh;
			has_paction = true;
			for (auto curr_ev = action->events_begin(); curr_ev != action->events_end(); curr_ev++)
				gh.handle(*curr_ev, _arena->get_map());
		}
	}

	if (!has_paction)
		printcon("Nothing to pull or push here");
}

void GameControl::action_on_enter(std::shared_ptr<ActionOnEnter> action)
{
	GameEventHandler gh;

	for (auto curr_ev = action->events_begin(); curr_ev != action->events_end(); curr_ev++)
		gh.handle(*curr_ev, _arena->get_map());
}

// TODO: THIS CAN ONLY EVER BE CALLED FROM LEVEL-0 (I.E. GROUND FLOOR) INDOORS MAPS!
//
// Returns true if the player entered "Y" to the question of whether she would like to leave a map.  Otherwise false is returned.

bool GameControl::leave_map()
{
	printcon("Do you wish to leave? (y/n)");

	switch (_em->get_key("yn")) {
	case 'y': {
		GameEventHandler gh;
		std::shared_ptr<EventLeaveMap> leave_event(new EventLeaveMap());

		leave_event->set_map_name(_arena->get_map()->get_name().c_str());
		leave_event->set_old_map_name(_party->map_name().c_str());
		gh.handle_event_leave_map(leave_event, _arena->get_map());

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
	_party->set_indoors(!mode);
}

void GameControl::set_map_name(const char* new_name)
{
	_party->set_map_name(new_name);
}

int GameControl::random(int min, int max)
{
	NumberDistribution distribution(min, max);
	Generator numberGenerator(_generator, distribution);
	return numberGenerator();
}

void GameControl::printcon(const std::string s, bool wait)
{
	Console::Instance().print(_normal_font, s, wait);
}

Clock* GameControl::get_clock()
{
	return &_clock;
}
