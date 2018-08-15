// This source file is part of eureka
//
// Copyright (c) 2007-2018 Andreas Bauer <a@pspace.org>
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
#include <list>
#include <utility>
#include <memory>
#include <algorithm>

#include <boost/unordered_map.hpp>

#include "squarearena.hh"
#include "world.hh"
#include "map.hh"
#include "party.hh"
#include "indoorsicons.hh"
#include "eureka.hh"
#include "soundsample.hh"
#include "playlist.hh"
#include "config.h"


SquareArena::SquareArena(std::shared_ptr<Map> map)
{
	_map = map;
	_top_hidden = 0;
	_bot_hidden = 0;
	_left_hidden = 0;
	_right_hidden = 0;
	_width = 0;
	_height = 0;

	int iconsize = IndoorsIcons::Instance().number_of_icons();
	if (iconsize > 0 && _drawn_icons.size() == 0) {
		for (int i = 0; i < iconsize; i++)
			_drawn_icons.push_back(-1);
	}
	else {
		std::cerr << "ERROR: squarearena.cc: Initialisation error. Icons not yet loaded although they should be.\n";
		exit(-1);
	}

	_corner_tile_uneven_offset = 0;
	_water_anim = 0;
	_party_anim = 0;
}

SquareArena::~SquareArena(void)
{
	_drawn_icons.clear();
}

/// Gets the absolute coordinates in pixels for a tile on (x, y)

SDL_Rect SquareArena::get_tile_coords(int x, int y) const
{
	if (x < 0 || y < 0)
		std::cerr << "WARNING: squarearena.cc: get_tile_coords was called with invalid arguments." << std::endl;

	SDL_Rect rect;
	rect.x = tile_size() * x;
	rect.w = tile_size();
	rect.h = tile_size();
	rect.y = tile_size() * y;
	return rect;
}

std::shared_ptr<Map> SquareArena::get_map(void) const
{
	return _map;
}

Offsets SquareArena::move(int dir)
{
	//     std::cout << "top_hidden: " << _top_hidden << ", "
	//          << "bot_hidden: " << _bot_hidden << ", "
	//          << "left_hidden: " << _left_hidden << ", "
	//          << "right_hidden: " << _right_hidden << "\n";

	switch (dir) {
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

	//     std::cout << "top_hidden: " << _top_hidden << ", "
	//          << "bot_hidden: " << _bot_hidden << ", "
	//          << "left_hidden: " << _left_hidden << ", "
	//          << "right_hidden: " << _right_hidden << "\n";

	return offsets();
}

// Determine map offsets in terms of pixels - not tiles!

Offsets SquareArena::determine_offsets()
{
	int w,h;
	SDL_QueryTexture(_texture, NULL, NULL, &w, &h);

	unsigned screen_width = w;
	unsigned screen_height = h;

	unsigned map_width  = get_map()->width()  * tile_size();
	unsigned map_height = get_map()->height() * tile_size();

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

// Convert the relative screen hex coordinates to the absolute map
// hex coordinates.

void SquareArena::screen_to_map(int sx, int sy, int& mx, int& my)
{
	mx = _left_hidden / (tile_size()) + sx; // - corner_tile_uneven_offset();
	my = _top_hidden / (tile_size()) + sy; // - ((sx%2 == 0)? 1 : 0);
}

/// Convert absolute map coordinates to screen hex coordinates.  The
/// screen coordinates are -1, respectively, if the requested part of
/// the map is currently not visible on the arena/screen.
///
/// @param mx Map x-coordinate
/// @param my Map y-coordinate
/// @param sx Relative x-coordinate, -1 if not visible on arena
/// @param sy Relative y-coordinate, -1 if not visible on arena

void SquareArena::map_to_screen(int mx, int my, int& sx, int& sy)
{
	sx = -1;
	sy = -1;

	if (mx >= (int) (_left_hidden / (tile_size())) &&
			mx <= (int) (get_map()->width() - _right_hidden / (tile_size())))
		sx = mx - _left_hidden / (tile_size()); // + corner_tile_uneven_offset();

	if (my >= (int) (_top_hidden / (tile_size())) &&
			my <= (int) (get_map()->height() - _bot_hidden / (tile_size())))
		sy = my - _top_hidden / (tile_size());
}

bool SquareArena::in_los(int xi, int yi, int xp, int yp)
{
	bool steep = abs(yi - yp) > abs(xi - xp);

#ifndef SWAP_INT
#define SWAP_INT(local_x, local_y) {            \
    int tmp = local_y;                          \
    local_y = local_x;                          \
    local_x = tmp;                              \
  }
#endif

	if (steep) {
		SWAP_INT(xp, yp);
		SWAP_INT(xi, yi);
    }

	if (xp > xi) {
		SWAP_INT(xp, xi);
		SWAP_INT(yp, yi);
    }

	int deltax = xi - xp;
	int deltay = abs(yi - yp);

	// TODO: Ignores night or light; shows always icons right next to player, is that desired?
	if (deltax <= 1 && deltay <= 1)
		return true;

	int error = ((float) deltax) / 2.0;
	int ystep = 0;
	int y = yp;
	if (yp < yi)
		ystep = 1;
	else
		ystep = -1;

	std::vector<int> row, row_objs;

	// This is a temporary object where we put for each location on the map, i.e., each element in the row, ALL the icons that belong to it: map icons AND object icons.
	// This is the only way, to get a realistic LOS result.  However, we do not use this map to draw the icons.  For this purpose we use the vectors row and row_objs
	// as before.
	boost::unordered_map<int, std::vector<int>> tmp_row_and_objs;

	int icon_no = 0;

#ifndef ADD_ICON
#define ADD_ICON(local_x, local_y) {                    			\
	icon_no = get_map()->get_tile(local_x, local_y);    			\
	tmp_row_and_objs[row.size()].push_back(icon_no);                \
    row.push_back(icon_no);                             			\
  }
#endif

	for (int x = xp; x <= xi; x++) {
		if (steep) {
			// Objects
			std::pair<int,int> coords = std::make_pair(y, x);
			auto avail_objects = get_map()->objs()->equal_range(coords);

			if (avail_objects.first != avail_objects.second) {
				for (auto curr_obj = avail_objects.first; curr_obj != avail_objects.second; curr_obj++) {
					MapObj& the_obj = curr_obj->second;
					IconProps* props = IndoorsIcons::Instance().get_props(the_obj.get_icon());
					if (!(props->_trans == PropertyStrength::Full)) {
						tmp_row_and_objs[std::max((int)row.size(), 0)].push_back(the_obj.get_icon());
						// Add at most one object to row, and only if it isn't transparent. So we have one obj per location on the map.
						row_objs.push_back(the_obj.get_icon());
						break;
					}
				}
			}

			// Normal icons
			ADD_ICON(y, x);
		}
		else {
			// Objects
			std::pair<int,int> coords = std::make_pair(x, y);
			auto avail_objects = get_map()->objs()->equal_range(coords);

			if (avail_objects.first != avail_objects.second) {
				for (auto curr_obj = avail_objects.first; curr_obj != avail_objects.second; curr_obj++) {
					MapObj& the_obj = curr_obj->second;
					IconProps* props = IndoorsIcons::Instance().get_props(the_obj.get_icon());
					if (!(props->_trans == PropertyStrength::Full)) {
						tmp_row_and_objs[std::max((int)row.size(), 0)].push_back(the_obj.get_icon());
						// Add at most one object to row, and only if it isn't transparent. So we have one obj per location on the map.
						row_objs.push_back(the_obj.get_icon());
						break;
					}
				}
			}

			// Normal icons
			ADD_ICON(x, y);
		}

		error -= deltay;

		if (error < 0) {
			y += ystep;
			error += deltax;
		}
	}

	int semitrans = 0;
	for (unsigned i = 1; i < row.size() - 1; i++) {
		std::vector<int>& icons = tmp_row_and_objs[i];

		for (unsigned l = 0; l < icons.size(); l++) {
			IconProps* props = IndoorsIcons::Instance().get_props(icons[l]);

			if (props && (props->_trans == PropertyStrength::None))
				return false;
			else if (i > 0 && props && (props->_trans == PropertyStrength::Some)) {
				// Decrease viewing distance by 4 on semi transparent icons, but
				// not when standing on one (i.e., i > 0), rather only when
				// those icons block the view, i.e., are in front of the player.
				if (row.size() - ++semitrans > 4)
					return false;
			}
		}
	}

	return true;
}

// Returns true if tile x,y is within the radius of a light source, false otherwise

bool SquareArena::is_illuminated(int x, int y)
{
	int party_light_radius = Party::Instance().light_radius();

	// Party could carry a light source...
	if (party_light_radius > 0)
		if (abs(Party::Instance().x - x) <= party_light_radius && abs(Party::Instance().y - y) <= party_light_radius)
			return true;

	// There could be objects lying around that are light sources...
	for (auto map_obj_pair = _map->objs()->begin(); map_obj_pair != _map->objs()->end(); map_obj_pair++) {
		MapObj& obj = map_obj_pair->second;
		int radius  = IndoorsIcons::Instance().get_props(obj.get_icon())->light_radius();

		if (radius > 0) {
			unsigned objx, objy;
			obj.get_coords(objx, objy);

			if (abs((int)objx - x) <= radius && abs((int)objy - y) <= radius) {
				return true;
			}
		}
	}

	// Check if some ordinary icons that are not objects illuminate the location in question
	int max_radius = 6; // TODO: This is a hack, it means that a light source can at most illuminate stuff 6 icons around it.
				        // For the game that should be sufficient, but it's not very realistic.
	for (int xoff = -max_radius; xoff < max_radius; xoff++) {
		for (int yoff = -max_radius; yoff < max_radius; yoff++) {
			int check_x = x + xoff;
			int check_y = y + yoff;

			if (check_x >= 0 && check_y >= 0 && check_x < (int)(_map->width()) && check_y < (int)(_map->height())) {
				int radius = IndoorsIcons::Instance().get_props(_map->get_tile((unsigned)check_x, (unsigned)check_y))->light_radius();

				if (radius > 0) {
					if (abs(check_x) - x <= radius && abs(check_y) - y <= radius) {
						return true;
					}
				}
			}
		}
	}

	return false;
}

/**
 * x_width and y_width define how large the viewing rectangle should be. Default is 0 for both which means
 * as large as the window allows.  Other values, such as 4 or 6, etc. are there to reflect an increasingly
 * shrinking view port when the night comes on and the player has no torch lit.
 */

void SquareArena::show_map(int x_width, int y_width)
{
//	 std::cout << "Left hidden: " << _left_hidden
//	        << ", Right hidden: " << _right_hidden
//	        << ", Bot hidden: " << _bot_hidden
//	        << ", Top hidden: " << _top_hidden << ".\n";
//	 std::cout << "Tile size: " << tile_size() << "\n";
//	 std::cout << "Map width: " << _map->width() << "\n";
//	 std::cout << "Map height: " << _map->height() << "\n";

	std::list<std::string> sound_effects_added;
	Playlist& playlist = Playlist::Instance();

	if (!_show_map && !_show_obj && !_show_act)
		return;

	// TODO: Don't show icons for actions or objects.
	_show_act = false;
	_show_obj = true;

	// First, blank entire screen to avoid overlay graphic errors
	_sdlwindow->clear_texture_arena();

	// Determine which icon needs to be drawn, in case it has an animation sequence defined
	if (!_party_is_moving) {
		for (unsigned i = 0; i < _drawn_icons.size(); i++) {
			if (IndoorsIcons::Instance().get_props(i)->next_anim() < 0)
				_drawn_icons[i] = i;
			else {
				if (_drawn_icons[i] < 0)
					_drawn_icons[i] = i;
				else
					_drawn_icons[i] = IndoorsIcons::Instance().get_props(_drawn_icons[i])->next_anim();
			}
		}
	}

	// TODO: The upper bounds should be -1 in the foor loop?
	for (unsigned x = _left_hidden / tile_size(), x2 = 0; x < _map->width() - _right_hidden / tile_size(); x++, x2++) {
		for (unsigned y = _top_hidden / tile_size(), y2 = 0; y < _map->height() - _bot_hidden / tile_size(); y++, y2++) {

			int tileno = _drawn_icons[_map->get_tile(x, y)];
			int puttile_errno = 0;

			if (tileno < 0) {
				std::cerr << "ERROR: squarearena.cc: Invalid tile number in SquareArena::show_map(): " << tileno << ".\n";
				exit(-1);
			}

			// Check for sound effects to be played for particular icon
			if (IndoorsIcons::Instance().get_props(tileno)->sound_effect().size() > 0) {
				boost::filesystem::path sample_path((std::string)DATADIR);
				sample_path = sample_path / PACKAGE_NAME / "data" / World::Instance().get_name() / "sound" / IndoorsIcons::Instance().get_props(tileno)->sound_effect();
				playlist.add_wav(sample_path.string(), SoundSample::sample_volume);
				sound_effects_added.push_back(sample_path.string());
			}

			int party_x = Party::Instance().x, party_y = Party::Instance().y;

//			int party_x, party_y;
//			map_to_screen(Party::Instance().x, Party::Instance().y, party_x, party_y);
//			screen_to_map(party_x, party_y, party_x, party_y);

			if (_show_map) {
				// See comments in hexarena.cc at same position!  Second line of if-statement basically, to simulate night, torches, etc.
				if (in_los(x, y, party_x, party_y) &&
						  (((x_width == 0 && y_width == 0) ||
						  (x-party_x + x_width / 2 <= (unsigned int)x_width && y-party_y + y_width / 2 <= (unsigned int)y_width)) ||
						  is_illuminated((int)x, (int)y)))
				{
					// Mirrors reflect, if an animate object is in front of it...
					if (IndoorsIcons::Instance().get_props(tileno)->get_name().find("mirror") != std::string::npos) {
						for (int offset = -1; offset <= 1; offset += 2) {
							auto found_obj = _map->objs()->equal_range(std::make_pair(x, y + offset));
							for (auto curr_obj = found_obj.first; curr_obj != found_obj.second; curr_obj++) {
								if (((MapObj)curr_obj->second).get_type() != MAPOBJ_ITEM) {
									tileno += 1; // Reflection icon is mirror icon + 1
									break;
								}
							}
						}

						if (Party::Instance().x == (int)x && abs((int)(Party::Instance().y) - (int)y) == 1)
							tileno += 1;
					}

					if ((puttile_errno = put_tile(x2, y2, IndoorsIcons::Instance().get_sdl_icon(tileno))) != 0)
						std::cerr << "WARNING: squarearena.cc: put_tile() returned " << puttile_errno << " in show_map()." << std::endl;
				}
			}

			if (_show_obj) {
				std::pair<unsigned, unsigned> coords;
				coords.first = x;
				coords.second = y;

				auto found_obj = _map->objs()->equal_range(coords);

				// Now draw the objects
				for (auto curr_obj = found_obj.first; curr_obj != found_obj.second; curr_obj++) {
					int obj_icon_no = ((MapObj)curr_obj->second).get_icon();

					if (in_los(x, y, party_x, party_y)) {
						if (((x_width == 0 && y_width == 0) ||
							 (x-party_x + x_width / 2 <= (unsigned int)x_width && y-party_y + y_width / 2 <= (unsigned int)y_width)) ||
							 is_illuminated((int)x, (int)y))
						{
							if ((puttile_errno = put_tile(x2, y2, IndoorsIcons::Instance().get_sdl_icon(_drawn_icons[obj_icon_no])) != 0))
								std::cerr << "WARNING: squarearena.cc: put_tile() returned " << puttile_errno << " in show_map()." << std::endl;
						}
					}

					// Check for sound effects to be played for particular object??
					if (IndoorsIcons::Instance().get_props(obj_icon_no)->sound_effect().size() > 0) {
						boost::filesystem::path sample_path((std::string)DATADIR);
						sample_path = sample_path / PACKAGE_NAME / "data" / World::Instance().get_name() / "sound" / IndoorsIcons::Instance().get_props(obj_icon_no)->sound_effect();
						playlist.add_wav(sample_path.string(), SoundSample::sample_volume);
						sound_effects_added.push_back(sample_path.string());
					}
				}
			}

			if (_show_act) {
				std::vector<std::shared_ptr<Action>> _acts = _map->get_actions(x, y);
				// TODO: Below, 150 is just some randomly chosen icon...
				if (_acts.size() > 0)
					put_tile(x2, y2, IndoorsIcons::Instance().get_sdl_icon(150));
			}
		}
	}

	_party_is_moving = false;
	blit();

	// Keep only those sound effects playing that were added this time 'round...
	for (auto itr = playlist.begin(); itr != playlist.end(); itr++) {
		if (std::find(sound_effects_added.begin(), sound_effects_added.end(), (*itr)->filename()) == sound_effects_added.end()) {
			playlist.stop_wav((*itr)->filename());
			// std::cout << "Removing from playlist: " << (*itr)->filename() << "\n";
		}
		else
			; // std::cout << "NOT removing from playlist: " << (*itr)->filename() << "\n";
	}
	playlist.clear_stopped();
	// std::cout << "Finished removing from playlist: " << playlist.size() << "\n";
	// std::cout << "Added: " << sound_effects_added.size() << "\n";
}

unsigned SquareArena::tile_size(void) const
{
	return World::Instance().get_indoors_tile_size();
}

Offsets SquareArena::offsets(void)
{
	Offsets new_offsets;
	new_offsets.top = _top_hidden;
	new_offsets.bot = _bot_hidden;
	new_offsets.left = _left_hidden;
	new_offsets.right = _right_hidden;
	return new_offsets;
}

// Returns relative coordinates on the hex arena.

void SquareArena::get_center_coords(int& x, int& y)
{
	int w,h;
	SDL_QueryTexture(_texture, NULL, NULL, &w, &h);

	x = w / 2 / tile_size();
	y = h / 2 / tile_size();
	// std::cout << "Center: " << x << ", " << y << std::endl;
}

std::pair<int, int> SquareArena::show_party(int x, int y)
{
	// Right now, if party has entered an object, e.g., a ship, don't draw
	// the icon, only the object.
//	if (Party::Instance().is_entered()) {
//		std::pair<int, int> new_coords;
//		new_coords.first = x;
//		new_coords.second = y;
//		return new_coords;
//	}

	// Draw party in the middle on default values
	if (x == -1 && y == -1)
		get_center_coords(x, y);

	// TODO: else draw custom position
	// ...

//	if (!_party_is_moving)
//	{
//		switch (_party_anim)
//		{
//		case 40:
//			_party_anim = 191;
//			break;
//		default:
//			_party_anim = 40;
//			break;
//		}
//	}

	// 40 and 191 are the party icon
	// put_tile(x, y, IndoorsIcons::Instance().get_sdl_icon(_party_anim));
	put_tile(x, y, IndoorsIcons::Instance().get_sdl_icon(Party::Instance().get_party_icon()));

	int mx, my;
	screen_to_map(x, y, mx, my);

	std::pair<int, int> new_coords;
	new_coords.first = mx;
	new_coords.second = my;

	return new_coords;
}

bool SquareArena::adjacent(int x1, int y1, int x2, int y2)
{
	return abs(x1 - x2) <= 1 && abs(y1 - y2) <= 1;
}

unsigned SquareArena::max_y_coordinate()
{
	return _map->height();
}

unsigned SquareArena::max_x_coordinate()
{
	return _map->width();
}
