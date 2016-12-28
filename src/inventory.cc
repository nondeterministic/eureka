// This source file is part of eureka
//
// Copyright (c) 2007-2016  Andreas Bauer <baueran@gmail.com>
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

#include "inventory.hh"
#include "item.hh"
#include "playercharacter.hh"
#include "party.hh"
#include "ztatswincontentprovider.hh"

#include <vector>
#include <iostream>
#include <sstream>
#include <map>

Inventory::Inventory()
{
}

Inventory::~Inventory()
{
	// std::cout << "~Inventory()\n";
}

// Return weight in stones.  Weight in weapon/shield files is given in kg!!

int Inventory::weight()
{
	int _w = 0;

	for (auto ptr = _items.begin(); ptr != _items.end(); ptr++) {
		_w += ptr->second.at(0)->weight() * ptr->second.size();
		// std::cout << ptr->second.at(0)->name() << " weighs " << ptr->second.at(0)->weight() << "\n";
	}

	return (int)(_w/6.35);
}

// Return how many items are in position n in the inventory.
//
// For example, inventory contains 5 bows and 3 arrows.  So _items.size() is only 2,
// whereas the first entry's size is 5 and the second entry's is 3.
//
// Returns 0 if n points to non-existent slot in the inventory, otherwise the size
// of that slot.

unsigned Inventory::how_many_at(unsigned n)
{
	if (n < _items.size()) {
		unsigned i = 0;
		for (auto ptr = _items.begin(); ptr != _items.end(); ptr++, i++) {
			if (i == n)
				return ptr->second.size();
		}
	}

	return 0;
}

unsigned Inventory::how_many_of(std::string item_name, std::string item_description)
{
	for (const std::pair<std::string, std::vector<Item*>>& item: _items)
		if (item.first == item_name + item_description) // See description of _items in header file for why this makes sense!
			return item.second.size();

	std::cout << "INFO: inventory.cc: how_man_of(" << item_name << ", " << item_description << ") returned 0.\n";
	return 0;
}

Item* Inventory::get_item(int n)
{
	if (n < (int)_items.size()) {
		int i = 0;
		for (auto ptr = _items.begin(); ptr != _items.end(); ptr++, i++) {
			if (i == n)
				return ptr->second.at(0);
		}
		std::cerr << "ERROR: inventory.cc (1): get_item() failed.\n";
		return NULL; // TODO: This should never happen!
	}
	else {
		std::cerr << "ERROR: inventory.cc (2): get_item() failed.\n";
		return NULL;
	}
}

// To be used in combination with zwin.select_item().  See gamecontrol::drop_items for an example.

std::vector<Item*>* Inventory::get(int n)
{
	int i = 0;
	for (auto ptr = _items.begin(); ptr != _items.end(); ptr++, i++) {
		if (i == n)
			return &(ptr->second);
	}
	std::cerr << "ERROR: inventory.cc: get() failed.\n";
	return NULL;
}

/*
std::vector<line_tuple> Inventory::to_line_tuples(std::map<std::string, int>& selection)
{
  std::vector<line_tuple> result;
  std::stringstream ss;
  int i = 1;

  for (auto ptr = selection.begin(); ptr != selection.end(); ptr++, i++) {
    ss << i << ") ";
    ss << ptr->first;
    if (ptr->second > 1)
      ss << " (" << ptr->second << ")";

    result.push_back(line_tuple(ss.str(), LEFTALIGN));
    ss.str(""); ss.clear();
  }

  return result;
}
*/

std::map<std::string, int> Inventory::list_wearables()
{
	std::map<std::string, int> result;

	for (auto ptr = _items.begin(); ptr != _items.end(); ptr++) {
		Item* item = ptr->second.at(0);
		result.insert(std::make_pair(item->name() + " " + item->description(), ptr->second.size()));
		// result.insert(std::make_pair(ptr->first, ptr->second.size()));
	}

	return result;
}

std::map<std::string, int> Inventory::list_all()
{
	std::map<std::string, int> result;

	for (auto ptr = _items.begin(); ptr != _items.end(); ptr++) {
		Item* item = ptr->second.at(0);
		result.insert(std::make_pair(item->name() + " " + item->description(), ptr->second.size()));
		// result.insert(std::make_pair(ptr->first, ptr->second.size()));
	}

	return result;
}

std::shared_ptr<ZtatsWinContentSelectionProvider<Item*>> Inventory::create_content_selection_provider(InventoryType inventory_type)
{
	std::shared_ptr<ZtatsWinContentSelectionProvider<Item*>> content_selection_provider(new ZtatsWinContentSelectionProvider<Item*>());
	std::vector<pair<StringAlignmentTuple, Item*>> content_page;
	int item_nr = 1;

	for (auto ptr = _items.begin(); ptr != _items.end(); ptr++, item_nr++) {
		Item* item = ptr->second.at(0);
		unsigned how_many_items = ptr->second.size();
		ostringstream item_content_stringstr;

		item_content_stringstr << item_nr << ") " << item->name() << " ";
		if (item->description().size() > 0)
			item_content_stringstr << "[" << item->description().substr(0, 5) << "] ";
		if (how_many_items > 1)
			item_content_stringstr << "(" << how_many_items << "x)";

		content_page.push_back(pair<StringAlignmentTuple, Item*>(StringAlignmentTuple(item_content_stringstr.str(), LEFTALIGN), item));
	}

	if (content_page.size() > 0) {
		content_selection_provider->create_content_page(content_page);
		std::cout << "CREATED CONTENT_SELECTION_PROVIDER: " << content_selection_provider->get_page().size() << "\n";
	}
	else
		std::cout << "INFO: inventory.cc: Created an empty ZtatsWinContentSelectionProvider object. Unless your inventory is actually empty, this is a (non-critical) bug.\n";

	return content_selection_provider;
}

std::shared_ptr<ZtatsWinContentProvider> Inventory::create_content_provider(InventoryType inventory_type)
{
	std::shared_ptr<ZtatsWinContentProvider> content_provider(new ZtatsWinContentProvider());
	std::vector<StringAlignmentTuple> content_page;
	int item_nr = 1;

	for (auto ptr = _items.begin(); ptr != _items.end(); ptr++, item_nr++) {
		Item* item = ptr->second.at(0);
		unsigned how_many_items = ptr->second.size();
		ostringstream item_content_stringstr;

		item_content_stringstr << item_nr << ") " << item->name() << " ";
		if (item->description().size() > 0)
			item_content_stringstr << "[" << item->description().substr(0, 5) << "] ";
		if (how_many_items > 1)
			item_content_stringstr << "(" << how_many_items << "x)";

		content_page.push_back(StringAlignmentTuple(item_content_stringstr.str(), LEFTALIGN));
	}

	if (content_page.size() > 0)
		content_provider->add_content_page(content_page);
	else
		std::cout << "INFO: inventory.cc: Created an empty ZtatsWinContentProvider object. Unless your inventory is actually empty, this is a (non-critical) bug.\n";

	return content_provider;
}

void Inventory::add(Item* item)
{
	// Don't add gold to inventory. Just add to party stats.
	if (item->name() == "gold coin") {
		Party& party = Party::Instance();
		party.set_gold(party.gold() + 1);
		return;
	}

	try {
		std::vector<Item*>& old_items = _items.at(item->name() + item->description());
		_items.insert(std::make_pair(item->name() + item->description(), old_items));
		old_items.push_back(item);
	}
	catch (std::out_of_range& oor) {
		std::vector<Item*> new_items;
		new_items.push_back(item);
		_items.insert(std::make_pair(item->name() + item->description(), new_items));
	}
}

// Removes the entry for an item named item_name and item_descr from the inventory.
// Throws exception if item is not in inventory.
// Frees memory!!

void Inventory::remove(std::string item_name, std::string item_descr)
{
	try {
		std::vector<Item*>& old_items = _items.at(item_name + item_descr); // Get all items with that name
		Item* removed_item = old_items.back();                             // Get ptr to exactly one of those items

		// If there were more than one item, remove one
		if (old_items.size() > 1)
			old_items.pop_back();
		// If there was exactly one, or none item remove that only one.
		else {
			if (_items.erase(item_name + item_descr) == 0)
				std::cerr << "ERROR: inventory.cc (1): Tried to erase '" << item_name << item_descr << "' from inventory but failed.\n";
		}

		// Free memory, if necessary
		if (removed_item != NULL)
			delete(removed_item);
	}
	catch (std::out_of_range& oor) {
		std::cerr << "WARNING: inventory.cc: out_of_range_exception in remove().\n";
	}
}

// Removes all entries for an item named item_name and item_descr from the inventory and returns that number.
// Throws exception if items are not in inventory.
// Frees memory!!

int Inventory::remove_all(std::string item_name, std::string item_descr)
{
	int how_many = 0;

	try {
		std::vector<Item*>& old_items = _items.at(item_name + item_descr);
		how_many = old_items.size();

		for (Item* i: old_items)
			if (i != NULL)
				delete(i);

		if (_items.erase(item_name + item_descr) == 0)
			std::cerr << "ERROR: inventory.cc (2): Tried to erase '" << item_name << item_descr << "' from inventory but failed.\n";
	}
	catch (std::out_of_range& oor) {
		std::cerr << "ERROR: inventory.cc: Failed to remove '" << item_name << item_descr << "'\n";
	}

	return how_many;
}

void Inventory::remove_all()
{
	std::vector<Item*> all_items;

//	std::vector<std::string> all_item_names;
//	for (auto ptr = _items.begin(); ptr != _items.end(); ptr++)
//		all_item_names.push_back(ptr->first);

	for (auto ptr = _items.begin(); ptr != _items.end(); ptr++) {
		Item* item = ptr->second.at(0); // Get representative Item from the beginning of vector
		all_items.push_back(item);
	}

	for (auto const& item: all_items)
		remove_all(item->name(), item->description());

	if (size() > 0)
		std::cerr << "WARNING: inventory.cc: Just tried to remove_all() but size() is " << size() << ".\n";

	if (number_items() > 0)
		std::cerr << "WARNING: inventory.cc: Just tried to remove_all() but number_items() is " << number_items() << ".\n";
}

// Returns total size of items, not just how many entries there are!

unsigned Inventory::number_items()
{
	unsigned _size = 0;

	for (auto ptr = _items.begin(); ptr != _items.end(); ptr++)
		_size += ptr->second.size();

	return _size;
}

unsigned Inventory::size()
{
	return _items.size();
}

void Inventory::add_all(Inventory& inv2)
{
	for (unsigned i = 0; i < inv2.size(); i++) {
		std::vector<Item*>* tmp_items = inv2.get(i);
		for (unsigned j = 0; j < tmp_items->size(); j++) {
			Item* new_item = tmp_items->at(j);
			add(new_item);
		}
	}
}
