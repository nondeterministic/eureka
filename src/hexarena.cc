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

#include <iostream>
#include <utility>
#include <memory>

#include <SDL2/SDL.h>

#include "hexarena.hh"
#include "world.hh"
#include "map.hh"
#include "party.hh"
#include "outdoorsicons.hh"
#include "eureka.hh"

HexArena::HexArena(std::shared_ptr<Map> map)
{
	_map = map;
	_top_hidden = 0;
	_bot_hidden = 0;
	_left_hidden = 0;
	_right_hidden = 0;
	_width = 0;
	_height = 0;
	_corner_tile_uneven_offset = 0;

	int iconsize = OutdoorsIcons::Instance().number_of_icons();
	if (iconsize > 0 && _drawn_icons.size() == 0) {
		for (int i = 0; i < iconsize; i++)
			_drawn_icons.push_back(-1);
	}
	else {
		std::cerr << "ERROR: hexarena.cc: Initialisation error. Icons not yet loaded although they should be.\n";
		exit(-1);
	}

	_corner_tile_uneven_offset = 0;
}

HexArena::~HexArena(void)
{
	_drawn_icons.clear();
}

int HexArena::put_tile_hex(int hex_x, int hex_y, SDL_Texture* brush)
{
  return put_tile(get_screen_x(hex_x), get_screen_y(hex_y), brush);
}

/// Converts a x-hex coordinate to a screen coordinate

int HexArena::get_screen_x(int hex_x) const
{
  return (hex_x - corner_tile_uneven_offset()) * (tile_size() - 10);
}

/// Converts a y-hex coordinate to a screen coordinate

int HexArena::get_screen_y(int hex_y) const
{
  if (hex_y%2 == 0)
    return hex_y/2 * (tile_size() - 1);
  else
    return tile_size()/2 - 1 + hex_y/2 * (tile_size() - 1);
}

/// Gets the absolute coordinates in pixels for a tile on (x, y)

SDL_Rect HexArena::get_tile_coords(int x, int y) const
{
  if (x < 0 || y < 0)
    std::cerr << "ERROR: hexarena.cc: get_tile_coords has wrong coords." << std::endl;

  SDL_Rect rect;
  rect.x = x;
  rect.w = tile_size();
  rect.h = tile_size();
  rect.y = y;
  return rect;
}

std::shared_ptr<Map> HexArena::get_map(void) const
{
  return _map;
}

Offsets HexArena::move(int dir)
{
//	     std::cout << "top_hidden: " << _top_hidden << ", "
//	   	    << "bot_hidden: " << _bot_hidden << ", "
//	   	    << "left_hidden: " << _left_hidden << ", "
//	   	    << "right_hidden: " << _right_hidden << "\n";

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
		// case DIR_RDOWN:
			//   if (_right_hidden >= (tile_size()-10))
		//     adjust_offsets((tile_size()-1), -(tile_size()-1), (tile_size()-10), -(tile_size()-10));
		//   break;
	}

//	   std::cout << "top_hidden: " << _top_hidden << ", "
//	 	    << "bot_hidden: " << _bot_hidden << ", "
//	 	    << "left_hidden: " << _left_hidden << ", "
//	 	    << "right_hidden: " << _right_hidden << "\n";

	return offsets();
}

Offsets HexArena::determine_offsets()
{
	int w, h;
	SDL_QueryTexture(get_win_texture(), NULL, NULL, &w, &h);

	// Makes type conversion below easier...
	unsigned screen_width = w;
	unsigned screen_height = h;

	// Determining the exact width of the hex map in pixels is a bit of
	// a bitch...
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
	else   {
		if (screen_width + _left_hidden < map_width)
			_right_hidden = map_width - _left_hidden - screen_width;
		else
			_right_hidden = 0;
	}

//	 std::cout
//	   << "top: " << _top_hidden << ", "
//	   << "bot: " << _bot_hidden << ", "
//	   << "left: " << _left_hidden << ", "
//	   << "right: " << _right_hidden << std::endl;

	return offsets();
}

/// Returns 0 when tile hex-x-coordinate in the upper left corner is even, otherwise 1.

int HexArena::corner_tile_uneven_offset(void) const
{
  return ((_left_hidden/(tile_size()-10))%2 == 0? 0 : 1);
}

/// Convert the relative screen hex coordinates to the absolute map hex coordinates.

void HexArena::screen_to_map(int sx, int sy, int& mx, int& my)
{
	mx = _left_hidden/(tile_size()-10) + sx; // - corner_tile_uneven_offset();
	// my = _top_hidden/(tile_size()-1)*2 + sy; // - ((sx%2 == 0)? 1 : 0);
	my = _top_hidden/(tile_size()-1)*2 + sy; // - ((sx%2 == 0)? 1 : 0);

	// std::cerr << "screen_to_map: " << sx << ", " << sy << " => " << mx << ", " << my << "\n";
}

/// Convert absolute map coordinates to screen hex coordinates.  The
/// screen coordinates are -1, respectively, if the requested part of
/// the map is currently not visible on the arena/screen.
/// 
/// @param mx Map x-coordinate
/// @param my Map y-coordinate
/// @param sx Relative x-coordinate, -1 if not visible on arena
/// @param sy Relative y-coordinate, -1 if not visible on arena

void HexArena::map_to_screen(int mx, int my, int& sx, int& sy)
{
	sx = -1; sy = -1;

	if (mx >= (int)(_left_hidden/(tile_size()-10)) &&
			mx <= (int)(get_map()->width() - _right_hidden/(tile_size()-10)))
		sx = mx - _left_hidden/(tile_size()-10); // + corner_tile_uneven_offset();

	if (my >= (int)(_top_hidden/(tile_size()-1)*2) &&
			my <= (int)(get_map()->height()*2 - _bot_hidden/(tile_size()-1)))
		sy = my - _top_hidden/(tile_size()-1)*2; // - ((sx%2 == 0)? 1 : 0);

	// std::cerr << "Party x,y: " << Party::Instance().x << ", " << Party::Instance().y << "\n";
	// std::cerr << "map_to_screen: " << mx << ", " << my << " => " << sx << ", " << sy << "\n";
}

/// Returns true if the hex defined by x1, y1 is adjacent to the hex defined by x2, y2; false otherwise.

bool HexArena::adjacent(int x1, int y1, int x2, int y2)
{
  if (x1 < 0 || y1 < 0 || x2 < 0 || y2 < 0)
    return false;
  return ( abs(x1 - x2) <= 1 && abs(y1 - y2) <= 2 );
}

/// Conditional compare, i.e., the enum "sign" is used to determine which type of compare we're dealing with here.

bool HexArena::cond_cmp(int x, int y, int sign)
{
  switch (sign) {
  case GRTR:
    return x > y;
    break;
  case LOWR:
    return x < y;
    break;
  case GEQ:
    return x >= y;
    break;
  default:
    return x <= y;
    break;
  }
}

/// Returns true if icon at (xi, yi) is in the LOS of icon at (xp, yp)
/// (the latter usually being the party's position, and the former some
/// icon on the map).  Otherwise, it returns false.

bool HexArena::in_los(int xi, int yi, int xp, int yp)
{
  // std::cout << 
  //   "xi: " << xi <<
  //   " yi: " << yi <<
  //   " xp: " << xp <<
  //   " yp: " << yp << "\n";

	// Immediate neighbourhood is always visible
	if (abs(xi - xp) <= 1 && abs(yi - yp) <=1)
		return true;

	int delta_x = xi - xp;
	int delta_y = yi - yp;
	float m = (float)delta_y/(float)delta_x;

	std::vector<int> row;
	int icon_no = 0;

#ifndef ADD_ICON
#define ADD_ICON(local_x, local_y)			\
		{ icon_no = get_map()->get_tile(local_x, local_y);	\
		row.push_back(icon_no); }
#endif

	int ystep = -1; int xstep = -1;

	bool steep = abs(((float)yi - (float)yp)/2) > abs(xi - xp);

	SIGN xsign = GRTR;
	SIGN ysign = GRTR;

  // /////////////////////////////////////////////////////
  // Determine octant according to the following picture
  //
  //        3|2
  //      4  |  1
  //     ----P----
  //      5  |  8
  //        6|7
  //
  // where the player, P, is in the center of the above.
  // This is needed in order to know whether to increase
  // the x or y coordinate as the line expands.
  // /////////////////////////////////////////////////////

	if (delta_y <= 0 && delta_x <= 0) {
		// std::cout << "3 & 4\n";
		ystep = -1; xstep = -1;
		ysign = GRTR;
		xsign = GRTR;
	}
	else if (delta_y >= 0 && delta_x >= 0) {
		// std::cout << "7 & 8\n";
		ystep = 1; xstep = 1;
		ysign = LOWR;
		xsign = LOWR;
	}
	else if (delta_y >= 0 && delta_x <= 0) {
		// std::cout << "6 & 5\n";
		ystep = 1; xstep = -1;
		ysign = LOWR;
		xsign = GRTR;
	}
	else if (delta_y <= 0 && delta_x >= 0) {
		// std::cout << "2 & 1\n";
		ystep = -1; xstep = 1;
		ysign = GRTR;
		xsign = LOWR;
	}
	else
		exit(0);

	if (steep) {
		for (int x_temp = xp, y_temp = yp;
				cond_cmp(x_temp, xi, xsign) || cond_cmp(y_temp, yi, ysign);) {
			// Treat current coordinate.
			// When a steep line is drawn, it is possible that the y-part
			// overtakes the end point, so an additional check is added.
			// The overtaking stems from the correction below.
			if (cond_cmp(y_temp, yi, ysign))
				ADD_ICON(x_temp, y_temp);

			// Determine next coordinates
			y_temp += ystep;
			x_temp = ((float)y_temp - (float)yp) / m + (float)xp;

			if ( (x_temp%2 != 0 && y_temp%2 == 0)
					|| (x_temp%2 == 0 && y_temp%2 != 0) )
				y_temp += ystep;
		}
	}
	else {
		int last_x = xp, last_y = yp;
		for (int x_temp = xp, y_temp = yp;
				cond_cmp(x_temp, xi, xsign) || cond_cmp(y_temp, yi, ysign);) {
			// Treat current coordinate
			ADD_ICON(x_temp, y_temp);

			// Determine next coordinates
			x_temp += xstep;
			y_temp = m*((float)x_temp - (float)xp) + (float)yp;

			if ( (x_temp%2 != 0 && y_temp%2 == 0)
					|| (x_temp%2 == 0 && y_temp%2 != 0) )
				y_temp += ystep;

			// Check if the current coordinates differ too much from the
			// previous ones, if so, treat adjacent coordinates in between
			if (abs(last_y - y_temp) > 2)
				ADD_ICON(x_temp, last_y + ystep);
			last_x = x_temp; last_y = y_temp;
		}
	}

	// Now do the actual check for transparency in the row, we just built.
	int semitrans = 0;
	for (unsigned i = 0; i < row.size(); i++) {
		IconProps *props = OutdoorsIcons::Instance().get_props(row[i]);
		if (props) {
			if (props->_trans == PropertyStrength::None) {
				row.clear();
				return false;
			}
			else if (i > 0 && (props->_trans == PropertyStrength::Some)) {
				// Decrease viewing distance by 4 on semi transparent icons, but
				// not when standing on one (i.e., i > 0), rather only when
				// those icons block the view, i.e., are in front of the player.
				if (row.size() - ++semitrans > 4)
					return false;
			}
		}
		else {
			row.clear();
			// std::cerr << "OutdoorsIcons IconProps == null!\n";
			return true;
		}
	}

	row.clear();
	return true;
}

unsigned HexArena::max_y_coordinate()
{
	return _map->height() * 2;
}

unsigned HexArena::max_x_coordinate()
{
	return _map->width();
}

/**
 * x_width and y_width define how large the viewing rectangle should be. Default is 0 for both which means
 * as large as the window allows.  Other values, such as 4 or 6, etc. are there to reflect an increasingly
 * shrinking view port when the night comes on and the player has no torch lit.
 */

void HexArena::show_map(int x_width, int y_width)
{
	_show_obj = true;

	if (!_show_map && !_show_act)
		return;

	// First, blank entire screen to avoid overlay graphic errors
	_sdlwindow->clear_texture_arena();

	// Don't show icons for actions.
	_show_act = false;

	// Recall: x and y are absolute map coordinates on the hex!  x2 and
	// y2 are the relative coordinates to draw hexes on the screen.

	for (unsigned x = _left_hidden / (tile_size() - 10),
			x2 = corner_tile_uneven_offset();
			x < _map->width()-_right_hidden / (tile_size() - 10);
			x++, x2++)
	{
		for (unsigned y = _top_hidden/(tile_size() - 1) * 2 + ((x2%2 == 0)? 0 : 1),
				y2 = (x2%2 == 0)? 0 : 1;
				y < (_map->height() * 2) - (_bot_hidden/(tile_size() - 1));
				y += 2, y2 += 2)
		{
			if (_show_map) {
				int tileno = _map->get_tile(x, y);
				int puttile_errno = 0;

				if (tileno < 0)
					std::cerr << "WARNING: hexarena.cc: Invalid tile number in HexArena::show_map(): " << tileno << std::endl;

				int party_x, party_y;
				map_to_screen(Party::Instance().x, Party::Instance().y, party_x, party_y);
				screen_to_map(party_x, party_y, party_x, party_y);

				// The second line says that if full viewport available OR
				// the party coordinates x_/y_width away from the party + x_/y_width / 2 (party is not the center, so we need to add the offset)
				//
				// In other words, if we run the next if with only the top line it will work as expected but not be able
				// to display night without torch, etc.

				if (in_los(x, y, party_x, party_y) &&
						  ((x_width == 0 && y_width == 0) ||
						   (x-party_x + x_width / 2 <= (unsigned int)x_width && y-party_y + y_width / 2 <= (unsigned int)y_width)))
				{
					// TODO: Animate icons
					// ...

					if ((puttile_errno = put_tile_hex(x2, y2, OutdoorsIcons::Instance().get_sdl_icon(tileno))) != 0)
						std::cerr << "ERROR: hexarena.cc: put_tile() returned " <<  puttile_errno << " in HexArena::show_map()." << std::endl;
				}
				else {
					if ((puttile_errno = put_tile_hex(x2, y2, OutdoorsIcons::Instance().get_sdl_icon(10))) != 0)
						std::cerr << "ERROR: hexarena.cc: put_tile() returned " <<  puttile_errno << " in HexArena::show_map()." << std::endl;
				}
			}

			if (_show_obj) {
				std::pair<unsigned, unsigned> coords(x, y);
				auto found_obj = _map->objs()->equal_range(coords);

				for (auto curr_obj = found_obj.first; curr_obj != found_obj.second; curr_obj++) {
					if (put_tile_hex(x2, y2, OutdoorsIcons::Instance().get_sdl_icon(((MapObj)curr_obj->second).get_icon())) != 0)
						std::cerr << "ERROR: hexarena.cc: put_tile returned error when trying to draw objects.\n";
				}
			}
		}
	}
}

unsigned HexArena::tile_size(void) const
{
  return World::Instance().get_outdoors_tile_size();
}

Offsets HexArena::offsets(void)
{
  Offsets new_offsets;
  new_offsets.top = _top_hidden;
  new_offsets.bot = _bot_hidden;
  new_offsets.left = _left_hidden;
  new_offsets.right = _right_hidden;
  return new_offsets;
}

/// Returns relative coordinates on the hex arena.

void HexArena::get_center_coords(int& x, int& y)
{
	int w, h;
	SDL_QueryTexture(_texture, NULL, NULL, &w, &h);

	x = w / 2 / (tile_size() - 11);
	y = h / tile_size() + corner_tile_uneven_offset();
	// std::cout << "Center: " << x << ", " << y << std::endl;
}

/// x and y are screen coordinates, not map coordinates!

std::pair<int, int> HexArena::show_party(int x, int y)
{
	// Draw party in the middle on default values
	if (x == -1 && y == -1)
		get_center_coords(x, y);
	// TODO: else draw custom position
	// ...

	if (put_tile_hex(x, y, OutdoorsIcons::Instance().get_sdl_icon(20)) != 0)
		std::cerr << "ERROR: hexarena.cc: put_tile returned failure when trying to show party.\n";

	int mx, my;
	screen_to_map(x, y, mx, my);

	// std::cerr << "show_party: " << x << ", " << y << " => " << mx << ", " << my << "\n";

	std::pair<int, int> new_coords;
	new_coords.first = mx;
	new_coords.second = my;

	return new_coords;
}
