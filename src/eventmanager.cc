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
#include <algorithm>

#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>

#include <boost/signals.hpp>
#include <boost/bind.hpp>

#include "eventmanager.hh"
#include "charset.hh"
#include "console.hh"
#include "gamecontrol.hh"

EventManager& EventManager::Instance()
{
	static EventManager ev;
	return ev;
}

EventManager::EventManager() { }

SDL_TimerID EventManager::add_event(Uint32 interval, SDL_TimerCallback callback_fn, void* callback_param)
{
	SDL_TimerID ti = SDL_AddTimer(interval, callback_fn, callback_param);
	return ti;
}

/**
 * Gets a single key input from the user.  If valid_inputs is != NULL,
 * then the function only returns the key if the corresponding
 * character is in valid_inputs.  E.g., if valid_inputs = "ad", then
 * all key strokes except a and d are ignored and not returned.  So
 * the function effectively waits for the user to press one of the
 * "right" keys.  If valid_inputs is NULL, any key press is returned
 * as long as it was an alphanumeric character.
 */

char EventManager::get_key(const char* valid_inputs)
{
	Charset normal_font;
	SDL_Event ev;
	char input = '_';  // To shut up GCC: any default char will do here.
	std::string valid_inputs_str = "";

	if (valid_inputs)
		valid_inputs_str = valid_inputs;

	while (1) {
		while (SDL_WaitEvent(&ev)) {
			if (ev.type == SDL_USEREVENT) {
				if (ev.user.code == TICK) {
					Console::Instance().animate_cursor(&normal_font); // This animates the cursor during keyboard input
					GameControl::Instance().redraw_graphics_arena(); // TODO: This animates the map, but make sure that a map is visible first!!
				}
			}
			else if (ev.type == SDL_KEYDOWN) {
				switch(ev.key.keysym.sym) {
				case SDLK_a: input = 'a'; break;
				case SDLK_b: input = 'b'; break;
				case SDLK_c: input = 'c'; break;
				case SDLK_d: input = 'd'; break;
				case SDLK_e: input = 'e'; break;
				case SDLK_f: input = 'f'; break;
				case SDLK_g: input = 'g'; break;
				case SDLK_h: input = 'h'; break;
				case SDLK_i: input = 'i'; break;
				case SDLK_j: input = 'j'; break;
				case SDLK_k: input = 'k'; break;
				case SDLK_l: input = 'l'; break;
				case SDLK_m: input = 'm'; break;
				case SDLK_n: input = 'n'; break;
				case SDLK_o: input = 'o'; break;
				case SDLK_p: input = 'p'; break;
				case SDLK_q: input = 'q'; break;
				case SDLK_r: input = 'r'; break;
				case SDLK_s: input = 's'; break;
				case SDLK_t: input = 't'; break;
				case SDLK_u: input = 'u'; break;
				case SDLK_v: input = 'v'; break;
				case SDLK_w: input = 'w'; break;
				case SDLK_x: input = 'x'; break;
				case SDLK_y: input = 'y'; break;
				case SDLK_z: input = 'z'; break;
				case SDLK_0: input = '0'; break;
				case SDLK_1: input = '1'; break;
				case SDLK_2: input = '2'; break;
				case SDLK_3: input = '3'; break;
				case SDLK_4: input = '4'; break;
				case SDLK_5: input = '5'; break;
				case SDLK_6: input = '6'; break;
				case SDLK_7: input = '7'; break;
				case SDLK_8: input = '8'; break;
				case SDLK_9: input = '9'; break;
				case SDLK_RETURN: input = SDLK_RETURN; break;
				case SDLK_BACKSPACE: input = SDLK_BACKSPACE; break;
				case SDLK_SPACE: input = SDLK_SPACE; break;
				default: input = -1; break;
				}
				if (input == -1)
					/* Do nothing. */ ;
				else if ((!valid_inputs && isalnum(input)) ||
						(!valid_inputs && (input == SDLK_RETURN || SDLK_BACKSPACE || SDLK_SPACE)) ||
						(valid_inputs && valid_inputs_str.find(input) != std::string::npos))
					return input;
			}
		}
	}
}

/// A more generic version of the above.  Handy for checking cursor keys, etc.
/// If keys is empty, any pressed key is returned!

SDL_Keycode EventManager::get_generic_key(std::list<SDL_Keycode>& keys)
{
	SDL_Event ev;
	Charset normal_font;

	while (SDL_WaitEvent(&ev)) {
		if (ev.type == SDL_USEREVENT) {
			if (ev.user.code == TICK) {
				Console::Instance().animate_cursor(&normal_font); // This animates the cursor during keyboard input
				GameControl::Instance().redraw_graphics_arena();               // This animates the map, but make sure that a map is visible first!!
			}
		}
		else if (ev.type == SDL_KEYDOWN) {
			if (std::find(keys.begin(), keys.end(), ev.key.keysym.sym) != keys.end() || keys.size() == 0)
				return ev.key.keysym.sym;
		}
	}

	throw "EXCEPTION: eventmanager.cc: get_generic_key did not return any key!\n";
}
