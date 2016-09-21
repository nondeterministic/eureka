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
#include <string>
#include <sstream>
#include <cmath>
#include <memory>
#include <utility>

#include <SDL.h>
#include <SDL_image.h>

#include <boost/filesystem/path.hpp>

#include "outdoorssdleditor.hh"
#include "../outdoorsicons.hh"
#include "../world.hh"
#include "../map.hh"
#include "../config.h"

OutdoorsSDLEditor::OutdoorsSDLEditor(std::shared_ptr<Map> map)
{
	_map = map;
	_top_hidden = 0;
	_bot_hidden = 0;
	_left_hidden = 0;
	_right_hidden = 0;
	_width = 0;
	_height = 0;
	_corner_tile_uneven_offset = 0;

	// Load hex grid
	boost::filesystem::path icon_path = boost::filesystem::path((std::string)(DATADIR));
	icon_path /= (std::string)PACKAGE_NAME;
	icon_path /= "data";
	icon_path /= "leibniz";
	icon_path /= "34x34hex.png";

	//	std::string icon_path =
	//			(std::string)DATADIR + "/" +
	//			(std::string)PACKAGE +
	//			"/data/leibniz/34x34hex.png";

	_hex_icon = IMG_Load(icon_path.c_str());
	if (!_hex_icon)
		std::cerr << "ERROR: outdoorssdleditor.cc: Couldn't load hex icon: " << IMG_GetError() << std::endl;

	SDL_SetAlpha(_hex_icon, !SDL_SRCALPHA, 255);
}

OutdoorsSDLEditor::~OutdoorsSDLEditor(void)
{
	SDL_Quit();
}

// x and y are screen coordinates in pixels

int OutdoorsSDLEditor::put_tile(int x, int y, SDL_Surface* brush)
{
  if (x < 0 || y < 0) {
    std::cerr << "WARNING: outdoorssdleditor.cc: put_tile has wrong coords." << std::endl;
    return -1;
  }

  if (brush == NULL) {
    std::cerr << "WARNING: outdoorssdleditor.cc: Brush to paint tile is NULL. " << std::endl;
    return 0;
  }
  
  SDL_Rect rect = get_tile_coords(x, y);
  return SDL_BlitSurface(brush, NULL, _sdl_surf, &rect);
}

int OutdoorsSDLEditor::put_tile_hex(int hex_x, int hex_y, SDL_Surface* brush)
{
  return put_tile(get_screen_x(hex_x), get_screen_y(hex_y), brush);
}

// Converts a x-hex coordinate to a screen coordinate

int OutdoorsSDLEditor::get_screen_x(int hex_x) const
{
  return (hex_x - corner_tile_uneven_offset()) * (World::Instance().get_outdoors_tile_size() - 10);
}

// Converts a y-hex coordinate to a screen coordinate

int OutdoorsSDLEditor::get_screen_y(int hex_y) const
{
  if (hex_y%2 == 0)
    return hex_y/2 * (World::Instance().get_outdoors_tile_size() - 1);
  else
    return (World::Instance().get_outdoors_tile_size())/2 - 1 + 
      hex_y/2 * (World::Instance().get_outdoors_tile_size() - 1);
}

// Gets the absolute coordinates in pixels for a tile on (x, y)

SDL_Rect OutdoorsSDLEditor::get_tile_coords(int x, int y) const
{
  if (x < 0 || y < 0)
    std::cerr << "WARNING: outdoorssdleditor.cc: get_tile_coords has wrong coords." << std::endl;

  SDL_Rect rect;
  rect.x = x;
  rect.w = tile_size();
  rect.h = tile_size();
  rect.y = y;
  return rect;
}

void OutdoorsSDLEditor::pixel_to_map(int x, int y, int& map_x, int& map_y)
{
	// In which rectangular cell did the user click?  Stored in these variables.
	unsigned x_cell = x/(tile_size()-10),
			 y_cell = y/(tile_size()-1);

	// Local wrt. the box the user clicked on
	int local_x = x - x_cell * (tile_size()-10);
	int local_y = y - y_cell * (tile_size()-1);

	// The hex coordinates (!= cell coordinates, which are the rectangular ones, and hex ones are rather different)
	int hex_x = 0, hex_y = 0;

	x_cell += corner_tile_uneven_offset();

	// User clicked in cell where one big hexagon dominates
	if (x_cell%2 == 0) {
		if (local_x >= 9) {
			// Big hex
			hex_x = x_cell;
			hex_y = y_cell*2;
		}
		else
		{
			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// This was a b*tch to get right (20 or so years after high school :-P):
			//
			// We want to determine where we clicked in a rectangle like this:
			// _______
			// |  /  |
			// | /   |
			// | \   |
			// |__\__|
			//
			// Here's how I determined if the user clicked in the upper left corner:
			//
			// I had two points (9,0) and (0,16) of the part-hex and I first determined the slope between them (recall, the slope m = (y2 - y1)/(x2 - x1)):
			//
			//   (0-16)/(9-0) = -1.78
			//
			// Then the equation for this line of the hex is, if we take point (9,0) and y - Py = m(x - Px) and resolve for y:
			//
			//   y = -1.78x - 16.02
			//
			// That is, given local_x, we look if local_y is below or above the expected y from the equation.
			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			if ((float)local_y < -1.78 * (float)local_x + 16.02) {
				// Upper left
				hex_x = x_cell - 1;
				hex_y = y_cell*2 - 1;
			}
			// Here I used point (0,16):
			else if ((float)local_y > 1.78 * (float)local_x + 16.02) {
				// Lower left
				hex_x = x_cell - 1;
				hex_y = y_cell*2 + 1;
			}
			else {
				// Big hex
				hex_x = x_cell;
				hex_y = y_cell*2;
			}
		}
	}
	// User clicked in cell where there are two same size hexagons
	else {
		if (local_x >= 9 && local_y < 16) {
			// Upper half
			hex_x = x_cell;
			hex_y = y_cell*2 - 1;
		}
		else if (local_x >=9 && local_y >= 16) {
			// Lower half
			hex_x = x_cell;
			hex_y = y_cell*2 + 1;
		}
		// The following calculations in the else case are analoguous to
		// the above ones:
		else {
			if (local_y < 16) {
				if ((float)local_y > 1.78 * (float)local_x) {
					// Small triangle
					hex_x = x_cell - 1;
					hex_y = y_cell*2;
				}
				else {
					// Upper half
					hex_x = x_cell;
					hex_y = y_cell*2 - 1;
				}
			}
			else if (local_y >= 16) {
				if ((float)local_y > -1.78 * (float)local_x + 33) {
					// Lower half
					hex_x = x_cell;
					hex_y = y_cell*2 + 1;
				}
				else {
					// Small triangle
					hex_x = x_cell - 1;
					hex_y = y_cell*2;
				}
			}
		}
	}

	// Now take into account the x-offset, too
	map_x = hex_x + _left_hidden/(tile_size()-10) - corner_tile_uneven_offset();
	map_y = hex_y + _top_hidden/(tile_size()-1)*2; // corner_tile_uneven_offset();  // TODO: I INSERTED THE +1 REMOVE AGAIN!
}

void OutdoorsSDLEditor::set_grid(bool state)
{
}
 
bool OutdoorsSDLEditor::grid_on(void) const 
{ 
	return true;
}

std::shared_ptr<Map> OutdoorsSDLEditor::get_map(void) const
{
	return _map;
}

void OutdoorsSDLEditor::adjust_offsets(int top, int bot, int left, int right)
{
	_top_hidden += top;
	_bot_hidden += bot;
	_left_hidden += left;
	_right_hidden += right;
}

void OutdoorsSDLEditor::set_offsets(unsigned top,  unsigned bot, unsigned left, unsigned right)
{
	_top_hidden = top;
	_bot_hidden = bot;
	_left_hidden = left;
	_right_hidden = right;
}

Offsets OutdoorsSDLEditor::move(int dir)
{
	switch (dir) {
	case DIR_UP:
		if (_top_hidden >= (tile_size()-1))
			adjust_offsets(-(tile_size()-1), (tile_size()-1), 0, 0);
		break;
	case DIR_DOWN:
		if (_bot_hidden >= (tile_size()-1))
			adjust_offsets((tile_size()-1), -(tile_size()-1), 0, 0);
		break;
	case DIR_LEFT:
		if (_left_hidden >= (tile_size()-10))
			adjust_offsets(0, 0, -(tile_size()-10), (tile_size()-10));
		break;
	case DIR_RIGHT:
		if (_right_hidden >= (tile_size()-10))
			adjust_offsets(0, 0, (tile_size()-10), -(tile_size()-10));
		break;
	}

	return offsets();
}

Offsets OutdoorsSDLEditor::determine_offsets(unsigned screen_width, unsigned screen_height)
{
	// Determining the exact width of the hex map in pixels is a bit of a bitch...
	unsigned map_width  = (get_map()->width())*(tile_size()-10) + 9;
	// ...height is easy though...
	unsigned map_height = get_map()->height()*(tile_size()-1);

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

// Returns 0 when tile hex-x-coordinate in the upper left corner is even, otherwise 1.

int OutdoorsSDLEditor::corner_tile_uneven_offset(void) const
{
	return ((_left_hidden/(tile_size()-10))%2 == 0? 0 : 1);
}

void OutdoorsSDLEditor::show_map(void)
{
	if (!_show_map && !_show_act)
		return;

	// Recall: x and y are absolute map coordinates on the hex!
	// x2 and y2 are the relative coordinates to draw hexes on the screen.

	for (unsigned x = _left_hidden/(tile_size()-10),
			x2 = corner_tile_uneven_offset();
			x < _map->width()-_right_hidden/(tile_size() - 10);
			x++, x2++)
	{
		// The loop should look exactly as in hexarena.cc.  And does now.
		for (unsigned y = _top_hidden/(tile_size()-1)*2 + ((x2%2 == 0)? 0 : 1),
				y2 = (x2%2 == 0)? 0 : 1;
				y < (_map->height()*2)-(_bot_hidden/(tile_size()-1));
				y += 2, y2 += 2)
		{
			if (_show_map) {
				int tileno = 0;
				tileno = _map->get_tile(x, y);
				int puttile_errno = 0;

				if (tileno < 0)
					std::cerr << "ERROR: outdoorssdleditor.cc: Invalid tile number in OutdoorsSDLEditor::show_map(): " << tileno << std::endl;

				if ((puttile_errno = put_tile_hex(x2, y2, OutdoorsIcons::Instance().get_sdl_icon(tileno))) != 0)
					std::cerr << "ERROR: outdoorssdleditor.cc: put_tile() returned " <<  puttile_errno << " in OutdoorsSDLEditor::show_map()." << std::endl;
			}

			if (_show_act) {
				std::vector<std::shared_ptr<Action>> _acts = _map->get_actions(x, y);

				if (_acts.size() > 0) {
					// std::cout << "Actions: " << _acts.size() << "\n";
					// std::cout << "Putting action (" << x << ", " << y << ")" << " to " << "(" << x2 << ", " << y2 << ")" << std::endl;
					put_tile_hex(x2, y2, OutdoorsIcons::Instance().get_sdl_icon(20));
				}
			}
		}
	}
}

unsigned OutdoorsSDLEditor::tile_size(void) const
{
	return World::Instance().get_outdoors_tile_size();
}

void OutdoorsSDLEditor::resize(unsigned w, unsigned h)
{
	_width = w;
	_height = h;
	_sdl_surf = SDL_SetVideoMode(_width, _height, 0, SDL_RESIZABLE);
}

void OutdoorsSDLEditor::show_grid(void)
{
  // First clear display, or we get some nasty graphics artifacts when
  // scrolling horizontally.
  clear();

  // Sort, of obvious note: If we want another grid, replace in the
  // column%2 thinggie the 0 for the 16 and vice versa.
  for (unsigned x = 0, column = 0; x < _width; 
       x += (tile_size() - 10), column++)
    {
      for (unsigned y = ((column+corner_tile_uneven_offset())%2 == 0? 0 : 16); 
	   y < _height; 
	   y += (tile_size() - 1))
        put_tile(x, y, _hex_icon);
    }
  
  // Later we can show the ugly green square grid again, if we need to
  // by #defining GREEN_GRID_ON.
#ifdef GREEN_GRID_ON
  SDL_Rect rect;
  for (unsigned x = 0; x < _width; x++)
    {
      if (x%(tile_size()-10) == 0)
	{
	  rect.x = x;
	  rect.y = 0;
	  rect.w = 1;
	  rect.h = _height;
	  SDL_FillRect(_sdl_surf, 
		       &rect, 
		       SDL_MapRGB(_sdl_surf->format, 0, 200, 0)); 
	}
    }
  for (unsigned y = 0; y < _height; y++)
    {
      if (y%(tile_size()-1) == 0)
	{
	  rect.x = 0;
	  rect.y = y;
	  rect.w = _width;
	  rect.h = 1;
	  SDL_FillRect(_sdl_surf, 
		       &rect, 
		       SDL_MapRGB(_sdl_surf->format, 0, 200, 0)); 
	}
    }
#endif
}

void OutdoorsSDLEditor::open_display(Gtk::Socket* socket, 
				     unsigned width, 
				     unsigned height)
{
	char* winhack = new char[32];
	_width = width; _height = height;

	// Make SDL windows appear inside socket window
	std::stringstream sdlhack;
	sdlhack << "SDL_WINDOWID=" << socket->get_id() << std::ends;
	sprintf(winhack,sdlhack.str().c_str());
	SDL_putenv(winhack);

	// Init SDL window
	if (SDL_Init(SDL_INIT_VIDEO))
		throw std::runtime_error("ERROR: outdoorssdleditor.cc: Error initialising SDL.");
	else
		_sdl_surf = SDL_SetVideoMode(_width, _height, 0,
				SDL_RESIZABLE | SDL_DOUBLEBUF);

	delete winhack;
}

void OutdoorsSDLEditor::clear(void)
{
	SDL_FillRect(SDL_GetVideoSurface(), NULL, 0);
}

Offsets OutdoorsSDLEditor::offsets(void)
{
	Offsets new_offsets;
	new_offsets.top = _top_hidden;
	new_offsets.bot = _bot_hidden;
	new_offsets.left = _left_hidden;
	new_offsets.right = _right_hidden;
	return new_offsets;
}
