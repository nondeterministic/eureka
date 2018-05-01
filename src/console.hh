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

#ifndef CONSOLE_HH
#define CONSOLE_HH

#include <string>
#include <utility>

#include <SDL2/SDL.h>

#include "type.hh"

class Console
{
public:
  static Console& Instance();
  void print(Type*, const std::string, bool wait = false);
  void print_line(Type*, const std::string);
  std::string gets();

  SDL_Texture* get_texture();
  std::pair<int,int> get_size();
  void remove_cursor(Type*, int x = -1, int y = -1, int offset = 2);
  void animate_cursor(Type*, int x = -1, int y = -1, int offset = 2);
  void pause(int = 30);
  void alarm();
  void clear();
  
protected:
  Console();
  ~Console();
  int cursor;
  int cursor_x, cursor_y;
  int pre_cursor_x, pre_cursor_y;
  std::string get_word(const std::string& s, int = 0);
  int scroll(int, int = 10);
  void blit();
  SDL_Renderer* get_renderer();

  // See charset.cc
  const int _BLANK_ICON = 423;
};

#endif
