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

#include "serviceshelper.hh"
#include "service.hh"
#include "luawrapper.hh"
#include "party.hh"
#include "gamecontrol.hh"

#include <string>
#include <iostream>

#include <lua.h>
#include <lualib.h>

#include <boost/algorithm/string.hpp>

ServicesHelper::ServicesHelper()
{
/*	globArrayName = "Shields";*/
}

ServicesHelper::~ServicesHelper()
{
	// TODO Auto-generated destructor stub
}

// See ShieldHelper.

Service* ServicesHelper::createFromLua(std::string array_name, lua_State* lua_state)
{
	std::string globArrayName = "Services";
	LuaWrapper lua(lua_state);
	Service *w = new Service();

	w->name(lua.get_item_prop<std::string>(globArrayName, array_name, "name"));
	w->heal        = (int)(lua.get_item_prop<double>(globArrayName, array_name, "heal"));
	w->heal_poison = lua.get_item_prop<bool>(globArrayName, array_name, "heal_poison");
	w->resurrect   = lua.get_item_prop<bool>(globArrayName, array_name, "resurrect");
	w->print_after = lua.get_item_prop<std::string>(globArrayName, array_name, "print_after");
	w->level_up    = lua.get_item_prop<bool>(globArrayName, array_name, "level_up");

	w->gold((int)(lua.get_item_prop<double>(globArrayName, array_name, "gold")));

	return w;
}

void ServicesHelper::apply(Service* s, int party_member)
{
	Party& p = Party::Instance();
	PlayerCharacter* pl = p.get_player(party_member);

	// A service other than resurrect cannot be performed on a dead party member
	if (pl->hp() <= 0 && !s->resurrect) {
		GameControl::Instance().printcon(pl->name() + " is dead. Choose someone else, please.");
		return;
	}
	else if (s->heal > 0 && pl->hp() < pl->hpm()) {
		pl->set_hp(min(pl->hpm(), pl->hp() + s->heal));
	}
	else if (s->heal_poison && pl->condition() == PlayerCondition::POISONED) {
		pl->set_condition(GOOD);
	}
	else if (pl->hp() <= 0 && s->resurrect) {
		pl->set_condition(GOOD);
	}
	else if (s->level_up) {
		GameControl::Instance().printcon("TODO: NOT YET IMPLEMENTED.");  // TODO
		return;
	}
	else {
		GameControl::Instance().printcon("Not applicable.");
		return;
	}

	GameControl::Instance().printcon(s->print_after);

	// In the end, pay for service
	p.set_gold(p.gold() - s->gold());
}

// Returns true if the Lua array has an entry named item_name, false otherwise.

bool ServicesHelper::existsInLua(std::string item_name, lua_State* lua_state)
{
	LuaWrapper lua(lua_state);
	return lua.hasEntry("Services", item_name);
}
