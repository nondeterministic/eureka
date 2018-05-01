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
	blit();
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
			scroll(font->char_height() + offset);

			// Drop initial white spaces on the newline.
			while (s[i] == ' ' && i < s.length() - 1)
				i++;
		}
		font->printch(get_texture(), s[i], x, y);
		blit();

		if (wait) {
			pause();
			blit();
		}
	}

	scroll(font->char_height() + offset);
	blit();
}

/**
 * Does what it says it does, one pixel at a time, amount times.
 * Returns -1 in case of error, 0 in case of coolness.
 */

int Console::scroll(int amount, int delay)
{
	int console_width, console_height;
	SDL_QueryTexture(get_texture(), NULL, NULL, &console_width, &console_height);

	int scroll_pixels = 1;

	if (amount > 0) {
		SDL_Rect srcRect;
		srcRect.x = 0;
		srcRect.y = scroll_pixels;
		srcRect.w = console_width;
		srcRect.h = console_height - scroll_pixels;

		SDL_Rect fillRect;
		fillRect.x = 0;
		fillRect.y = console_height - scroll_pixels;
		fillRect.w = console_width;
		fillRect.h = scroll_pixels;

		SDL_Rect dstRect;
		dstRect.x = 0;
		dstRect.y = 0;
		dstRect.w = console_width;
		dstRect.h = console_height - scroll_pixels;

		if (SDL_SetRenderTarget(get_renderer(), get_texture()) < 0) {
			std::cerr << "ERROR: sdlwindow.cc: Setting RenderTarget failed: " << IMG_GetError() << std::endl;
			return -1;
		}

		if (SDL_RenderCopy(get_renderer(), get_texture(), &srcRect, &dstRect) == 0) {
			SDL_SetRenderDrawColor(get_renderer(), 0, 0, 0, 0);
			SDL_RenderFillRect(get_renderer(), &fillRect);
			return scroll(amount - 1, delay);
		}
		else {
			std::cerr << "ERROR: sdlwindow.cc: scroll_console() failed: " << IMG_GetError() << std::endl;
			return -1;
		}
	}

	return 0;
}

// Read a string from the console, i.e., get user input.

std::string Console::gets()
{
	Charset& normal_font = Charset::Instance();
	EventManager* em = &(EventManager::Instance());
	std::string input = "";

	while (1) {
		char key = em->get_key();
		switch (tolower(key)) {
		case SDLK_RETURN:
			cursor_x = 0;
			cursor_y = 0;
			print_line(&normal_font, input + " ");
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

// Copied this basically from animate_cursor(). Seems, we only care for offset.

void Console::remove_cursor(Type* font, __attribute__((unused)) int x, __attribute__((unused)) int y, int offset)
{
	font->printch(get_texture(),
				  _BLANK_ICON,
				  (pre_cursor_x * font->char_width()) + offset,
				  (pre_cursor_y * (font->char_height() + offset)) + (get_size().second - font->char_height() - offset));
	blit();
}

void Console::animate_cursor(Type* font, int x, int y, int offset)
{
	// Only change cursor position, if one was explicitly set; -1 is default value.
	if (x != -1) cursor_x = x;
	if (y != -1) cursor_y = y;

	// If cursor is in new position, black out the old one
	if (pre_cursor_x != cursor_x || pre_cursor_y != cursor_y) {
		font->printch(get_texture(),
					  _BLANK_ICON,
					  (pre_cursor_x * font->char_width()) + offset,
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

	blit();
}

void Console::pause(int delay)
{
	SDL_Delay(delay);
}

SDL_Renderer* Console::get_renderer()
{
	return SDLWindow::Instance().get_renderer();
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

void Console::clear()
{
	SDL_Texture*   t = get_texture();
	SDL_Renderer*  r = get_renderer();

	SDL_SetRenderTarget(r, t);
	SDL_SetRenderDrawColor(r, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(r);
	blit();
}

// Briefly highlights the window in white as a kind of alarm.
// Used for noting the fact that the player has been hit.

void Console::alarm() 
{
	const std::pair<int,int> size = get_size();
	SDL_Renderer*  r = get_renderer();
	SDL_Texture*   t = get_texture();
	SDL_Texture* tmp = SDL_CreateTexture(r, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, size.first, size.second);

	// Back texture up.
	SDL_SetRenderTarget(r, tmp);
	SDL_RenderCopy(r, t, NULL, NULL);

	// Whiten texture.
	SDL_SetRenderTarget(r, t);
	SDL_SetRenderDrawColor(r, 255, 255, 255, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(r);

	// Show for some time.
	SDLWindow::Instance().blit_entire_window_texture();
	blit();
	SDL_Delay(100);

	// Show old texture again.
	SDL_SetRenderTarget(r, t);
	SDL_RenderCopy(r, tmp, NULL, NULL);
	blit();
	SDLWindow::Instance().blit_entire_window_texture();
	SDL_DestroyTexture(tmp);
}

void Console::blit()
{
	SDLWindow::Instance().blit_console();
}
