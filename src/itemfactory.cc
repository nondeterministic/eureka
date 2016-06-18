/*
 * itemfactory.cpp
 *
 *  Created on: Jun 20, 2014
 *      Author: baueran
 */

#include "itemfactory.hh"
#include "item.hh"
#include "edibleshelper.hh"
#include "weaponhelper.hh"
#include "shieldhelper.hh"
#include "serviceshelper.hh"
#include "mischelper.hh"
#include "util.hh"

#include <boost/algorithm/string/predicate.hpp>
#include <string>
#include <vector>
#include <iostream>

ItemFactory::ItemFactory()
{
	// TODO Auto-generated constructor stub
}

ItemFactory::~ItemFactory()
{
	// TODO Auto-generated destructor stub
}

/**
 * Where lua_name is prefixed by weapon::lua_name, etc.
 */

Item* ItemFactory::create(std::string lua_name)
{
	std::vector<std::string> lname_split = Util::splitString(lua_name, "::");

	if (lname_split.size() > 0) {
		if (lname_split[0] == "weapons")
			return WeaponHelper::createFromLua(lname_split[1]);
		else if (lname_split[0] == "shields")
			return ShieldHelper::createFromLua(lname_split[1]);
		else if (lname_split[0] == "edibles")
			return EdiblesHelper::createFromLua(lname_split[1]);
		else if (lname_split[0] == "services")
			return ServicesHelper::createFromLua(lname_split[1]);
		else if (lname_split[0] == "miscitems")
			return MiscHelper::createFromLua(lname_split[1]);
		else
			throw std::runtime_error("create(): Don't know how to create item with lua_name '" + lua_name + "'.");
	}
	else
		throw std::runtime_error("create(): Cannot extract Lua name from '" + lua_name + "'.");
}

/**
 * Like create(), except that it doesn't need a prefix and returns NULL if item cannot be created.
 */

Item* ItemFactory::create_plain_name(std::string item_name)
{
	if (WeaponHelper::exists(item_name))
		return WeaponHelper::createFromLua(item_name);
	else if (ShieldHelper::exists(item_name))
		return ShieldHelper::createFromLua(item_name);
	else if (EdiblesHelper::exists(item_name))
		return EdiblesHelper::createFromLua(item_name);
	else if (ServicesHelper::exists(item_name))
		return ServicesHelper::createFromLua(item_name);
	else {
		// We didn't know which item to create. Should not happen.
		// TODO: Maybe throw exception?
		std::cout << "Don't know what to create in create_plain_name(): " << item_name << "." << std::endl;
		return NULL;
	}
}
