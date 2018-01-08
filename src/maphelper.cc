// This source file is part of eureka
//
// Copyright (c) 2007-2018 Andreas Bauer <baueran@gmail.com>
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

#include "maphelper.hh"
#include "ztatswincontentprovider.hh"
#include "item.hh"
#include "itemfactory.hh"
#include "util.hh"

#include <vector>
#include <string>
#include <utility> // std::pair
#include <iostream>
#include <memory>

using namespace std;

MapHelper::MapHelper(Map* map)
{
	_map = map;
}

std::shared_ptr<ZtatsWinContentSelectionProvider<MapObj>> MapHelper::create_ztatswin_content_selection_provider_for_coords(unsigned x, unsigned y, ItemPickup pickup_mode)
{
	std::shared_ptr<ZtatsWinContentSelectionProvider<MapObj>> content_selection_provider(new ZtatsWinContentSelectionProvider<MapObj>);

	if (x >= _map->width() || y >= _map->height())
		return content_selection_provider;

	std::vector<std::pair<StringAlignmentTuple, MapObj>> content_page;

	// Range of available MapObjects. These are not the actual items!
	auto avail_objects = _map->objs()->equal_range(std::pair<unsigned,unsigned>(x,y));

	for (auto obj_itr = avail_objects.first; obj_itr != avail_objects.second; obj_itr++) {
		MapObj curr_obj = obj_itr->second;

		if ((pickup_mode == ItemPickup::RemovableOnly && curr_obj.is_removeable()) || pickup_mode == ItemPickup::AllItems) {
			// We generate an item according to its lua name in order to get the properties
			// for the list of items to get.  Not sure if this is necessary or if one should
			// simply dissect the lua name.  But this way, we keep the mapping bewteen icons
			// and the actual item inside the respective factory functions, centrally. So it
			// probably is a good idea to do it as it is done now.
			Item* item = NULL;

			try {
				item = ItemFactory::create(curr_obj.lua_name, &curr_obj);
			}
			catch (std::exception const& e) {
				std::cerr << "EXCEPTION: gamecontrol.cc: " << e.what() << "\n";
				std::cerr << "ERROR: gamecontrol.cc: Skipping picking up of an item, due to earlier errors.\n";
				continue; // Might be better to throw here?!  But together with the error message, this is OK behaviour.
			}

			if (curr_obj.how_many > 1) {
				std::string plural_name = item->plural_name() + " (" + std::to_string(curr_obj.how_many) + "x)";
				StringAlignmentTuple mapobj_name = StringAlignmentTuple(plural_name,Alignment::LEFTALIGN);
				content_page.push_back(std::pair<StringAlignmentTuple, MapObj>(mapobj_name, curr_obj));
			}
			else {
				StringAlignmentTuple mapobj_name = StringAlignmentTuple(item->name(),Alignment::LEFTALIGN);
				content_page.push_back(std::pair<StringAlignmentTuple, MapObj>(mapobj_name, curr_obj));
			}

			delete item;
		}
	}

	content_selection_provider->add_content_page(content_page);
	return content_selection_provider;
}
