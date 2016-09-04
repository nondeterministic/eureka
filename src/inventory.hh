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

#ifndef INVENTORY_HH
#define INVENTORY_HH

#include "item.hh"
#include "eureka.hh"

#include <list>
#include <memory>
#include <vector>
#include <map>

class Inventory
{
private:
	// First argument contains item name + description in one string without space in between.
	std::map<std::string, std::vector<Item*>> _items;

public:
	Inventory();
	~Inventory();
	int weight();
	Item* get_item(int);
	int how_many_at(int);
	std::vector<Item*>* get(int);
	std::map<std::string, int> list_wearables();
	std::map<std::string, int> list_all();
	//  std::vector<line_tuple> to_line_tuples(std::map<std::string, int>&);
	void add(Item*);
	void remove(std::string, std::string);
	int remove_all(std::string, std::string);
	void remove_all();
	unsigned size();
	unsigned number_items();
	void add_all(Inventory&);
};

#endif
