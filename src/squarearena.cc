#include <iostream>
#include <list>
#include <utility>
#include <memory>
#include "squarearena.hh"
#include "world.hh"
#include "map.hh"
#include "party.hh"
#include "indoorsicons.hh"
#include "simplicissimus.hh"
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
  if (iconsize > 0) {
    for (int i = 0; i < iconsize; i++)
      _drawn_icons.push_back(-1);
    std::cout << iconsize << " icons loaded\n";
  }
  else {
    std::cerr << "squarearena.cc: Initialisation error. Icons not yet loaded.\n";
    exit(0);
  }

  // _corner_tile_uneven_offset = 0;
}

SquareArena::~SquareArena(void)
{
	if (_sdl_surf != NULL)
		SDL_FreeSurface(_sdl_surf);

	_drawn_icons.clear();
	// SDL_Quit();
}

void SquareArena::set_SDL_surface(SDL_Surface* s)
{
  _clipped_surf = s;
  _sdl_surf = SDL_CreateRGBSurface(SDL_HWSURFACE | SDL_SRCALPHA,
                                   s->w + 100, s->h + 100,
                                   32, 0, 0, 0, 0);
}

// x and y are screen coordinates in pixels

int SquareArena::put_tile(int x, int y, SDL_Surface* brush)
{
  if (x < 0 || y < 0) {
    std::cerr << "Warning: put_tile has wrong coords." << std::endl;
    return -1;
  }

  if (brush == NULL) {
    std::cerr << "Warning: Brush to paint tile is NULL. " << std::endl;
    return 0;
  }

  SDL_Rect rect = get_tile_coords(x, y);
  return SDL_BlitSurface(brush, NULL, _sdl_surf, &rect);
}

// Gets the absolute coordinates in pixels for a tile on (x, y)

SDL_Rect SquareArena::get_tile_coords(int x, int y) const
{
  if (x < 0 || y < 0)
    std::cerr << "Warning: get_tile_coords has wrong coords." << std::endl;

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
  //   std::cout << "top_hidden: " << _top_hidden << ", "
  //        << "bot_hidden: " << _bot_hidden << ", "
  //        << "left_hidden: " << _left_hidden << ", "
  //        << "right_hidden: " << _right_hidden << "\n";

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

  //   std::cout << "top_hidden: " << _top_hidden << ", "
  //        << "bot_hidden: " << _bot_hidden << ", "
  //        << "left_hidden: " << _left_hidden << ", "
  //        << "right_hidden: " << _right_hidden << "\n";

  return offsets();
}

// Determine map offsets in terms of pixels - not tiles!

Offsets SquareArena::determine_offsets()
{
	std::cerr << "Offsets:\n";
	unsigned screen_width = _sdl_surf->w;
	std::cerr << "sdl_surf->w" << screen_width << "\n";
	unsigned screen_height = _sdl_surf->h;
	std::cerr << "sdl_surf->h" << screen_height << "\n";

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

	std::cout << "top_hidden: " << _top_hidden << ", "
			  << "bot_hidden: " << _bot_hidden << ", "
	          << "left_hidden: " << _left_hidden << ", "
	          << "right_hidden: " << _right_hidden << "\n";

	return offsets();
}

// Returns 0 when tile hex-x-coordinate in the upper left corner is
// even, otherwise 1.

// int SquareArena::corner_tile_uneven_offset(void) const
// {
//   return ((_left_hidden/(tile_size()-10))%2 == 0? 0 : 1);
// }

// Convert the relative screen hex coordinates to the absolute map
// hex coordinates.

void SquareArena::screen_to_map(int sx, int sy, int& mx, int& my)
{
  mx = _left_hidden / (tile_size()) + sx; // - corner_tile_uneven_offset();
  my = _top_hidden / (tile_size()) + sy; // - ((sx%2 == 0)? 1 : 0);
  // std::cerr << "screen_to_map: " << sx << ", " << sy
  //        << " => " << mx << ", " << my << "\n";
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

  // std::cerr << "Party x,y: " << Party::Instance().x << ", "
  //        << Party::Instance().y << "\n";
  // std::cerr << "map_to_screen: " << mx << ", " << my
  //        << " => " << sx << ", " << sy << "\n";
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
	int icon_no = 0;

#ifndef ADD_ICON
#define ADD_ICON(local_x, local_y) {                    			\
	icon_no = get_map()->get_tile(local_x, local_y);    			\
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
					if (!(props->_trans == IT_FULLY)) {
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
					if (!(props->_trans == IT_FULLY)) {
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

	// Now do the actual check for transparency in the row of icons, we just built.
	int semitrans = 0;
	for (unsigned i = 1; i < row.size() - 1; i++) {
		IconProps* props = IndoorsIcons::Instance().get_props(row[i]);

		if (props && (props->_trans == IT_NOT))
			return false;
		else if (i > 0 && props && (props->_trans == IT_SEMI)) {
			// Decrease viewing distance by 4 on semi transparent icons, but
			// not when standing on one (i.e., i > 0), rather only when
			// those icons block the view, i.e., are in front of the player.
			if (row.size() - ++semitrans > 4)
				return false;
		}
	}

	// And here is a row of objects, we filled.  Problem is, that the row of icons can be longer or shorter than the row of icons as there
	// can be 0 to n objects per icon on the map.  So with this naiv scanning of objects, there sometimes could be odd effects in the
	// view of the player.
	if (row_objs.size() > 0) {
		semitrans = 0;

		for (unsigned i = 0; i < row_objs.size(); i++) {
			IconProps* props = IndoorsIcons::Instance().get_props(row_objs[i]);

			if (props && (props->_trans == IT_NOT))
				return false;
			else if (props && (props->_trans == IT_SEMI)) {
				// Decrease viewing distance by 4 on semi transparent icons, but
				// not when standing on one (i.e., i > 0), rather only when
				// those icons block the view, i.e., are in front of the player.
				// TODO: This is OK for icons as in the loop above, but won't work like this for objects.  Bad?
				if (row_objs.size() - ++semitrans > 4)
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
				std::cout << "ILLUMINATED BY OBJ!\n";
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

			if (check_x >= 0 && check_y >= 0 && check_x < _map->width() && check_y < _map->height()) {
				int radius = IndoorsIcons::Instance().get_props(_map->get_tile((unsigned)check_x, (unsigned)check_y))->light_radius();

				if (radius > 0) {
					if (abs(check_x) - x <= radius && abs(check_y) - y <= radius) {
						std::cout << "ILLUMINATED BY ICON!\n";
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
	// std::cout << "Left hidden: " << _left_hidden
	//        << ", Right hidden: " << _right_hidden
	//        << ", Bot hidden: " << _bot_hidden
	//        << ", Top hidden: " << _top_hidden << ".\n";
	// std::cout << "Tile size: " << tile_size() << "\n";
	// std::cout << "Map width: " << _map->width() << "\n";
	// std::cout << "Map height: " << _map->height() << "\n";

	std::list<std::string> sound_effects_added;
	Playlist& playlist = Playlist::Instance();

	if (!_show_map && !_show_obj && !_show_act)
		return;

	// TODO: Don't show icons for actions or objects.
	_show_act = false;
	_show_obj = true;

	// First, blank entire screen to avoid overlay graphic errors
	SDL_FillRect(_sdl_surf, NULL, 0x000000);

	// Determine which icon needs to be drawn, in case it has an
	// animation sequence defined
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
				std::cerr << "Invalid tile number in SquareArena::show_map()." << std::endl;
				exit(0);
			}

			// Check for sound effects to be played for particular icon
			if (IndoorsIcons::Instance().get_props(tileno)->sound_effect().size() > 0) {
				std::string sample_path = (std::string)DATADIR + "/" + (std::string)SAMPLES_PATH + IndoorsIcons::Instance().get_props(tileno)->sound_effect();
				playlist.add_wav(sample_path);
				sound_effects_added.push_back(sample_path);
				// std::cout << "Adding to playlist: " << sample_path << "\n";
			}

			// Check for sound effects to be played for particular object??
			// TODO

			int party_x, party_y;
			map_to_screen(Party::Instance().x, Party::Instance().y, party_x, party_y);
			screen_to_map(party_x, party_y, party_x, party_y);

			if (_show_map) {
				// See comments in hexarena.cc at same position!  Second line of if-statement basically, to simulate night, torches, etc.
				if (in_los(x, y, party_x, party_y) &&
						((x_width == 0 && y_width == 0 || abs(x-party_x + x_width / 2) <= x_width && abs(y-party_y + y_width / 2 <= y_width)) ||
							is_illuminated((int)x, (int)y)))
				{
					if ((puttile_errno = put_tile(x2, y2, IndoorsIcons::Instance().get_sdl_icon(tileno))) != 0)
						std::cerr << "WARNING: put_tile() returned " << puttile_errno << " in SquareArena::show_map()." << std::endl;

				}
			}

			if (_show_obj) {
				std::pair<unsigned, unsigned> coords;
				coords.first = x;
				coords.second = y;

				auto found_obj = _map->objs()->equal_range(coords);

				// Now draw the objects
				for (auto curr_obj = found_obj.first; curr_obj != found_obj.second; curr_obj++) {
					if (in_los(x, y, party_x, party_y)) {
						if ((x_width == 0 && y_width == 0 || abs(x-party_x + x_width / 2) <= x_width && abs(y-party_y + y_width / 2 <= y_width)) || is_illuminated((int)x, (int)y)) {
							int obj_icon_no = ((MapObj)curr_obj->second).get_icon();

							if ((puttile_errno = put_tile(x2, y2, IndoorsIcons::Instance().get_sdl_icon(_drawn_icons[obj_icon_no])) != 0))
								std::cerr << "WARNING: put_tile() returned " << puttile_errno << " in SquareArena::show_map()." << std::endl;
						}
					}
				}
			}

			if (_show_act) {
				std::shared_ptr<Action> _act = _map->get_action(x, y);
				// TODO: Below, 150 is just some randomly chosen icon...
				if (_act != NULL)
					put_tile(x2, y2, IndoorsIcons::Instance().get_sdl_icon(150));
			}
		}
	}

	_party_is_moving = false;

	// Keep only those sound effects playing that were added this time 'round...
	for (auto itr = playlist.begin(); itr != playlist.end(); itr++) {
		if (std::find(sound_effects_added.begin(), sound_effects_added.end(), (*itr)->filename()) == sound_effects_added.end()) {
			playlist.stop_wav((*itr)->filename());
			// std::cout << "Removing from playlist: " << (*itr)->filename() << "\n";
		}
		// else
		//   std::cout << "NOT removing from playlist: " << (*itr)->filename() << "\n";
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
  x = _clipped_surf->w / 2 / tile_size();
  y = _clipped_surf->h / 2 / tile_size();
  // std::cout << "Center: " << x << ", " << y << std::endl;
}

std::pair<int, int> SquareArena::show_party(int x, int y)
{
  // Draw party in the middle on default values
  if (x == -1 && y == -1)
    get_center_coords(x, y);

  // TODO: else draw custom position
  // ...

  if (!_party_is_moving)
    {
      switch (_party_anim)
        {
        case 40:
          _party_anim = 191;
          break;
        default:
          _party_anim = 40;
          break;
        }
    }

  // 40 and 191 are the party icon
  put_tile(x, y, IndoorsIcons::Instance().get_sdl_icon(_party_anim));

  int mx, my;
  screen_to_map(x, y, mx, my);

  // std::cerr << "show_party: " << x << ", " << y
  //        << " => " << mx << ", " << my << "\n";

  std::pair<int, int> new_coords;
  new_coords.first = mx;
  new_coords.second = my;

  return new_coords;
}

void SquareArena::update()
{
  SDL_Rect srcRect;
  srcRect.x = tile_size();
  srcRect.y = tile_size();
  srcRect.w = _sdl_surf->w;
  srcRect.h = _sdl_surf->h;

  SDL_BlitSurface(_sdl_surf, &srcRect, _clipped_surf, NULL);
}

bool SquareArena::adjacent(int x1, int y1, int x2, int y2)
{
  return abs(x1 - x2) <= 1 && abs(y1 - y2) <= 1;
}
