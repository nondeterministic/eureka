// This source file is part of eureka
//
// Copyright (c) 2007-2017  Andreas Bauer <baueran@gmail.com>
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

#ifndef SDLWINDOW_HH
#define SDLWINDOW_HH

#include <string>
#include <vector>
#include <utility>

#include <SDL2/SDL.h>

class SDLWindow
{
protected:
	Uint32 rmask, gmask, bmask, amask;
	SDL_Window* _win;
	SDL_Renderer* _renderer;
	SDL_Texture* _texture_entire_window;
	SDL_Texture* _texture_arena;
	SDL_Texture* _texture_console;
	SDL_Texture* _texture_ztats;
	SDL_Texture* _texture_mini_win;
	SDL_Texture* _texture_tiny_win;
	int _x, _y, _w, _h;
	int _arena_w, _arena_h;
	std::vector<SDL_Texture*> _vec_frameicons;
	static const int _frame_icon_size = 16;

	SDLWindow();
	~SDLWindow();

public:
	static SDLWindow& Instance (void);

	int init(int = 0, int = 0, int = 0, Uint32 = 0);
	void close();
	SDL_Texture* get_texture_arena();
	SDL_Texture* get_texture_console();
	SDL_Texture* get_texture_ztats();
	SDL_Texture* get_texture_mini_win();
	SDL_Texture* get_texture_tiny_win();
	SDL_Texture* get_texture_entire_win();
	SDL_Window* get_native_sdl_window();
	std::pair<int,int> get_size();
	int draw_frame(int, int);
	int blit_entire_window_texture();
	int blit_all();
	int blit_arena();
	int blit_console();
	int blit_ztats();
	int blit_mini_win();
	int blit_tiny_win();
	int scroll_console(int, int = 5);
	int create_texture_console();
	int create_texture_ztats();
	int create_texture_mini_win();
	int create_texture_tiny_win();
	int create_texture_entire_window();
	int frame_icon_size();
	void clear_texture_arena();

	SDL_Renderer* getRenderer() const;
	bool resetRenderer();
};

class SurfaceNULL
{
protected: 
	std::string err;
public: 
	SurfaceNULL(std::string s) { err = s; }
	std::string print() const { return err; }
};

#endif
