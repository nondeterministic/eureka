// This source file is part of eureka
//
// Copyright (c) 2007-2018 Andreas Bauer <baueran@gmail.com>
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

#ifndef GAMECONTROL_HH
#define GAMECONTROL_HH

#include <SDL.h>

#include <string>
#include <utility>
#include <memory>

#include <boost/signal.hpp>
#include <boost/random.hpp>

#include "sdlwindow.hh"
#include "arena.hh"
#include "charset.hh"
#include "actiononenter.hh"
#include "gamecharacter.hh"
#include "eventmanager.hh"
#include "party.hh"
#include "clock.hh"
#include "pathfinding.hh"
#include "mapobj.hh"
#include "spell.hh"
#include "soundsample.hh"
#include "iconprops.hh"

typedef boost::uniform_int<> NumberDistribution; 
typedef boost::mt19937       RandomNumberGenerator; 
typedef boost::variate_generator<RandomNumberGenerator&, NumberDistribution> Generator;

class GameControl
{
public:
	enum Walking { Whole_Party, Individual_Game_Character };

protected:
	RandomNumberGenerator _generator;
	EventManager* _em;
	Party* _party;
	int _turn_passed;
	std::pair<int, int> screen_pos_party;
	std::shared_ptr<Arena> _arena;
	Charset* _normal_font;
	unsigned _turns;
	Clock _clock;
	SoundSample* _game_music;
	SoundSample _sample;
	bool _game_is_started;
	bool check_walkable(int, int, Walking);
	GameControl();

public:
	static GameControl& Instance();
	bool is_arena_outdoors();
	bool move_party(LDIR, bool = false);
	void move_objects();
	void redraw_graphics_status(bool = true);
	int tick_event_handler();
	int tick_event_turn_handler();
	int key_event_handler(SDL_Event* = NULL);
	int set_arena(std::shared_ptr<Arena>);
	std::shared_ptr<Arena> get_arena();
	int redraw_graphics_arena();
	int close_win();
	int set_party(int, int);
	void set_outdoors(bool);
	void action_on_enter(std::shared_ptr<ActionOnEnter>);
	bool leave_map();
	bool unlock_item();
	void game_over();
	PropertyStrength get_forcefieldstrength(int, int);

	void keypress_cast();
	void keypress_talk();
	void keypress_move_party(LDIR);
	void keypress_use();
	void keypress_hole_up();
	void keypress_open_act();
	void keypress_attack();
	void keypress_quit();
	void keypress_ztats();
	void keypress_drop_items();
	void keypress_inventory();
	void keypress_pull_push();
	void keypress_look();
	void keypress_get_item();
	void keypress_mix_reagents();

	std::shared_ptr<ZtatsWinContentProvider> create_party_content_provider();
	/// Makes all guards of a town turn hostile (e.g., after committing a crime), or neutral, etc.
	void make_guards(PERSONALITY);
	void get_attacked();
	void set_game_music(SoundSample*);
	std::string keypress_ready_item(int);
	void keypress_yield_item(int);
	std::pair<int, int> select_coords();
	void set_map_name(const char*);
	void start_turns();
	void stop_turns();
	void do_turn(bool = false);
	bool game_won();
	int random(int, int);
	void printcon(const std::string, bool = false);
	bool walkable_for_party(int, int);
	bool walkable(int, int);
	std::shared_ptr<Map> get_map();
	Clock* get_clock();
	std::pair<int,int> get_viewport();
	std::string select_spell(unsigned);
	void cast_spell(int, Spell);
	void create_random_monsters_in_dungeon();
	void set_game_started(bool);
	bool get_game_started();
};

#endif
