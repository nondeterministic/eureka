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
#include "eventmanager.hh"
#include "party.hh"
#include "clock.hh"
#include "pathfinding.hh"
#include "mapobj.hh"
#include "spell.hh"
#include "soundsample.hh"

typedef boost::uniform_int<> NumberDistribution; 
typedef boost::mt19937       RandomNumberGenerator; 
typedef boost::variate_generator<RandomNumberGenerator&, NumberDistribution> Generator;

class GameControl
{
protected:
	RandomNumberGenerator generator;
	EventManager* em;
	Party* party;
	int _turn_passed;
	std::pair<int, int> screen_pos_party;
	std::shared_ptr<Arena> arena;
	Charset normal_font;
	std::string input;
	unsigned _turns;
	Clock _clock;
	SoundSample* _game_music;
	GameControl();

public:
	static GameControl& Instance();
	bool is_arena_outdoors();
	bool move_party_quietly(LDIR, bool = false);
	void move_party(LDIR);
	void move_objects();
	void draw_status(bool = true);
	int tick_event_handler();
	int tick_event_turn_handler();
	int key_event_handler(SDL_Event* = NULL);
	int set_arena(std::shared_ptr<Arena>);
	std::shared_ptr<Arena> get_arena();
	int show_win();
	int close_win();
	int set_party(int, int);
	void set_outdoors(bool);
	void action_on_enter(std::shared_ptr<ActionOnEnter>);
	bool leave_map();
	void unlock_item();
	void talk();
	void game_over();
	void use();
	void hole_up();
	void open_act();
	void make_guards(PERSONALITY);
	void attack();
	void get_attacked();
	void quit();
	void ztats();
	void drop_items();
	void inventory();
	void pull_push();
	void set_game_music(SoundSample*);
	std::string ready_item(int);
	std::string yield_item(int);
	void look();
	void get_item();
	std::pair<int, int> select_coords();
	void set_map_name(const char*);
	void start_turns();
	void stop_turns();
	void do_turn(bool = false);
	int random(int, int);
	void printcon(const std::string, bool = false);
	bool walk_fullspeed(int, int);
	bool walkable(int, int);
	std::shared_ptr<Map> get_map();
	Clock* get_clock();
	std::pair<int,int> get_viewport();
	std::string select_spell(int);
	void cast_spell(int, Spell);
	void create_random_monsters_in_dungeon();
};

#endif
