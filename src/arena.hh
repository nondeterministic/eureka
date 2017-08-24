// This source file is part of eureka
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

#ifndef ARENA_HH
#define ARENA_HH

#include <SDL2/SDL.h>

#include <vector>
#include <memory>
#include <utility>

#include <boost/assign/list_of.hpp>
#include <boost/unordered_map.hpp>

#include "map.hh"
#include "sdlwindow.hh"

enum LDIR {
  DIR_UP,
  DIR_DOWN,
  DIR_RIGHT,
  DIR_LEFT,
  DIR_LUP,
  DIR_LDOWN,
  DIR_RUP,
  DIR_RDOWN,
  DIR_NONE
};

const boost::unordered_map<LDIR, std::string> ldirToString =
	boost::assign::map_list_of
	(DIR_UP, "North")
	(DIR_DOWN, "South")
	(DIR_RIGHT, "East")
	(DIR_LEFT, "West")
	(DIR_LUP, "West")
	(DIR_LDOWN, "West")
	(DIR_RUP, "East")
	(DIR_RDOWN, "East")
	(DIR_NONE, "Nowhere");

class Offsets
{
public:
  int top, bot, left, right;
  bool operator== (Offsets);
  bool operator!= (Offsets);
};

class Arena
{
public:
  Arena();
  virtual ~Arena();

  // Does the return type need to be static to ensure only one object
  // is created?
  static std::shared_ptr<Arena> create(std::string, std::string);

  void adjust_offsets(int = 0, int = 0, int = 0, int = 0);
  void set_offsets(unsigned = 0, unsigned = 0, unsigned = 0, unsigned = 0);
  void set_show_map(bool = true);
  void set_show_obj(bool = true);
  void set_show_act(bool = true);
  void refresh();
  void moving(bool);
  bool is_moving();
  void set_map(std::shared_ptr<Map>);
  SDL_Texture* get_win_texture();

  virtual void show_map(int = 0, int = 0) = 0;
  virtual std::shared_ptr<Map> get_map() const = 0;
  // Moves the map into a direction defined via integer argument, and
  // returns the new map offset.
  virtual Offsets move(int) = 0;
  // This function is usually called when the editor window has been
  // resized and the right and lower offsets need to be redetermined.
  virtual Offsets determine_offsets() = 0;
  void set_SDLWindow_object(SDLWindow*);

  // The following methods were specifically added for SqArena:
  virtual SDL_Rect get_tile_coords(int, int) const = 0;
  int put_tile(int, int, SDL_Texture*);
  virtual unsigned tile_size() const = 0;
  virtual void get_center_coords(int&, int&) = 0;
  virtual std::pair<int, int> show_party(int = -1, int = -1) = 0;
  int blit();
  virtual void screen_to_map(int, int, int&, int&) = 0;
  virtual void map_to_screen(int, int, int&, int&) = 0;
  virtual bool adjacent(int, int, int, int) = 0;
  virtual unsigned max_y_coordinate() = 0;
  virtual unsigned max_x_coordinate() = 0;

protected:
  SDL_Texture* _texture;
  SDLWindow* _sdlwindow_object;
  SDL_Renderer* _renderer;
  std::shared_ptr<Map> _map;
  bool _show_grid, _show_map, _show_obj, _show_act;  
  unsigned _top_hidden, _bot_hidden, _left_hidden, _right_hidden;
  bool _party_is_moving;
  std::vector<int> _drawn_icons;

  void resetRenderer();
};

#endif
