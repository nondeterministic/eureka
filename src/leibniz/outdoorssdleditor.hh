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

#ifndef OUTDOORSSDLEDITOR_HH
#define OUTDOORSSDLEDITOR_HH

#include <gtkmm.h>
#include <gtkmm/socket.h>
#include <vector>
#include <SDL.h>
#include "../map.hh"
#include "sdleditor.hh"
#include "../outdoorsmap.hh"

class OutdoorsSDLEditor : public SDLEditor
{
public:
  OutdoorsSDLEditor(Map*);
  ~OutdoorsSDLEditor();

  // Overriding virtual methods
  void show_grid();
  void show_map();
  void open_display(Gtk::Socket*, unsigned, unsigned);
  void resize(unsigned, unsigned);
  Map* get_map() const;
  void set_offset(int, int);
  bool grid_on() const;
  void set_grid(bool = true);
  void pixel_to_map(int, int, int&, int&);
  Offsets move(int);
  Offsets determine_offsets(unsigned, unsigned);

  void adjust_offsets(int = 0, int = 0, int = 0, int = 0);
  void set_offsets(unsigned = 0, unsigned = 0, unsigned = 0, unsigned = 0);
  SDL_Rect get_tile_coords(int, int) const;
  int put_tile(int, int, SDL_Surface* = NULL);
  int put_tile_hex(int, int, SDL_Surface* = NULL);

  int get_screen_x(int) const;
  int get_screen_y(int) const;

protected:
  Offsets offsets();
  unsigned tile_size() const;
  int corner_tile_uneven_offset(void) const;
  void clear();

  unsigned _top_hidden, _bot_hidden, _left_hidden, _right_hidden;
  int _corner_tile_uneven_offset;
  unsigned _width, _height;
  SDL_Surface* _hex_icon;
};

#endif
