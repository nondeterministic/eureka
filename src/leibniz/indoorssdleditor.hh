// This source file is part of eureka
//
// Copyright (c) 2007-2018  Andreas Bauer <baueran@gmail.com>
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

#ifndef INDOORSSDLEDITOR_HH
#define INDOORSSDLEDITOR_HH

#include <vector>
#include <memory>

#include <SDL2/SDL.h>

#include <gtkmm.h>
#include <gtkmm/socket.h>

#include "../map.hh"
#include "../indoorsmap.hh"
#include "sdleditor.hh"

class IndoorsSDLEditor : public SDLEditor
{
public:
  IndoorsSDLEditor(std::shared_ptr<Map>);
  ~IndoorsSDLEditor();

  // Overriding virtual methods
  void show_grid();
  void show_map();
  std::shared_ptr<Map> get_map() const;
  void set_offset(int, int);
  bool grid_on() const;
  void set_grid(bool = true);
  void pixel_to_map(int, int, int&, int&);
  Offsets move(int);
  Offsets determine_offsets(unsigned, unsigned);

  void adjust_offsets(int = 0, int = 0, int = 0, int = 0);
  void set_offsets(unsigned = 0, unsigned = 0, unsigned = 0, unsigned = 0);
  SDL_Rect get_tile_coords(int, int) const;
  int put_tile(int, int, SDL_Texture* = NULL);

protected:
  Offsets offsets();
  unsigned tile_size() const;
  bool convert_icons_to_textures(SDL_Renderer*);

  // Number of pixels of the map that are currently not displayed
  unsigned _top_hidden, _bot_hidden, _left_hidden, _right_hidden;
};

#endif
