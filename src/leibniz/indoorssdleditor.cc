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

#include <iostream>
#include <sstream>
#include <memory>
#include <SDL.h>
#include <SDL_image.h>
#include "indoorssdleditor.hh"
#include "../indoorsicons.hh"
#include "../world.hh"
#include "../map.hh"
#include "../mapobj.hh"
#include "../action.hh"

IndoorsSDLEditor::IndoorsSDLEditor(Map* map)
{
  _map = map;
  _top_hidden = 0;
  _bot_hidden = 0;
  _left_hidden = 0;
  _right_hidden = 0;
  _width = 0;
  _height = 0;
}

IndoorsSDLEditor::~IndoorsSDLEditor(void)
{
  SDL_Quit();
}

int IndoorsSDLEditor::put_tile(int x, int y, SDL_Surface* brush)
{
  if (x < 0 || y < 0) {
    std::cerr << "Warning: put_tile has wrong coords." << std::endl;
    return -1;
  }

  if (brush == NULL) {
    std::cerr << "Warning: Brush to paint tile is NULL. " 
	      << "Will use white tile instead." << std::endl;
    exit(0);
    SDL_Rect rect = get_tile_coords(x, y);
    SDL_FillRect(_sdl_surf, 
		 &rect, 
		 SDL_MapRGB(_sdl_surf->format, 255, 255, 255)); 
    return 0;
  }
  else {
    SDL_Rect rect = get_tile_coords(x, y);
    return SDL_BlitSurface(brush, NULL, _sdl_surf, &rect);
  }
}

// Gets the absolute coordinates in pixels for a tile on (x, y)

SDL_Rect IndoorsSDLEditor::get_tile_coords(int x, int y) const
{
  if (x < 0 || y < 0)
    std::cerr << "Warning: get_tile_coords has wrong coords." << std::endl;

  unsigned grid_pixel = grid_on() ? 1 : 0;

  SDL_Rect rect;
  rect.x = tile_size()*x + grid_pixel;
  rect.w = tile_size()   - grid_pixel;
  rect.h = tile_size()   - grid_pixel;
  rect.y = tile_size()*y + grid_pixel;
  return rect;
}

void IndoorsSDLEditor::pixel_to_map(int x, int y, int& map_x, int& map_y)
{
  map_x = (x+_left_hidden)/(World::Instance().get_indoors_tile_size() 
			    + (grid_on() ? 1 : 0));
  map_y = (y+_top_hidden)/(World::Instance().get_indoors_tile_size() 
			   + (grid_on() ? 1 : 0));
}

void IndoorsSDLEditor::set_grid(bool state)
{
  _show_grid = state;
}

bool IndoorsSDLEditor::grid_on(void) const
{
  return _show_grid;
}

Map* IndoorsSDLEditor::get_map(void) const
{
  return _map;
}

void IndoorsSDLEditor::adjust_offsets(int top, int bot, int left, int right)
{
  _top_hidden += top;
  _bot_hidden += bot;
  _left_hidden += left;
  _right_hidden += right;
}

void IndoorsSDLEditor::set_offsets(unsigned top,  unsigned bot, 
				   unsigned left, unsigned right)
{
  _top_hidden = top;
  _bot_hidden = bot;
  _left_hidden = left;
  _right_hidden = right;
}

Offsets IndoorsSDLEditor::offsets(void)
{
  Offsets new_offsets;
  new_offsets.top = _top_hidden;
  new_offsets.bot = _bot_hidden;
  new_offsets.left = _left_hidden;
  new_offsets.right = _right_hidden;
  return new_offsets;
}

Offsets IndoorsSDLEditor::determine_offsets(unsigned screen_width, 
					    unsigned screen_height)
{
  unsigned map_width  = get_map()->width()*tile_size();
  unsigned map_height = get_map()->height()*tile_size();

  // Does the map height fit into the window height?
  if (map_height <= screen_height) {
    _bot_hidden = 0;
    _top_hidden = 0;
  }
  else {
    if (screen_height + _top_hidden < map_height)
      _bot_hidden = map_height - _top_hidden - screen_height;
    else
      _bot_hidden = 0;
  }
  
  if (map_width <= screen_width) {
    _left_hidden = 0;
    _right_hidden = 0;
  }
  else {
    if (screen_width + _left_hidden < map_width)
      _right_hidden = map_width - _left_hidden - screen_width;
    else
      _right_hidden = 0;
  }
  
  return offsets();
}

Offsets IndoorsSDLEditor::move(int dir)
{
  switch (dir)
    {
    case DIR_UP:
      if (_top_hidden >= tile_size())
	adjust_offsets(-tile_size(), tile_size(), 0, 0);
      break;
    case DIR_DOWN:
      if (_bot_hidden >= tile_size())
	adjust_offsets(tile_size(), -tile_size(), 0, 0);
      break;
    case DIR_LEFT:
      if (_left_hidden >= tile_size())
	adjust_offsets(0, 0, -tile_size(), tile_size());
      break;
    case DIR_RIGHT:
      if (_right_hidden >= tile_size())
	adjust_offsets(0, 0, tile_size(), -tile_size());      
      break;
    }

  return offsets();
}

void IndoorsSDLEditor::show_map(void)
{
	if (!_show_map && !_show_obj && !_show_act)
		return;

	// The upper bounds should be -1 in the foor loop?
	for (unsigned x = _left_hidden/tile_size(), x2 = 0; x < _map->width()-_right_hidden/tile_size(); x++, x2++) {
		for (unsigned y = _top_hidden/tile_size(), y2 = 0; y < _map->height()-_bot_hidden/tile_size(); y++, y2++) {
			int tileno = _map->get_tile(x, y);
			int puttile_errno = 0;

			if (tileno < 0)
				std::cerr
				<< "Invalid tile number in IndoorsSDLEditor::show_map()."
				<< std::endl;

			if (_show_map) {
				if ((puttile_errno =
						put_tile(x2, y2,
								IndoorsIcons::Instance().get_sdl_icon(tileno)))
						!= 0)
					std::cerr << "put_tile() returned "
					<<  puttile_errno
					<< " in IndoorsSDLEditor::show_map()." << std::endl;
			}

			if (_show_obj) {
				std::pair<unsigned, unsigned> coords;
				coords.first = x;
				coords.second = y;

				std::pair
				<boost::unordered_multimap
				<std::pair<unsigned, unsigned>, MapObj>::iterator,
				boost::unordered_multimap
				<std::pair<unsigned, unsigned>, MapObj>::iterator>
				found_obj = _map->objs()->equal_range(coords);

				// boost::unordered_multimap
				// 	       <std::pair<unsigned, unsigned>, MapObj>::const_iterator
				for (auto curr_obj = found_obj.first; curr_obj != found_obj.second; curr_obj++)
					put_tile(x2, y2, IndoorsIcons::Instance().get_sdl_icon(((MapObj)curr_obj->second).get_icon()));
			}

			if (_show_act) {
				std::shared_ptr<Action> _act = _map->get_action(x, y);
				// TODO: Below, 150 is just some randomly chosen icon...
				if (_act != NULL)
					put_tile(x2, y2, IndoorsIcons::Instance().get_sdl_icon(150));
			}
		}
	}
}

unsigned IndoorsSDLEditor::tile_size(void) const
{
  return World::Instance().get_indoors_tile_size() + (grid_on() ? 1 : 0);
}

void IndoorsSDLEditor::resize(unsigned w, unsigned h)
{
  _width = w;
  _height = h;
  _sdl_surf = SDL_SetVideoMode(_width, _height, 0, SDL_RESIZABLE);
}

void IndoorsSDLEditor::show_grid(void)
{
  clear();

  if (!grid_on())
    return;

  SDL_Rect rect;

  for (unsigned x = 0; x < _width; x++)
    {
      if (x%tile_size() == 0)
	{
	  rect.x = x;
	  rect.y = 0;
	  rect.w = 1;
	  rect.h = _height;
	  SDL_FillRect(_sdl_surf, 
		       &rect, 
		       SDL_MapRGB(_sdl_surf->format, 255, 0, 0)); 
	}
    }

  for (unsigned y = 0; y < _height; y++)
    {
      if (y%tile_size() == 0)
	{
	  rect.x = 0;
	  rect.y = y;
	  rect.w = _width;
	  rect.h = 1;
	  SDL_FillRect(_sdl_surf, 
		       &rect, 
		       SDL_MapRGB(_sdl_surf->format, 255, 0, 0)); 
	}
    }
}

void IndoorsSDLEditor::open_display(Gtk::Socket* socket, 
				    unsigned width, 
				    unsigned height)
{
  char* winhack = new char[32];
  _width = width; _height = height;

  // Make SDL windows appear inside socket window
  std::stringstream sdlhack;
  sdlhack << "SDL_WINDOWID=" << socket->get_id();
  sprintf(winhack, sdlhack.str().c_str());
  SDL_putenv(winhack);
  
  // Init SDL window
  if (SDL_Init(SDL_INIT_VIDEO))
    throw std::runtime_error("Error initialising SDL.");
  else
    _sdl_surf = SDL_SetVideoMode(_width, _height, 0, 
				 SDL_RESIZABLE | SDL_DOUBLEBUF);

  delete winhack;
}


void IndoorsSDLEditor::clear(void)
{
  SDL_FillRect(SDL_GetVideoSurface(), NULL, 0);
}
