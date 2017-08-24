//
//
// Copyright (c) 2010  Andreas Bauer <baueran@gmail.com>
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
#include <sstream>
#include <ctype.h>
#include "console.hh"
#include "charset.hh"
#include "type.hh"
#include "eventmanager.hh"
#include "sdlwindow.hh"
#include "sdltricks.hh"

using namespace std;

Console::Console()
{
	cursor = 433;
	cursor_x = 0;
	cursor_y = 0;
	pre_cursor_x = 0;
	pre_cursor_y = 0;
}

Console::~Console()
{
}

Console& Console::Instance()
{
	static Console inst;
	return inst;
}

// Returns the (rest of a) word inside string s at position pos.

std::string Console::get_word(const string& s, int pos)
{
	std::stringstream ss;

	for (unsigned i = pos; i < s.length(); i++) {
		if (s[i] == ' ' || s[i] == '\n')
			break;
		ss << s[i];
	}

	return ss.str();
}

// Like print, but doesn't scroll at end of line; simply cuts off at right end of console.

void Console::print_line(Type* font, std::string s)
{
	int offset = 2;
	int j = 0;
	for (unsigned i = 0; i < s.length(); i++, j++) {
		int x = offset + j * font->char_width();
		int y = get_size().second - font->char_height() - offset;
		font->printch(get_texture(), s[i], x, y);
	}
	SDLWindow::Instance().blit_console();
}

void Console::print(Type* font, const std::string s, bool wait)
{
	int offset = 2;
	int j = 0;
	for (unsigned i = 0; i < s.length(); i++, j++) {
		int x = offset + j * font->char_width();
		int y = get_size().second - font->char_height() - offset;

		// Check if we have to blit and write into the next line
		if ( s[i] == '\n' ||
				// Automatic line break at long words:
				x + (get_word(s, i).length() * font->char_width()) >= (get_size().first - 1 - font->char_width()) )
		{
			j = 0;
			x = offset;

			// Chomp newline if there was one
			if (s[i] == '\n' && i < s.length() - 1)
				i++;

			// Tell SDLWindow to blit console upwards
			SDLWindow::Instance().scroll_console(font->char_height() + offset);

			// Drop initial white spaces on the newline.
			while (s[i] == ' ' && i < s.length() - 1)
				i++;
		}
		font->printch(get_texture(), s[i], x, y);
		SDLWindow::Instance().blit_console();

		if (wait) {
			pause();
			SDLWindow::Instance().blit_console();
		}
	}

	SDLWindow::Instance().scroll_console(font->char_height() + offset);
	SDLWindow::Instance().blit_console();
}

// Read a string from the console, i.e., get user input.

std::string Console::gets()
{
	Charset normal_font;
	EventManager* em = &(EventManager::Instance());
	std::string input = "";

	// print(&normal_font, input, false);

	while (1) {
		char key = em->get_key();
		switch (tolower(key)) {
		case SDLK_RETURN:
			cursor_x = 0;
			cursor_y = 0;
			return input;
		case SDLK_BACKSPACE:
			if (input.length() >= 1)
				input = input.substr(0, input.length() - 1);
			print_line(&normal_font, input);
			animate_cursor(&normal_font, input.length());
			break;
		default:
			input = input + key;
			animate_cursor(&normal_font, input.length());
			print_line(&normal_font, input);
			break;
		}
	}
}

void Console::animate_cursor(Type* font, int x, int y, int offset)
{
	// Only change cursor position, if one was explicitly set; -1 is default value.
	if (x != -1) cursor_x = x;
	if (y != -1) cursor_y = y;

	// If cursor is in new position, black out the old one
	int blank_icon = 423; // see charset.cc
	if (pre_cursor_x != cursor_x || pre_cursor_y != cursor_y) {
		font->printch(get_texture(), blank_icon,
					  (pre_cursor_x * font->char_width()) + (offset),
					  (pre_cursor_y * (font->char_height() + offset)) + (get_size().second - font->char_height() - offset));
	}

	pre_cursor_x = cursor_x;
	pre_cursor_y = cursor_y;

	// Set animation
	if (cursor >= 436)
		cursor = 433;
	else
		cursor++;

	font->printch(get_texture(), cursor,
				  (cursor_x * font->char_width()) + (offset),
				  (cursor_y * (font->char_height() + offset)) + (get_size().second - font->char_height() - offset));

	SDLWindow::Instance().blit_console();
}

void Console::pause(int delay)
{
	SDL_Delay(delay);
}

SDL_Texture* Console::get_texture()
{
	return SDLWindow::Instance().get_texture_console();
}

std::pair<int,int> Console::get_size()
{
	int w = -1, h = -1;
	if (SDL_QueryTexture(get_texture(), NULL, NULL, &w, &h) < 0)
		std::cerr << "WARNING: console.cc: cannot determine size.\n";
	return std::make_pair(w,h);
}

// Briefly highlights the window in white as a kind of alarm.
// Used for noting the fact that the player has been hit.

void Console::alarm() 
{
// TODO SDL

//  SDL_Surface *s = get_surface();
//  SDL_Surface *copy = SDL_ConvertSurface(s, s->format, s->flags);
//
//  // cf. http://cboard.cprogramming.com/game-programming/99430-how-make-grayscale-filter.html
//  SDL_LockSurface(s);
//  for ( int x = 0; x != s->w; ++x )
//  {
//    for ( int y = 0; y != s->h; ++y )
//    {
//      Uint32 pixel = SDLTricks::Instance().getpixel(s,x,y);
//      Uint8 r = 0;
//      Uint8 g = 0;
//      Uint8 b = 0;
//      SDL_GetRGB(pixel, s->format, &r, &g, &b);
//      r = g = b = (( r+g+b ) / 9);
//      SDLTricks::Instance().putpixel(s, x, y, SDL_MapRGB(s->format, r,g,b));
//    }
//  }
//  SDL_UnlockSurface(s);
//
//  SDLWindow::Instance().blit_console();
//  SDL_Delay(100);
//  SDL_BlitSurface(copy, NULL, s, NULL);
//  SDL_FreeSurface(copy);
}
