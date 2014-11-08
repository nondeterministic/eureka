// leibniz
//
// Copyright (c) 2009  Andreas Bauer <baueran@gmail.com>
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

#ifndef SDLEDITOR_HH
#define SDLEDITOR_HH

#include <string>
#include <gtkmm.h>
#include <gtkmm/socket.h>
#include <SDL.h>
#include "../map.hh"

typedef struct
{
  int top, bot, left, right;
} Offsets;

enum LDIR {
  DIR_UP,
  DIR_DOWN,
  DIR_RIGHT,
  DIR_LEFT,
  DIR_LUP,
  DIR_LDOWN,
  DIR_RUP,
  DIR_RDOWN
};

class SDLEditor
{
public:
  SDLEditor();
  virtual ~SDLEditor();

  // Does the return type need to be static to ensure only one object
  // is created?
  static SDLEditor* create(std::string, std::string);

  void set_show_map(bool = true);
  void set_show_obj(bool = true);
  void set_show_act(bool = true);
  void refresh();

  virtual void show_grid() = 0;
  virtual void show_map() = 0;
  virtual void open_display(Gtk::Socket*, unsigned, unsigned) = 0;
  virtual void resize(unsigned, unsigned) = 0;
  virtual Map* get_map() const = 0;
  virtual bool grid_on() const = 0;
  virtual void set_grid(bool = true) = 0;
  virtual void pixel_to_map(int, int, int&, int&) = 0;
  // Moves the map into a direction defined via integer argument, and
  // returns the new map offset.
  virtual Offsets move(int) = 0;
  // This function is usually called when the editor window has been
  // resized and the right and lower offsets need to be redetermined.
  virtual Offsets determine_offsets(unsigned, unsigned) = 0;

  // The following methods were specifically added for
  // IndoorsSDLEditor:
  virtual void adjust_offsets(int = 0, int = 0, int = 0, int = 0) = 0;
  virtual 
  void set_offsets(unsigned = 0, unsigned = 0, unsigned = 0, unsigned = 0) = 0;
  virtual SDL_Rect get_tile_coords(int, int) const = 0;
  virtual int put_tile(int, int, SDL_Surface* = NULL) = 0;

protected:
  SDL_Surface* _sdl_surf;
  Map* _map;
  bool _show_grid, _show_map, _show_obj, _show_act;
};

#endif
