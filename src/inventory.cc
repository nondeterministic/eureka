#include "inventory.hh"
#include "item.hh"
#include "playercharacter.hh"
#include "party.hh"

#include <vector>
#include <iostream>
#include <sstream>
#include <map>

Inventory::Inventory()
{
}

Inventory::~Inventory()
{
  std::cout << "~Inventory()\n";
}

// Return weight in stones.  Weight in weapon/shield files is given in kg!!

int Inventory::weight()
{
	int _w = 0;

	for (auto ptr = _items.begin(); ptr != _items.end(); ptr++)
		_w += ptr->second.at(0)->weight() * ptr->second.size();

	return (int)(_w/6.35);
}

Item* Inventory::get_item(int n)
{
	if (n < (int)_items.size()) {
		int i = 0;
		for (auto ptr = _items.begin(); ptr != _items.end(); ptr++, i++) {
			if (i == n)
				return ptr->second.at(0);
		}
		return NULL; // TODO: This should never happen!
	}
	else
		return NULL;
}

std::vector<Item*>* Inventory::get(int n)
{
  int i = 0;
  for (auto ptr = _items.begin(); ptr != _items.end(); ptr++, i++) {
    if (i == n)
      return &(ptr->second);
  }
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
    result.insert(std::make_pair(ptr->first, ptr->second.size()));
  }

  return result;
}

std::map<std::string, int> Inventory::list_all()
{
  std::map<std::string, int> result;

  for (auto ptr = _items.begin(); ptr != _items.end(); ptr++) {
    result.insert(std::make_pair(ptr->first, ptr->second.size()));
  }

  return result;
}

void Inventory::add(Item* item)
{
	try {
		std::vector<Item*>& old_items = _items.at(item->name());
		old_items.push_back(item);

		_items.insert(std::make_pair(item->name(), old_items));
	}
	catch (std::out_of_range& oor) {
		std::vector<Item*> new_items;
		new_items.push_back(item);
		_items.insert(std::make_pair(item->name(), new_items));
	}
}

// Removes the entry for an item named item_name from the inventory.
// Throws exception if item is not in inventory.
// Frees memory!!

void Inventory::remove(std::string item_name)
{
	try {
		std::vector<Item*>& old_items = _items.at(item_name); // Get all items with that name
		Item* removed_item = old_items.back();                // Get ptr to exactly one of those items

		// If there were more than one item, remove one
		if (old_items.size() > 1)
			old_items.pop_back();
		// If there was exactly one, or none item remove that only one.
		else
			_items.erase(item_name);

		// Free memory, if necessary
		if (removed_item != NULL)
			delete(removed_item);
	}
	catch (std::out_of_range& oor) {
	}
}

// Removes all entries for an item named item_name from the inventory and returns that number.
// Throws exception if items are not in inventory.
// Frees memory!!

int Inventory::remove_all(std::string item_name)
{
  int how_many = 0;

  try {
    std::vector<Item*>& old_items = _items.at(item_name);
    how_many = old_items.size();

    for (Item* i: old_items)
    	if (i != NULL)
    		delete(i);

    _items.erase(item_name);
  }
  catch (std::out_of_range& oor) {
    std::cout << "+++++++++++ Failed to remove " << item_name << "\n";
  }

  return how_many;
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
