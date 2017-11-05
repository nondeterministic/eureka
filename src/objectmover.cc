#include <vector>
#include <utility>
#include <cmath>

#include "objectmover.hh"
#include "mapobj.hh"
#include "gamecontrol.hh"
#include "iconprops.hh"
#include "indoorsicons.hh"

ObjectMover::ObjectMover()
{
	gc = &GameControl::Instance();
	party = &Party::Instance();
}

/**
 * Move indoors objects according to the individual context (e.g., follow, flee, roam, etc.)
 */

void ObjectMover::move()
{
	if (gc->is_arena_outdoors())
		return;

	std::vector<std::pair<int,int>> moved_objects_coords;

	// First calculate new positions for objects and store them in moved_objects_coords.
	for (auto map_obj_pair = gc->get_arena()->get_map()->objs()->begin(); map_obj_pair != gc->get_arena()->get_map()->objs()->end(); map_obj_pair++) {
		MapObj* map_obj = &(map_obj_pair->second);
		do_actual_moving(map_obj, moved_objects_coords);
	}

	// Now do the actual moving of objects.
	for (std::pair<int,int> coords: moved_objects_coords) {
		for (MapObj* obj: gc->get_arena()->get_map()->get_objs(coords.first, coords.second)) {
			if (obj->is_animate()) {
				MapObj tmpObj = *obj;  // Make a deep copy of the object that is about to be kicked off the map
				gc->get_arena()->get_map()->pop_obj_animate(coords.first, coords.second);
				gc->get_arena()->get_map()->push_obj(tmpObj);
				break; // Assume there is at most one animate object per coordinate; so ignore other objects here
			}
		}
	}
}

/**
 * Instead of a return value, the result is afterwards stored in moved_objects_coords -
 * maybe not so super nice, but ok for now, as method is private and only called once from here.
 */

void ObjectMover::do_actual_moving(MapObj* map_obj, std::vector<std::pair<int,int>>& moved_objects_coords)
{
	int x_off = 0, y_off = 0;
	unsigned obj_x, obj_y;

	map_obj->get_coords(obj_x, obj_y);

	// ROAM around
	if (map_obj->move_mode == ROAM) {
		int move = gc->random(0,16);  // That is, a 50% chance of keeping the same position

		unsigned ox, oy;
		map_obj->get_origin(ox, oy);

		if (move <= 2) {
			x_off = 0; y_off = -1;
		}
		else if (move <= 4) {
			x_off = 0; y_off = 1;
		}
		else if (move <= 6) {
			x_off = -1; y_off = 0;
		}
		else if (move <= 8) {
			x_off = 1; y_off = 0;
		}

		// First check if everything is within bounds...
		if (obj_x > 0 && obj_x < gc->get_map()->width() - 1 && obj_y > 0 && obj_y < gc->get_map()->height() - 1 &&
				std::abs((int)obj_x - (int)ox + (int)x_off) <= 2 && std::abs((int)obj_y - (int)oy + (int)y_off) <= 2 &&
				(obj_x + x_off != party->x || obj_y + y_off != party->y))
		{
			// Now check "walkability properties" of icon in detail...
			if (gc->walkable(obj_x + x_off, obj_y + y_off)) {
				int icon = gc->get_arena()->get_map()->get_tile(obj_x + x_off, obj_y + y_off);
				IconProps* icon_props = IndoorsIcons::Instance().get_props(icon);

				if (icon_props->_poisonous == PropertyStrength::None &&
				    icon_props->_magical_force_field == PropertyStrength::None)
				{
					map_obj->set_coords(obj_x + x_off, obj_y + y_off);
					moved_objects_coords.push_back(std::make_pair(obj_x, obj_y));
				}
			}
		}
	}

	// FOLLOW
	if ((map_obj->personality == HOSTILE && abs((int)obj_x - party->x) < 8 && abs((int)obj_y - party->y) < 8) ||
			map_obj->move_mode == FOLLOWING)
	{
		// Only follow each round with certain probability or the following leaves the player no space to breathe
		if (gc->random(0,100) < 60)
			return;

		PathFinding pf(gc->get_arena()->get_map().get());

		unsigned obj_x, obj_y;
		map_obj->get_coords(obj_x, obj_y);

		std::pair<unsigned,unsigned> new_coords = pf.follow_party(obj_x, obj_y, party->x, party->y);

		if ((obj_x != new_coords.first || obj_y != new_coords.second) &&                       // If coordinates changed...
				((int)new_coords.first != party->x || (int)new_coords.second != party->y))     // If new coordinates aren't those of the party...
		{
			int icon = gc->get_arena()->get_map()->get_tile(new_coords.first, new_coords.second);
			IconProps* icon_props = IndoorsIcons::Instance().get_props(icon);

			// We need to check again for walkability, as other objects may have moved to this position in the same round...
			if (// walkable(new_coords.first, new_coords.second) &&
				icon_props->_magical_force_field == PropertyStrength::None &&
					std::find(moved_objects_coords.begin(),
							moved_objects_coords.end(),
							std::make_pair((int)(obj_x), (int)(obj_y))) == moved_objects_coords.end())
			{
				moved_objects_coords.push_back(std::make_pair(obj_x, obj_y));
				map_obj->set_coords(new_coords.first, new_coords.second);
			}
		}
	}

	// FLEE
	// TODO: Also, like in the FOLLOW-case, add condition to only flee n fields max. distance? Otherwise they all flee to the edge of town...
	if (map_obj->move_mode == FLEE) {
		// Only flee each round with 70% probability or the fleeing leaves the player no chance to ever catch up
		if (gc->random(0,100) < 40)
			return;

		unsigned obj_x, obj_y;
		map_obj->get_coords(obj_x, obj_y);

		// Get list of furthest away fields from party (as the person flees...)
		int longest_dist = abs(party->x - (int)obj_x) + abs(party->y - (int)obj_y);
		int best_x = 0, best_y = 0;
		for (int x = -1; x < 2; x++) {
			for (int y = -1; y < 2; y++) {
				if ((int)obj_x + x > 0 && (int)obj_x + x < (int)gc->get_arena()->get_map()->width() - 3 &&
						(int)obj_y + y > 0 && (int)obj_y + y < (int)gc->get_arena()->get_map()->height() - 3 &&
						gc->walkable((int)obj_x + x, (int)obj_y + y))
				{
					int new_dist = abs(party->x - (int)obj_x - x) + abs(party->y - (int)obj_y - y);
					if (new_dist >= longest_dist) {
						longest_dist = new_dist;
						best_x = x; best_y = y;
					}
				}
			}
		}

		moved_objects_coords.push_back(std::make_pair(obj_x, obj_y));
		map_obj->set_coords((int)obj_x + best_x, (int)obj_y + best_y);
	}
}
