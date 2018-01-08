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

#include <boost/unordered_map.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <libxml++/libxml++.h>

#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>
#include <utility>
#include <memory>
#include <vector>

#include "map.hh"
#include "world.hh"
#include "mapobj.hh"
#include "gameevent.hh"
#include "eventermap.hh"
#include "eventleavemap.hh"
#include "eventchangeicon.hh"
#include "eventprintcon.hh"
#include "eventplaysound.hh"
#include "eventluascript.hh"
#include "eventdeleteobj.hh"
#include "eventaddobj.hh"
#include "action.hh"
#include "actiontake.hh"
#include "actiononenter.hh"
#include "actionpullpush.hh"
#include "actionopened.hh"
#include "actonlook.hh"
#include "indoorsicons.hh"
#include "soundsample.hh"

Map::Map()
{
	is_dungeon = false;
	guarded_city = false;
	_modified = true;
	initial = false;
	initial_x = -1;
	initial_y = -1;
	_longname = "";
	// _main_map_xml_root = NULL;
	// _main_map_xml_file = NULL;
}

Map::Map(const Map& p)
{
	is_dungeon = p.is_dungeon;
	guarded_city = p.guarded_city;
	_name = p._name;
	_longname = p._longname;
	_modified = p._modified;
	_data = p._data;
	_map_objects = p._map_objects;
	_actions = p._actions;
	initial = p.initial;
	initial_x = p.initial_x;
	initial_y = p.initial_y;
	// std::cout << "DEEP MAP COPY MOTHERFUCKER\n";
}

Map::~Map()
{
	// std::cerr << "WARNING: map.cc::~Map(): Also destroyed all objects " << _name << " contained!" << std::endl;
	unload_map_data();

	// Memory was new'ed in EditorWin::add_event().
	//	for (std::vector<Action*>::iterator curr_act = _actions.begin(); curr_act != _actions.end(); curr_act++)
	//		delete *curr_act;

	_actions.clear();
}

void Map::unload_map_data(void)
{
	if (_data.size() > 0)
		for (unsigned i = 0; i < _data.size(); i++)
			_data[i].clear();
	_data.clear();
	_longname = "";
	_map_objects.clear();
	_modified = false;
}

unsigned Map::height(void) const
{
	return _data.size();
}

unsigned Map::width(void) const
{
	return (_data.size()? _data[0].size() : 0);
}

bool Map::modified(void) const
{
	return _modified;
}

void Map::set_notmodified(bool notmod)
{
	_modified = !notmod;
}

std::string Map::get_name(void)
{
	return _name;
}

void Map::set_name(const char* new_name)
{
	_name = new_name;
	_modified = true;
}

std::string Map::get_longname(void)
{
	return _longname;
}

void Map::set_longname(const char* new_name)
{
	_longname = new_name;
	_modified = true;
}

std::vector<MapObj*> Map::get_objs(std::pair<unsigned, unsigned> coords)
{
	return get_objs(coords.first, coords.second);
}

std::vector<MapObj*> Map::get_objs(unsigned x, unsigned y)
{
	std::pair<unsigned, unsigned> coords(x, y);
	auto found_objs = _map_objects.equal_range(coords);
	std::vector<MapObj*> results;

	for (auto curr_obj = found_objs.first; curr_obj != found_objs.second; curr_obj++)
		results.push_back(&(curr_obj->second));

	return results;
}

std::vector<std::shared_ptr<Action>> Map::get_actions(unsigned x, unsigned y)
{
	std::vector<std::shared_ptr<Action>> return_actions;

	for (auto curr_act: _actions) {
		if (curr_act->get_x() == x && curr_act->get_y() == y)
			return_actions.push_back(curr_act);
	}

	return return_actions;
}

void Map::add_event_to_action(unsigned x, unsigned y, std::shared_ptr<GameEvent> ev)
{
	for (auto curr_act: _actions) {
		if (curr_act->get_x() == x && curr_act->get_y() == y) {
			curr_act->add_event(ev);
			return;
		}
	}
}

std::pair<int,int> Map::get_initial_coords()
{
	if (initial_x < 0 || initial_y < 0) {
		throw NoInitialCoordsException("This map contains no initial coordinates.");
	}
	else
		return std::make_pair(initial_x, initial_y);
}

void Map::add_action(std::shared_ptr<Action> new_act)
{
	// Only one action per icon is allowed!  Theoretically more could be done but then it's difficult in the editor
	// and I cannot think of many examples, where multiple actions on a single icon are needed.
	for (auto curr_act = _actions.begin(); curr_act != _actions.end(); curr_act++)
		if ((*curr_act)->get_x() == new_act->get_x() && (*curr_act)->get_y() == new_act->get_y())
			return;

	try {
		_actions.push_back(std::shared_ptr<Action>(new_act));
		_modified = true;
	}
	catch(const std::exception& ex) {
		std::cout << "Exception upon adding action: " << ex.what() << std::endl;
	}
}

void Map::del_action(unsigned x, unsigned y)
{
	for (auto curr_act = _actions.begin(); curr_act != _actions.end(); curr_act++) {
		if ((*curr_act)->get_x() == x && (*curr_act)->get_y() == y) {

			// TODO: Is the delete before erase necessary?  I believe
			// so, since we new'ed the memory prior.  And a simple clear
			// would not do, so why would an erase?
			// delete (*curr_act);

			_actions.erase(curr_act);
			_modified = true;
			return;
		}
	}
}

unsigned Map::how_many_mapobj_at(unsigned x, unsigned y)
{
	std::pair<boost::unordered_multimap<std::pair<unsigned, unsigned>, MapObj>::iterator,
	 	 boost::unordered_multimap<std::pair<unsigned, unsigned>, MapObj>::iterator>
			found_objs = _map_objects.equal_range(std::pair<unsigned,unsigned>(x,y));

	unsigned i = 0;
	for (auto curr_obj = found_objs.first; curr_obj != found_objs.second; curr_obj++, i++);
	return i;
}

void Map::rm_obj(MapObj* map_obj)
{
	if (map_obj->lua_name.length() == 0 && map_obj->id.length() == 0) {
		std::cerr << "ERROR: map.cc: Cannot pop MapObj without Lua-name and ID in pop_ob(MapObj*). "
				  << "Perhaps use pop(x,y) instead, but then make sure, there is EXACTLY one MapObj on the coordinates x, y!\n";
		return;
	}

	unsigned x, y;
	map_obj->get_coords(x, y);

	std::pair<boost::unordered_multimap<std::pair<unsigned, unsigned>, MapObj>::iterator,
		boost::unordered_multimap<std::pair<unsigned, unsigned>, MapObj>::iterator>
			found_objs = _map_objects.equal_range(std::pair<unsigned, unsigned>(x,y));

	for (auto ptr = found_objs.first; ptr != found_objs.second; ptr++) {
		MapObj& curr_obj = ptr->second;
//		if (ptr->lua_name == map_obj->lua_name &&
//				curr_obj->id == map_obj->id &&
//				curr_obj->description() == map_obj->description())
		if (curr_obj == *map_obj) {
			_map_objects.erase(ptr);
			_modified = true;
			return;
		}
	}

	std::cout << "INFO: map.cc: Tried to pop MapObj, but nothing was popped / no items found in location.\n";
}

/// See comment inside map.hh!

void Map::pop_obj(unsigned x, unsigned y)
{
	unsigned how_many_objs = how_many_mapobj_at(x,y);
	if (how_many_objs > 1) {
		std::cerr << "ERROR: map.cc: Cannot use pop(x,y), if there are more than one MapObj in said location. "
				  << "Use pop(x,y,lua_name) instead!\n";
		return;
	}
	else if (how_many_objs == 0)
		return;

	std::pair<unsigned, unsigned> coords(x, y);
	std::pair<boost::unordered_multimap<std::pair<unsigned, unsigned>, MapObj>::iterator,
	 	 boost::unordered_multimap<std::pair<unsigned, unsigned>, MapObj>::iterator>
			found_objs = _map_objects.equal_range(coords);

	for (auto curr_obj = found_objs.first, next_obj = found_objs.first;
			curr_obj != found_objs.second;
			curr_obj++)
	{
		// Delete only last element
		if (++next_obj == found_objs.second) {
			_map_objects.erase(curr_obj);
			_modified = true;
			return;
		}
	}

	std::cerr << "WARNING: map.cc: pop_obj(x,y) failed for some reason.\n";
}

/// As there can be at most one animate object (animal, monster, person) in one location at a time, this will pop at most one object
/// at the given map coordinates.

void Map::pop_obj_animate(unsigned x, unsigned y)
{
	std::pair<unsigned, unsigned> coords(x, y);
	std::pair<boost::unordered_multimap<std::pair<unsigned, unsigned>, MapObj>::iterator,
	 	 boost::unordered_multimap<std::pair<unsigned, unsigned>, MapObj>::iterator>
			found_objs = _map_objects.equal_range(coords);

	for (auto curr_obj = found_objs.first, next_obj = found_objs.first;
			curr_obj != found_objs.second;
			curr_obj++)
	{
		// Delete only last element
		if (++next_obj == found_objs.second && curr_obj->second.is_animate()) {
			_map_objects.erase(curr_obj);
			_modified = true;
			return;
		}
	}

	std::cerr << "WARNING: map.cc: pop_obj_animate(" << x << ", " << y << ") failed. No animate object on location?\n";
}

void Map::pop_obj_animate(std::pair<unsigned, unsigned> coords)
{
	pop_obj_animate(coords.first, coords.second);
}

// Returns the number of deleted MapObj.

int Map::rm_obj_by_id(std::string pop_id)
{
	int deleted = 0;
	bool done = false;

	while (!done) {
		done = true;

		for (auto curr_obj = _map_objects.begin(); curr_obj != _map_objects.end(); curr_obj++) {
			MapObj& mo = (curr_obj->second);
			if (mo.id == pop_id) {
				_map_objects.erase(curr_obj);
				deleted++;
				_modified = true;
				done = false;
				break;
			}
		}
	}

	return deleted;
}

void Map::push_obj(MapObj obj)
{
	std::pair<unsigned, unsigned> coords;
	obj.get_coords(coords.first, coords.second);
	obj.set_layer(_map_objects.count(coords));

	_map_objects.insert(std::make_pair(coords, obj));
	_modified = true;
}

//// Returns the number of deleted MapObj.
//
//int Map::rm_obj(MapObj delmo)
//{
//	int deleted = 0;
//	bool done = false;
//
//	while (!done) {
//		done = true;
//
//		for (auto curr_obj = _map_objects.begin(); curr_obj != _map_objects.end(); curr_obj++) {
//			MapObj& mo = (curr_obj->second);
//			if (mo == delmo) {
//				_map_objects.erase(curr_obj);
//				deleted++;
//				_modified = true;
//				done = false;
//				break;
//			}
//		}
//	}
//
//	return deleted;
//}

boost::unordered_multimap<std::pair<unsigned, unsigned>, MapObj>* Map::objs(void)
{
	return &_map_objects;
}

// This call is somewhat expensive with disk access, but it's more
// reliable to actually check for the file rather than handling a
// flag.

bool Map::exists_on_disk(void)
{
	return boost::filesystem::exists(World::Instance().get_path() /	World::Instance().get_name() / "maps" / (_name + ".xml"));
}

MapObj Map::return_object_node(const xmlpp::Element* objElement)
{
	MapObj new_obj;
	int x = 0, y = 0, ox = 0, oy = 0;
	const xmlpp::Element::AttributeList& attributes = objElement->get_attributes();

	// Could be empty, if object doesn't have an actions tag
	xmlpp::Node::NodeList actions_node = objElement->get_children("actions");

	// Could be empty, if object doesn't have a description tag
	xmlpp::Node::NodeList description_node = objElement->get_children("description");

	// Iterate through attributes of object node
	for (auto iter = attributes.begin(); iter != attributes.end(); ++iter) {
		const xmlpp::Attribute* attribute = *iter;
		std::string a_name = attribute->get_name();
		if (a_name == "x")
			x = atoi(attribute->get_value().c_str());
		else if (a_name == "y")
			y = atoi(attribute->get_value().c_str());
		else if (a_name == "ox")
			ox = atoi(attribute->get_value().c_str());
		else if (a_name == "oy")
			oy = atoi(attribute->get_value().c_str());
		else if (a_name == "icon_no")
			new_obj.set_icon(atoi(attribute->get_value().c_str()));
		else if (a_name == "layer")
			new_obj.set_layer(atoi(attribute->get_value().c_str()));
		else if (a_name == "init_script")
			new_obj.set_init_script_path(attribute->get_value().c_str());
		else if (a_name == "combat_script")
			new_obj.set_combat_script_path(attribute->get_value().c_str());
		else if (a_name == "id")
			new_obj.id = attribute->get_value().c_str();
		else if (a_name == "random_monster")
			new_obj.is_random_monster = attribute->get_value().uppercase() == "YES";
		else if (a_name == "locked") {
			new_obj.openable = true;

			if (attribute->get_value().uppercase() == "NORMAL")
				new_obj.lock_type = NORMAL_LOCK;
			else if (attribute->get_value().uppercase() == "MAGIC")
				new_obj.lock_type = MAGIC_LOCK;
			else // if (attribute->get_value().uppercase() == "UNLOCKED")
				new_obj.lock_type = UNLOCKED;
		}
		else if (a_name == "personality") {
			if (attribute->get_value().uppercase() == "HOSTILE")
				new_obj.personality = HOSTILE;
			else if (attribute->get_value().uppercase() == "RIGHTEOUS")
				new_obj.personality = RIGHTEOUS;
		}
		else if (a_name == "move_mode") {
			if (attribute->get_value().uppercase() == "FLEE")
				new_obj.move_mode = FLEE;
			else if (attribute->get_value().uppercase() == "FOLLOWING")
				new_obj.move_mode = FOLLOWING;
			else if (attribute->get_value().uppercase() == "FOLLOW")
				new_obj.move_mode = FOLLOWING;
			else if (attribute->get_value().uppercase() == "ROAM")
				new_obj.move_mode = ROAM;
		}
		else if (a_name == "type") {
			if (attribute->get_value().uppercase() == "ITEM")
				new_obj.set_type(MAPOBJ_ITEM);
			else if (attribute->get_value().uppercase() == "MONSTER")
				new_obj.set_type(MAPOBJ_MONSTER);
			else if (attribute->get_value().uppercase() == "PERSON")
				new_obj.set_type(MAPOBJ_PERSON);
			else
				new_obj.set_type(MAPOBJ_ANIMAL);
		}
		else if (a_name == "lua_name")
			new_obj.lua_name = attribute->get_value().c_str();
		else if (a_name == "how_many")
			new_obj.how_many = atoi(attribute->get_value().c_str());
	}

	// If no explicit lua_name was set, see if there is a default_lua_name in the world file and set it, if there is.
	if (new_obj.lua_name.length() == 0 && IndoorsIcons::Instance().get_props(new_obj.get_icon())->default_lua_name().length() > 0)
		new_obj.lua_name = IndoorsIcons::Instance().get_props(new_obj.get_icon())->default_lua_name();

	// Parse actions, if there are any associated to the object
	if (actions_node.size() > 0) {
		std::vector<std::shared_ptr<Action>> actions = parse_actions_node(actions_node.front());

		for (auto action: actions)
			new_obj.add_action(action);
	}

	// Parse description if there was one (see code at beginning of function).
	if (description_node.size() > 0) {
		const xmlpp::Element* nodeElement = dynamic_cast<const xmlpp::Element*>(description_node.front());
		if (nodeElement) {
			new_obj.set_description(nodeElement->get_child_text()->get_content());
			// std::cout << "DESCRIPTION READ: " << new_obj.description() << "\n";
		}
		else
			std::cerr << "WARNING: map.cc: Read empty object-description (<description>) tag in map '" << _name << "'.\n";
	}

	new_obj.set_coords(x, y);

	if (ox != 0 || oy != 0)
		new_obj.set_origin(ox, oy);

	if (new_obj.get_type() != MAPOBJ_ITEM && (ox != 0 || oy != 0))
		new_obj.set_coords(ox, oy);

	return new_obj;
}

void Map::parse_objects_node(const xmlpp::Node* node)
{
	xmlpp::Node::NodeList list = node->get_children();
	for (xmlpp::Node::NodeList::iterator iter = list.begin(); iter != list.end(); ++iter) {
		const xmlpp::Element* objectElement = dynamic_cast<const xmlpp::Element*>(*iter);
		if (objectElement) {
			MapObj new_obj = return_object_node(objectElement);
			push_obj(new_obj);
		}
		// else std::cout << "INFO: parse_objects_node: Tried to parse object node '" + node->get_name() + "', but got an objects node instead?!\n";
	}
}

std::vector<std::shared_ptr<Action>> Map::parse_actions_node(const xmlpp::Node* node)
{
	std::vector<std::shared_ptr<Action>> parsed_actions;

	// Get all action nodes
	xmlpp::Node::NodeList actions = node->get_children();
	for (auto action = actions.begin(); action != actions.end(); ++action) {
		const xmlpp::Element* nodeElement = dynamic_cast<const xmlpp::Element*>(*action);

		if (nodeElement) {
			std::string curr_act_name = nodeElement->get_attribute_value("type");  // Name of action
			std::shared_ptr<Action> _act;                                   // Actual action

			// Determine which action was parsed, if any
			if (curr_act_name == "ACT_ON_ENTER")
				_act = std::make_shared<ActionOnEnter>(atoi(nodeElement->get_attribute_value("x").c_str()), atoi(nodeElement->get_attribute_value("y").c_str()), "ACT_ON_ENTER");
			else if (curr_act_name == "ACT_ON_PULLPUSH") {
				std::string xs = nodeElement->get_attribute_value("x");
				std::string ys = nodeElement->get_attribute_value("y");

				if (xs.length() > 0 && ys.length() > 0)
					_act = std::make_shared<ActionPullPush>(atoi(xs.c_str()), atoi(ys.c_str()), "ACT_ON_PULLPUSH");
				else
					_act = std::make_shared<ActionPullPush>("ACT_ON_PULLPUSH");

				//_act = std::make_shared<ActionPullPush>(atoi(nodeElement->get_attribute_value("x").c_str()), atoi(nodeElement->get_attribute_value("y").c_str()), "ACT_ON_PULLPUSH");
			}
			else if (curr_act_name == "ACT_ON_OPENED")
				_act = std::make_shared<ActionOpened>("ACT_ON_OPENED"); // Can only be associated with an object
			else if (curr_act_name == "ACT_ON_TAKE")
				_act = std::make_shared<ActionOnTake>("ACT_ON_TAKE");   // Can only be associated with an object
			else if (curr_act_name == "ACT_ON_LOOK") {
				std::string xs = nodeElement->get_attribute_value("x");
				std::string ys = nodeElement->get_attribute_value("y");

				if (xs.length() > 0 && ys.length() > 0)
					_act = std::make_shared<ActionOnLook>(atoi(xs.c_str()), atoi(ys.c_str()), "ACT_ON_LOOK");
				else
					_act = std::make_shared<ActionOnLook>("ACT_ON_LOOK");
			}
			else {
				std::cerr << "WARNING: map.cc: Unrecognised element inside <actions> node.\n";
				continue;
			}

			// Get all the event nodes for an action
			xmlpp::Node::NodeList events = (*action)->get_children();
			for (auto event = events.begin(); event != events.end(); ++event) {
				const xmlpp::Element* eventElement = dynamic_cast<const xmlpp::Element*>(*event);

				if (eventElement) {
					std::string event_type_s = eventElement->get_attribute_value("type");

					if (event_type_s == "EVENT_ENTER_MAP") {
						// Check if all the required properties of the tag are there.
						const xmlpp::Element* event_x = (xmlpp::Element*)(*event)->get_children("x").front();
						const xmlpp::Element* event_y = (xmlpp::Element*)(*event)->get_children("y").front();
						const xmlpp::Element* event_target_map = (xmlpp::Element*)(*event)->get_children("target_map").front();

						// If they are, add the event.
						if (event_x && event_y && event_target_map) {
							std::shared_ptr<EventEnterMap> new_ev(new EventEnterMap());

							new_ev->set_x((unsigned)atoi(event_x->get_child_text()->get_content().c_str()));
							new_ev->set_y((unsigned)atoi(event_y->get_child_text()->get_content().c_str()));
							new_ev->set_map_name(event_target_map->get_child_text()->get_content().c_str());

							_act->add_event(new_ev);
						}
						else
							std::cerr << "ERROR: map.cc: XML load error: EVENT_ENTER_MAP malformed?" << std::endl;
					}
					else if (event_type_s == "EVENT_LEAVE_MAP") {
						std::shared_ptr<EventLeaveMap> new_ev(new EventLeaveMap());
						_act->add_event(new_ev);
					}
					else if (event_type_s == "EVENT_DELETE_OBJECT") {
						std::shared_ptr<EventDeleteObject> new_ev(new EventDeleteObject());
						_act->add_event(new_ev);
					}
					else if (event_type_s == "EVENT_ADD_OBJECT") {
						xmlpp::Element* objElement = (xmlpp::Element*)(*event)->get_children("object").front();

						if (objElement != NULL) {
							MapObj addedObject = return_object_node(objElement);
							std::shared_ptr<EventAddObject> new_ev(new EventAddObject(addedObject));
							_act->add_event(new_ev);
						}
						else
							std::cerr << "ERROR: map.cc: Tried to add an object to event, but no object found. Is map the XML-file OK?\n";
					}
					else if (event_type_s == "EVENT_PLAY_MUSIC" || event_type_s == "EVENT_PLAY_SOUND") {
						int loop = event_type_s == "EVENT_PLAY_MUSIC"? -1 : 0; // Songs loop forever (-1), samples once by default (0)!
						int volume = event_type_s == "EVENT_PLAY_MUSIC"? SoundSample::music_volume : SoundSample::sample_volume;

						std::string loop_string = nodeElement->get_attribute_value("loop");
						if (loop_string.length() > 0)
							loop = atoi(loop_string.c_str());

						std::string vol_string = nodeElement->get_attribute_value("volume");
						if (vol_string.length() > 0)
							volume = atoi(vol_string.c_str());

						std::string filename = eventElement->get_child_text()->get_content().c_str();
						boost::algorithm::trim(filename);
						if (event_type_s == "EVENT_PLAY_SOUND") {
							std::shared_ptr<EventPlaySound> new_ev(new EventPlaySound(filename, loop, volume));
							_act->add_event(new_ev);
						}
						else {
							std::shared_ptr<EventPlayMusic> new_ev(new EventPlayMusic(filename, loop, volume));
							_act->add_event(new_ev);
						}
					}
					else if (event_type_s == "EVENT_CHANGE_ICON") {
						const xmlpp::Element* event_change_icon = (xmlpp::Element*)(*event)->get_children("change_icon").front();

						if (event_change_icon) {
							std::shared_ptr<EventChangeIcon> new_ev(new EventChangeIcon());

							new_ev->x = std::stoi(event_change_icon->get_attribute_value("x").c_str());
							new_ev->y = std::stoi(event_change_icon->get_attribute_value("y").c_str());
							new_ev->icon_now = std::stoi(event_change_icon->get_attribute_value("icon_now").c_str());
							new_ev->icon_new = std::stoi(event_change_icon->get_attribute_value("icon_new").c_str());

							_act->add_event(new_ev);
						}
						else
							std::cerr << "ERROR: map.cc: XML load error: EVENT_CHANGE_ICON malformed?" << std::endl;
					}
					else if (event_type_s == "EVENT_PRINTCON") {
						std::shared_ptr<EventPrintcon> new_ev(new EventPrintcon());
						new_ev->text = eventElement->get_child_text()->get_content().c_str();
						boost::algorithm::trim(new_ev->text);
						_act->add_event(new_ev);
					}
					else if (event_type_s == "EVENT_LUA_SCRIPT") {
						std::shared_ptr<EventLuaScript> new_ev(new EventLuaScript());
						new_ev->file_name = eventElement->get_child_text()->get_content().c_str();
						boost::algorithm::trim(new_ev->file_name);
						_act->add_event(new_ev);
					}
					else
						std::cerr << "ERROR: map.cc: XML load error: unsupported event type in map file: " << event_type_s << std::endl;
				}
			}

			parsed_actions.push_back(_act);
		}
	}

	return parsed_actions;
}

void Map::parse_data_node(const xmlpp::Node* node)
{
	xmlpp::Node::NodeList list = node->get_children();
	for (xmlpp::Node::NodeList::iterator iter = list.begin();
			iter != list.end(); ++iter) {
		const xmlpp::Element* nodeElement =
				dynamic_cast<const xmlpp::Element*>(*iter);

		if (nodeElement) {
			const xmlpp::TextNode* row_data = nodeElement->get_child_text();
			std::vector<std::string> row_string_values;
			std::string row_string = row_data->get_content();
			boost::algorithm::split(row_string_values,
					row_string,
					boost::algorithm::is_any_of(" "));
			// Convert the icon number strings into unsigned ints
			std::vector<unsigned> row_int_values;
			for (std::vector<std::string>::iterator
					curr_string_val = row_string_values.begin();
					curr_string_val != row_string_values.end();
					curr_string_val++)
				row_int_values.push_back
				(boost::lexical_cast<unsigned>(*curr_string_val));
			// Add the row to _data
			_data.push_back(row_int_values);
		}
	}
}

void Map::parse_node(const xmlpp::Node* node)
{
	const xmlpp::ContentNode* nodeContent = dynamic_cast<const xmlpp::ContentNode*>(node);
	const xmlpp::Element*     nodeElement = dynamic_cast<const xmlpp::Element*>(node);

	if (nodeElement) {
		if (nodeElement->get_name().uppercase() == "NAME")
			_name = nodeElement->get_child_text()->get_content();
		else if (nodeElement->get_name().uppercase() == "LONGNAME")
			_longname = nodeElement->get_child_text()->get_content();
		else if (nodeElement->get_name().uppercase() == "DUNGEON")
			is_dungeon = nodeElement->get_child_text()->get_content().uppercase() == "TRUE"? true : false;
		else if (nodeElement->get_name().uppercase() == "GUARDED_CITY")
			guarded_city = nodeElement->get_child_text()->get_content().uppercase() == "TRUE"? true : false;
		else if (nodeElement->get_name().uppercase() == "INITIAL_X")
			initial_x = std::stoi(nodeElement->get_child_text()->get_content().c_str());
		else if (nodeElement->get_name().uppercase() == "INITIAL_Y")
			initial_y = std::stoi(nodeElement->get_child_text()->get_content().c_str());
		else if (nodeElement->get_name().uppercase() == "OBJECTS")
			parse_objects_node(node);
		else if (nodeElement->get_name().uppercase() == "DATA")
			parse_data_node(node);
		else if (nodeElement->get_name().uppercase() == "ACTIONS") {
			std::vector<std::shared_ptr<Action>> actions = parse_actions_node(node);
			for (auto action : actions)
				add_action(action);
		}
	}

	if (!nodeContent) {
		xmlpp::Node::NodeList list = node->get_children();
		for (xmlpp::Node::NodeList::iterator iter = list.begin();
				iter != list.end(); ++iter)
			parse_node(*iter);
	}
}

bool Map::xml_load_map_data()
{
	boost::filesystem::path empty_path;
	return xml_load_map_data(empty_path);
}

// A DOM parser that reads map data from disk

bool Map::xml_load_map_data(boost::filesystem::path pathToFile)
{
	boost::filesystem::path filepath;

	if (pathToFile.empty())
		filepath = World::Instance().get_path() / World::Instance().get_name() / "maps" / (_name + ".xml");
	else
		filepath = pathToFile;

	try {
		xmlpp::DomParser parser;
		// We just want the text to be resolved/unescaped automatically.
		// TODO: do we?
		parser.set_substitute_entities();
		parser.parse_file(filepath.c_str());
		if (parser) {
			// Deleted by DomParser:
			const xmlpp::Node* pNode = parser.get_document()->get_root_node();
			parse_node(pNode);
		}
	}
	catch(const std::exception& ex) {
		std::cerr << "ERROR: map.cc: Exception upon loading map: " << ex.what() << std::endl;
		return false;
	}

	// Something we just read cannot be already modified by definition.
	_modified = false;
	return true;
}

// Note to self: Element inherits from Node.

void Map::write_action_node(xmlpp::Element* node, Action* action)
{
	std::stringstream s_x;
	s_x << action->get_x();
	std::stringstream s_y;
	s_y << action->get_y();

	// Create action node
	if (dynamic_cast<ActionOnEnter*>(action)) {
		node->set_attribute("type", "ACT_ON_ENTER");
		node->set_attribute("x", s_x.str());
		node->set_attribute("y", s_y.str());
	}
	else if (dynamic_cast<ActionPullPush*>(action)) {
		ActionPullPush* act = dynamic_cast<ActionPullPush*>(action);
		node->set_attribute("type", "ACT_ON_PULLPUSH");
		if (act->get_x() > 0 && act->get_y() > 0) {
			node->set_attribute("x", s_x.str());
			node->set_attribute("y", s_y.str());
		}
	}
	else if (dynamic_cast<ActionOnLook*>(action)) {
		ActionOnLook* act = dynamic_cast<ActionOnLook*>(action);
		node->set_attribute("type", "ACT_ON_LOOK");
		if (act->get_x() > 0 && act->get_y() > 0) {
			node->set_attribute("x", s_x.str());
			node->set_attribute("y", s_y.str());
		}
	}
	else if (dynamic_cast<ActionOpened*>(action)) {
		node->set_attribute("type", "ACT_ON_OPENED");
	}
	else if (dynamic_cast<ActionOnTake*>(action)) {
		node->set_attribute("type", "ACT_ON_TAKE");
	}
	else
		node->set_attribute("type", "Check Map::write_action_node() for error!");

	// Add event nodes if there are any
	for (auto curr_ev = action->events_begin(); curr_ev != action->events_end(); curr_ev++) {
		xmlpp::Element* ev_node = node->add_child("event");

		if (std::dynamic_pointer_cast<EventEnterMap>(*curr_ev)) {
			std::shared_ptr<EventEnterMap> event_enter_map = std::dynamic_pointer_cast<EventEnterMap>(*curr_ev);

			ev_node->set_attribute("type", "EVENT_ENTER_MAP");
			ev_node->add_child("x")->add_child_text(std::to_string(event_enter_map->get_x()));
			ev_node->add_child("y")->add_child_text(std::to_string(event_enter_map->get_y()));
			ev_node->add_child("target_map")->add_child_text(event_enter_map->get_map_name());
		}
		else if (std::dynamic_pointer_cast<EventLeaveMap>(*curr_ev)) {
			ev_node->set_attribute("type", "EVENT_LEAVE_MAP");
		}
		else if (std::dynamic_pointer_cast<EventChangeIcon>(*curr_ev)) {
			std::shared_ptr<EventChangeIcon> event_change_icon = std::dynamic_pointer_cast<EventChangeIcon>(*curr_ev);
			ev_node->set_attribute("type", "EVENT_CHANGE_ICON");

			xmlpp::Element* change_icon_el = ev_node->add_child("change_icon");
			change_icon_el->set_attribute("x", std::to_string(event_change_icon->x));
			change_icon_el->set_attribute("y", std::to_string(event_change_icon->y));
			change_icon_el->set_attribute("icon_now", std::to_string(event_change_icon->icon_now));
			change_icon_el->set_attribute("icon_new", std::to_string(event_change_icon->icon_new));
		}
		else if (std::dynamic_pointer_cast<EventDeleteObject>(*curr_ev)) {
			ev_node->set_attribute("type", "EVENT_DELETE_OBJECT");
		}
		else if (std::dynamic_pointer_cast<EventAddObject>(*curr_ev)) {
			std::shared_ptr<EventAddObject> ev_obj = std::dynamic_pointer_cast<EventAddObject>(*curr_ev);
			ev_node->set_attribute("type", "EVENT_ADD_OBJECT");
			xmlpp::Element* object_node = ev_node->add_child("object");
			MapObj mapObj = ev_obj->get_obj();
			write_obj_xml_node(mapObj, object_node);
		}
		else if (std::dynamic_pointer_cast<EventPrintcon>(*curr_ev)) {
			std::shared_ptr<EventPrintcon> event_printcon = std::dynamic_pointer_cast<EventPrintcon>(*curr_ev);
			ev_node->set_attribute("type", "EVENT_PRINTCON");
			ev_node->set_child_text(event_printcon->text);
		}
		else if (std::dynamic_pointer_cast<EventPlaySound>(*curr_ev)) {
			std::shared_ptr<EventPlaySound> event_playsound = std::dynamic_pointer_cast<EventPlaySound>(*curr_ev);
			ev_node->set_attribute("type", "EVENT_PLAY_SOUND");
			ev_node->set_child_text(event_playsound->filename);

			if (event_playsound->loop != 0)
				ev_node->set_attribute("loop", std::to_string(event_playsound->loop));
			if (event_playsound->volume != SoundSample::sample_volume)
				ev_node->set_attribute("loop", std::to_string(event_playsound->volume));
		}
		else if (std::dynamic_pointer_cast<EventPlayMusic>(*curr_ev)) {
			std::shared_ptr<EventPlayMusic> event_playmusic = std::dynamic_pointer_cast<EventPlayMusic>(*curr_ev);
			ev_node->set_attribute("type", "EVENT_PLAY_MUSIC");
			ev_node->set_child_text(event_playmusic->filename);

			if (event_playmusic->loop != -1)
				ev_node->set_attribute("loop", std::to_string(event_playmusic->loop));
			if (event_playmusic->volume != SoundSample::music_volume)
				ev_node->set_attribute("loop", std::to_string(event_playmusic->volume));
}
		else if (std::dynamic_pointer_cast<EventLuaScript>(*curr_ev)) {
			std::shared_ptr<EventLuaScript> event_lua_script = std::dynamic_pointer_cast<EventLuaScript>(*curr_ev);
			ev_node->set_attribute("type", "EVENT_LUA_SCRIPT");
			ev_node->set_child_text(event_lua_script->file_name);
		}
		else if (std::dynamic_pointer_cast<EventDeleteObject>(*curr_ev)) {
			std::shared_ptr<EventDeleteObject> event_printcon= std::dynamic_pointer_cast<EventDeleteObject>(*curr_ev);
			ev_node->set_attribute("type", "EVENT_DELETE_OBJECT");
		}
	}
}

bool Map::xml_write_map_data()
{
	boost::filesystem::path empty_path;
	return xml_write_map_data(empty_path);
}

bool Map::write_obj_xml_node(MapObj mapObj, xmlpp::Element* object_node)
{
	unsigned x, y;
	unsigned ox, oy;

	mapObj.get_coords(x, y);
	mapObj.get_origin(ox, oy);

	object_node->set_attribute("x", std::to_string(x));
	object_node->set_attribute("y", std::to_string(y));
	object_node->set_attribute("ox", std::to_string(ox));
	object_node->set_attribute("oy", std::to_string(oy));
	object_node->set_attribute("icon_no", std::to_string(mapObj.get_icon()));
	object_node->set_attribute("layer", std::to_string(mapObj.get_layer()));
	object_node->set_attribute("id", mapObj.id);
	object_node->set_attribute("lua_name", mapObj.lua_name);
	object_node->set_attribute("how_many", boost::lexical_cast<std::string>(mapObj.how_many));
	object_node->set_attribute("random_monster", (mapObj.is_random_monster? "yes" : "no"));

	if (mapObj.openable) {
		if (mapObj.lock_type == NORMAL_LOCK)
			object_node->set_attribute("locked","normal");
		else if (mapObj.lock_type == MAGIC_LOCK)
			object_node->set_attribute("locked","magic");
		else if (mapObj.lock_type == UNLOCKED)
			object_node->set_attribute("locked","unlocked");
	}

	if (mapObj.personality == HOSTILE)
		object_node->set_attribute("personality","hostile");
	else if (mapObj.personality == RIGHTEOUS)
		object_node->set_attribute("personality","righteous");
	else
		object_node->set_attribute("personality","neutral");

	if (mapObj.move_mode == FLEE)
		object_node->set_attribute("move_mode","flee");
	else if (mapObj.move_mode == FOLLOWING)
		object_node->set_attribute("move_mode","following");
	else if (mapObj.move_mode == ROAM)
		object_node->set_attribute("move_mode","roam");
	else
		object_node->set_attribute("move_mode","static");

	switch (mapObj.get_type()) {
	case MAPOBJ_ITEM:
		object_node->set_attribute("type", "item");
		break;
	case MAPOBJ_ANIMAL:
		object_node->set_attribute("type", "animal");
		break;
	case MAPOBJ_PERSON:
		object_node->set_attribute("type", "person");
		break;
	default:
		object_node->set_attribute("type", "monster");
		break;
	}
	if (mapObj.get_init_script_path().length() > 0)
		object_node->set_attribute("init_script", mapObj.get_init_script_path());
	if (mapObj.get_combat_script_path().length() > 0)
		object_node->set_attribute("combat_script", mapObj.get_combat_script_path());

	// Write object actions, if there are any
	if (mapObj.actions()->size() > 0) {
		xmlpp::Element* actions_node = object_node->add_child("actions");

		for (auto curr_act = mapObj.actions()->begin(); curr_act != mapObj.actions()->end(); curr_act++) {
			xmlpp::Element* action_node = actions_node->add_child("action");
			write_action_node(action_node, curr_act->get());
		}
	}

	return true;
}

bool Map::xml_write_map_data(boost::filesystem::path path)
{
	xmlpp::Document* _main_map_xml_file;
	xmlpp::Node* _main_map_xml_root;

	try {
		_main_map_xml_file = new xmlpp::Document();

		// TODO: Check if _main_map_xml_root needs to be deleted again.
		_main_map_xml_root = _main_map_xml_file->create_root_node("map");
		_main_map_xml_root->add_child("name")->add_child_text(_name);
		if (_longname.length() > 0)
			_main_map_xml_root->add_child("longname")->add_child_text(_longname);
		_main_map_xml_root->add_child("outdoors")->add_child_text((is_outdoors()? "true" : "false"));
		_main_map_xml_root->add_child("dungeon")->add_child_text((is_dungeon? "true" : "false"));
		_main_map_xml_root->add_child("guarded_city")->add_child_text((guarded_city? "true" : "false"));
		if (initial_x >= 0 && initial_y >= 0) {
			_main_map_xml_root->add_child("initial_x")->add_child_text(std::to_string(initial_x));
			_main_map_xml_root->add_child("initial_y")->add_child_text(std::to_string(initial_y));
		}
		xmlpp::Element* data_entry = _main_map_xml_root->add_child("data");
		std::ostringstream curr_row;

		// Write map data
		for (unsigned y = 0; y < height(); y++) {
			for (unsigned x = 0; x < width(); x++)
				curr_row << (_data[y])[x] << ((x < width() - 1)? " " : "");

			data_entry->add_child("row")->add_child_text(curr_row.str());
			curr_row.str("");
		}

		// Write objects
		if (_map_objects.size() > 0) {
			xmlpp::Element* objects_node = _main_map_xml_root->add_child("objects");

			for (auto curr_obj = _map_objects.begin(); curr_obj != _map_objects.end(); curr_obj++) {
				xmlpp::Element* object_node = objects_node->add_child("object");
				MapObj mapObj = curr_obj->second;
				write_obj_xml_node(mapObj, object_node);
			}
		}

		// Write global map actions (& events)
		if (_actions.size() > 0) {
			xmlpp::Element* actions_node = _main_map_xml_root->add_child("actions");

			for (auto curr_act = _actions.begin(); curr_act != _actions.end(); curr_act++) {
				xmlpp::Element* action_node = actions_node->add_child("action");
				write_action_node(action_node, curr_act->get());
			}
		}

		if (path.empty()) { // Default
			_main_map_xml_file->write_to_file_formatted((World::Instance().get_path() / World::Instance().get_name() / "maps" / (_name + ".xml")).c_str());
		}
		else { // For saving game-status
			boost::filesystem::path save_to_where = path / World::Instance().get_name() / "maps" / (_name + ".xml");
			std::cout << "INFO: Saving map to " << save_to_where.string() << std::endl;
			_main_map_xml_file->write_to_file_formatted(save_to_where.c_str());
		}

		_modified = false;
	}
	catch (...)
	{
		std::cerr << "ERROR: map.cc: xml_write_world_data() failed." << std::endl;
		return false;
	}

	if (_main_map_xml_file != NULL)
	{
		delete _main_map_xml_file;
		_main_map_xml_file = NULL;
	}
	return true;
}

void Map::expand_map_data(int top, int bot, int right, int left)
{
	// Set up a row to add n times, if required
	std::vector<unsigned> row;
	for (unsigned i = 0; i < width(); i++)
		row.push_back(0);

	if (top != 0 && bot != 0 && right != 0 && left != 0)
		return;

	try {
		if (top > 0) {
			for (int times = 0; times < top; times++) {
				if (_data.size() > 0)
					_data.insert(_data.begin(), row);
				else
					_data.push_back(row);
			}
		}
		else if (top < 0) {
			for (int times = 0; times < abs(top); times++) {
				// You cannot completely remove the map!
				if (_data.size() > 1) {
					_data.begin()->clear();
					_data.erase(_data.begin());
				}
				else
					break;
			}
		}

		if (bot > 0)
			for (int times = 0; times < bot; times++)
				_data.push_back(row);
		else if (bot < 0) {
			for (int times = 0; times < abs(bot); times++) {
				// You cannot completely remove the map!
				if (_data.size() > 1) {
					_data.end()->clear();
					_data.pop_back();
				}
				else
					break;
			}
		}

		if (right > 0)
			for (unsigned col = 0; col < _data.size(); col++)
				for (int times = 0; times < right; times++)
					_data[col].push_back(0);
		else if (right < 0) {
			for (unsigned col = 0; col < _data.size(); col++) {
				for (int times = 0; times < abs(right); times++) {
					if (_data[col].size() > 1)
						_data[col].pop_back();
					else
						break;
				}
			}
		}

		if (left > 0)
			for (unsigned col = 0; col < _data.size(); col++)
				for (int times = 0; times < left; times++)
					_data[col].insert(_data[col].begin(), 0);
		else if (left < 0) {
			for (unsigned col = 0; col < _data.size(); col++) {
				for (int times = 0; times < abs(left); times++)	{
					if (_data[col].size() > 1)
						_data[col].erase(_data[col].begin());
					else
						break;
				}
			}
		}

		row.clear();
	}
	catch (...) {
		std::cerr << "ERROR: map.cc: BOOOOMMM!!! You better manually rescue your map data now." << std::endl;
	}

	// Now check/adjust objects...
	//
	// Some design rationale: Now it also becomes obvious that hash maps
	// are the wrong data structure for the objects as I have to
	// expensively copy and re-copy them around in memory.  A vector
	// would have been much better!  On the other hand in the game the
	// hash maps are the one and only choice for fast access to objects.
	// And objects will very rarely be copied around, e.g., only when
	// the player picks up an item and drops it somewhere else again.
	// So that's why in the editor I also went with hash maps.

	// Note to self: objects can only appear in inside maps.  Hence no
	// further case distinction below!

	// First, copy all objects with new coordinates, if they are not
	// deleted by a shrinking of the map, to a temporary vector
	std::vector<MapObj> tmp_objs;
	for (boost::unordered_multimap
			<std::pair<unsigned, unsigned>, MapObj>::iterator
			curr_obj = _map_objects.begin();
			curr_obj != _map_objects.end();
			curr_obj++)
	{
		unsigned old_x, old_y;
		curr_obj->second.get_coords(old_x, old_y);

		int new_y = (int)old_y + top;
		int new_x = (int)old_x + left;

		if (new_x >= 0
				&& new_y >= 0
				&& new_x < (int)width()
				&& new_y < (int)height())
		{
			curr_obj->second.set_coords(new_x, new_y);
			tmp_objs.push_back(curr_obj->second);
		}
	}
	_map_objects.clear();

	// Now copy all elements from the vector back into the hash map with
	// new keys, i.e., the changed object coordinates
	for (std::vector<MapObj>::iterator curr_obj = tmp_objs.begin(); curr_obj != tmp_objs.end(); curr_obj++) {
		std::pair<unsigned, unsigned> coords;
		unsigned the_x = 0, the_y = 0;
		curr_obj->get_coords(the_x, the_y);
		push_obj(*curr_obj);
		// push_obj(the_x, the_y, curr_obj->get_icon());
	}

	_modified = true;
}
