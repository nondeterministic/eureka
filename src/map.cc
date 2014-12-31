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

#include <boost/unordered_map.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
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
#include "action.hh"
#include "actiononenter.hh"

Map::Map()
{
	guarded_city = false;
	_modified = true;
	// _main_map_xml_root = NULL;
	// _main_map_xml_file = NULL;
}

Map::Map(const Map& p)
{
	guarded_city = p.guarded_city;
	_name = p._name;
	_modified = p._modified;
	_data = p._data;
	_map_objects = p._map_objects;
	_actions = p._actions;
	std::cout << "DEEP COPY MOTHERFUCKER\n";
}

Map::~Map()
{
	std::cout << "WARNING: map.cc::~Map(): Also destroyed all objects " << _name << " contained!" << std::endl;
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

std::shared_ptr<Action> Map::get_action(unsigned x, unsigned y)
{
	std:: cout << "Actions: " << _actions.size() << std::endl;

	for (auto curr_act: _actions) {
		if (curr_act->get_x() == x && curr_act->get_y() == y) {
			return curr_act;
		}
	}

	std::cerr << "ERROR: map.cc::get_action(): returning empty shared_ptr (NULL) for an action.\n";
	// exit(-1);

	std::shared_ptr<Action> nullinger;
	return nullinger;
}

void Map::add_event_to_action(unsigned x, unsigned y, std::shared_ptr<GameEvent> ev)
{
	for (auto curr_act: _actions) {
		if (curr_act->get_x() == x && curr_act->get_y() == y) {
			curr_act->add_event(ev);
			return;
		}
	}
//	for (std::vector<Action>::iterator curr_act = _actions.begin(); curr_act != _actions.end(); curr_act++) {
//		if (curr_act->get_x() == x && curr_act->get_y() == y) {
//			curr_act->add_event(ev);
//			return;
//		}
//	}
}

void Map::add_action(std::shared_ptr<Action> new_act)
{
	// Only one action per icon is allowed!
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

void Map::pop_obj(int x, int y)
{
	std::pair<unsigned, unsigned> coords(x, y);
	std::pair<boost::unordered_multimap<std::pair<unsigned, unsigned>, MapObj>::iterator,
	boost::unordered_multimap<std::pair<unsigned, unsigned>, MapObj>::iterator>
	found_objs = _map_objects.equal_range(coords);

	// for (boost::unordered_multimap <std::pair<unsigned, unsigned>, MapObj>::iterator curr_obj = found_objs.first, next_obj = found_objs.first;
	for (auto curr_obj = found_objs.first, next_obj = found_objs.first;
			curr_obj != found_objs.second;
			curr_obj++)
	{
		// Delete only last element
		if (++next_obj == found_objs.second) {
			_map_objects.erase(curr_obj);
			_modified = true;
			break;
		}
	}
}

void Map::pop_obj(std::string pop_id)
{
	bool done = false;

	while (!done) {
		done = true;

		for (auto curr_obj = _map_objects.begin(); curr_obj != _map_objects.end(); curr_obj++) {
			MapObj& mo = (curr_obj->second);
			if (mo.id == pop_id) {
				_map_objects.erase(curr_obj);
				_modified = true;
				done = false;
				break;
			}
		}
	}
}

void Map::push_obj(MapObj obj)
{
	std::pair<unsigned, unsigned> coords;
	obj.get_coords(coords.first, coords.second);
	obj.set_layer(_map_objects.count(coords));

	_map_objects.insert(std::make_pair(coords, obj));
	_modified = true;
}

// Used to be called push_obj, but a MapObj is more than just an icon
// and coordinates.  So it became push_icon, e.g., to push crosshair
// icon, etc.  It still uses, internally, however, an underspecified
// MapObj.  That's OK for now...

void Map::push_icon(int x, int y, unsigned icon)
{
	MapObj new_obj;
	std::pair<unsigned, unsigned> coords;

	coords.first = x;
	coords.second = y;

	new_obj.set_coords((unsigned)x, (unsigned)y);
	new_obj.set_icon(icon);
	new_obj.set_layer(_map_objects.count(coords));

	_map_objects.insert(std::make_pair(coords, new_obj));
	_modified = true;
}

boost::unordered_multimap<std::pair<unsigned, unsigned>, MapObj>* Map::objs(void)
{
	return &_map_objects;
}

// This call is somewhat expensive with disk access, but it's more
// reliable to actually check for the file rather than handling a
// flag.

bool Map::exists_on_disk(void)
{
	return boost::filesystem::exists(World::Instance().get_path() + "/" +
			World::Instance().get_name() + "/" +
			"maps/" +
			_name + ".xml");
}

void Map::parse_objects_node(const xmlpp::Node* node)
{
	xmlpp::Node::NodeList list = node->get_children();
	for (xmlpp::Node::NodeList::iterator iter = list.begin(); iter != list.end(); ++iter) {
		MapObj new_obj;
		int x = 0, y = 0;

		const xmlpp::Element* nodeElement = dynamic_cast<const xmlpp::Element*>(*iter);
		if (nodeElement) {
			const xmlpp::Element::AttributeList& attributes = nodeElement->get_attributes();

			// Iterate through attributes of object node
			for (auto iter = attributes.begin(); iter != attributes.end(); ++iter) {
				const xmlpp::Attribute* attribute = *iter;
				std::string a_name = attribute->get_name();
				if (a_name == "x")
					x = atoi(attribute->get_value().c_str());
				else if (a_name == "y")
					y = atoi(attribute->get_value().c_str());
				else if (a_name == "icon_no")
					new_obj.set_icon(atoi(attribute->get_value().c_str()));
				else if (a_name == "layer")
					new_obj.set_layer(atoi(attribute->get_value().c_str()));
				else if (a_name == "init_script")
					new_obj.set_init_script_path(attribute->get_value().c_str());
				else if (a_name == "id")
					new_obj.id = attribute->get_value().c_str();
				else if (a_name == "removable")
					new_obj.removable = attribute->get_value() == "yes";
				else if (a_name == "type") {
					if (attribute->get_value() == "item")
						new_obj.set_type(MAPOBJ_ITEM);
					else
						new_obj.set_type(MAPOBJ_MONSTER);
				}
				else if (a_name == "lua_name")
					new_obj.lua_name = attribute->get_value().c_str();
				else if (a_name == "how_many")
					new_obj.how_many = atoi(attribute->get_value().c_str());
			}
			new_obj.set_coords(x, y);
			push_obj(new_obj);
		}
	}
}

void Map::parse_actions_node(const xmlpp::Node* node)
{
	// Get all action nodes
	xmlpp::Node::NodeList actions = node->get_children();
	for (auto action = actions.begin(); action != actions.end(); ++action) {
		const xmlpp::Element* nodeElement = dynamic_cast<const xmlpp::Element*>(*action);

		if (nodeElement) {
			if (nodeElement-> get_attribute_value("type") == "ACT_ON_ENTER") {
				std::shared_ptr<ActionOnEnter> _act(new ActionOnEnter(atoi(nodeElement->get_attribute_value("x").c_str()),
   	   	   	   	   	                                                  atoi(nodeElement->get_attribute_value("y").c_str()),
   	   	   	   	   	                                                  "ACT_ON_ENTER"));

				// Get all event nodes for an action
				xmlpp::Node::NodeList events = (*action)->get_children();
				for (auto event = events.begin(); event != events.end(); ++event) {
					const xmlpp::Element* eventElement = dynamic_cast<const xmlpp::Element*>(*event);

					if (eventElement) {
						std::string event_type_s = eventElement->get_attribute_value("type");

						if (event_type_s == "EVENT_ENTER_MAP") {
							// Check if all the required properties of the tag are there.
							std::cout << "Loaded node_list of length " << (*event)->get_children().size() << "\n";

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
								std::cerr << "XML load error: EventEnterMap malformed?" << std::endl;
						}
						else
							std::cerr << "XML load error: unsupported event type: " << event_type_s << std::endl;
					}
				}

				add_action(_act);
			}
		}
	}
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
	const xmlpp::Element* nodeElement = dynamic_cast<const xmlpp::Element*>(node);

	if (nodeElement) {
		// TODO: This is not very nice design, perhaps:
		// We don't read the outdoors tag, because at this stage, either
		// an indoors or outdoors map was already created by World().
		if (nodeElement->get_name() == "name")
			_name = nodeElement->get_child_text()->get_content();
		else if (nodeElement->get_name() == "guarded_city")
			guarded_city = nodeElement->get_child_text()->get_content() == "true"? true : false;
		else if (nodeElement->get_name() == "objects")
			parse_objects_node(node);
		else if (nodeElement->get_name() == "data")
			parse_data_node(node);
		else if (nodeElement->get_name() == "actions")
			parse_actions_node(node);
	}

	if (!nodeContent) {
		xmlpp::Node::NodeList list = node->get_children();
		for (xmlpp::Node::NodeList::iterator iter = list.begin();
				iter != list.end(); ++iter)
			parse_node(*iter);
	}
}

// A DOM parser that reads map data from disk

bool Map::xml_load_map_data(std::string pathToFile)
{
	std::string filepath;

	if (pathToFile.length() == 0)
		filepath = World::Instance().get_path() + "/" + World::Instance().get_name() + "/maps/" + _name + ".xml";
	else
		filepath = pathToFile;

	try {
		xmlpp::DomParser parser;
		// We just want the text to be resolved/unescaped automatically.
		// TODO: do we?
		parser.set_substitute_entities();
		parser.parse_file(filepath);
		if (parser) {
			// Deleted by DomParser:
			const xmlpp::Node* pNode = parser.get_document()->get_root_node();
			parse_node(pNode);
		}
	}
	catch(const std::exception& ex) {
		std::cout << "Exception upon loading map: " << ex.what() << std::endl;
		return false;
	}

	// Something we just read cannot be already modified by definition.
	_modified = false;
	return true;
}

// Note to self: Element inherits from Node.

void Map::write_action_node(xmlpp::Element* node, Action* action)
{
	ActionOnEnter* action_on_enter = dynamic_cast<ActionOnEnter*>(action);
	std::stringstream s_x;
	s_x << action->get_x();
	std::stringstream s_y;
	s_y << action->get_y();

	// Create action node
	if (action_on_enter) {
		node->set_attribute("type", "ACT_ON_ENTER");
		node->set_attribute("x", s_x.str());
		node->set_attribute("y", s_y.str());
	}
	else
		node->set_attribute("type", "Check Map::write_action_node() for error!");

	// Add event nodes if there are any
	for (auto curr_ev = action->events_begin(); curr_ev != action->events_end(); curr_ev++) {
		xmlpp::Element* ev_node = node->add_child("event");

		std::shared_ptr<EventEnterMap> event_enter_map = std::dynamic_pointer_cast<EventEnterMap>(*curr_ev);
		ev_node->set_attribute("type", "EVENT_ENTER_MAP");
		std::stringstream x_s;
		x_s << event_enter_map->get_x();
		std::stringstream y_s;
		y_s << event_enter_map->get_y();
		ev_node->add_child("x")->add_child_text(x_s.str());
		ev_node->add_child("y")->add_child_text(y_s.str());
		ev_node->add_child("target_map")->add_child_text(event_enter_map->get_map_name());
	}
}

bool Map::xml_write_map_data(std::string path)
{
	xmlpp::Document* _main_map_xml_file;
	xmlpp::Node* _main_map_xml_root;

	try {
		_main_map_xml_file = new xmlpp::Document();

		// TODO: Check if _main_map_xml_root needs to be deleted again.
		_main_map_xml_root = _main_map_xml_file->create_root_node("map");
		_main_map_xml_root->add_child("name")->add_child_text(_name);
		_main_map_xml_root->
		add_child("outdoors")->
		add_child_text((is_outdoors()? "true" : "false"));
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
				unsigned x, y;
				MapObj mapObj = curr_obj->second;

				mapObj.get_coords(x, y);
				std::stringstream tmp_coord;
				tmp_coord << x;
				object_node->set_attribute("x", tmp_coord.str());
				tmp_coord.str("");
				tmp_coord << y;
				object_node->set_attribute("y", tmp_coord.str());
				tmp_coord.str("");
				tmp_coord << mapObj.get_icon();
				object_node->set_attribute("icon_no", tmp_coord.str());
				tmp_coord.str("");
				tmp_coord << mapObj.get_layer();
				object_node->set_attribute("layer", tmp_coord.str());
				tmp_coord.str("");
				object_node->set_attribute("id", mapObj.id);
				object_node->set_attribute("lua_name", mapObj.lua_name);
				object_node->set_attribute("how_many", boost::lexical_cast<std::string>(mapObj.how_many));
				// object_node->set_attribute("init_script", mapObj.get_init_script_path());
				object_node->set_attribute("removable", (mapObj.removable? "yes" : "no"));
				switch (mapObj.get_type()) {
				case MAPOBJ_ITEM:
					object_node->set_attribute("type", "item");
					break;
				default:
					object_node->set_attribute("type", "monster");
					break;
				}
				tmp_coord.str("");
				if (mapObj.get_init_script_path().length() > 0) {
					object_node->set_attribute("init_script", mapObj.get_init_script_path());
				}
			}
		}

		// Write actions (& events)
		if (_actions.size() > 0) {
			xmlpp::Element* actions_node = _main_map_xml_root->add_child("actions");

			for (auto curr_act = _actions.begin(); curr_act != _actions.end(); curr_act++) {
				xmlpp::Element* action_node = actions_node->add_child("action");
				write_action_node(action_node, curr_act->get());
			}
		}

		if (path.length() == 0) { // Default
			_main_map_xml_file->write_to_file_formatted(World::Instance().get_path() + "/" +
														World::Instance().get_name() + "/" +
														"maps/" + _name + ".xml");
		}
		else { // For saving game-status
			std::string save_to_where = path + World::Instance().get_name() + "/" + "maps/" + _name + ".xml";
			std::cout << "Saving map to " << save_to_where << std::endl;
			_main_map_xml_file->write_to_file_formatted(save_to_where);
		}

		_modified = false;
	}
	catch (...)
	{
		std::cerr << "map.cc:xml_write_world_data() failed." << std::endl;
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

	try
	{
		if (top > 0)
		{
			for (int times = 0; times < top; times++)
			{
				if (_data.size() > 0)
					_data.insert(_data.begin(), row);
				else
					_data.push_back(row);
			}
		}
		else if (top < 0)
		{
			for (int times = 0; times < abs(top); times++)
			{
				// You cannot completely remove the map!
				if (_data.size() > 1)
				{
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
		else if (bot < 0)
		{
			for (int times = 0; times < abs(bot); times++)
			{
				// You cannot completely remove the map!
				if (_data.size() > 1)
				{
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
		else if (right < 0)
		{
			for (unsigned col = 0; col < _data.size(); col++)
			{
				for (int times = 0; times < abs(right); times++)
				{
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
		else if (left < 0)
		{
			for (unsigned col = 0; col < _data.size(); col++)
			{
				for (int times = 0; times < abs(left); times++)
				{
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
		std::cerr << "BOOOOMMM!!! You better manually rescue your map data now." << std::cerr;
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
