// This source file is part of eureka
//
// Copyright (c) 2007-2018 Andreas Bauer <a@pspace.org>
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

#ifndef SRC_SPELLSBEARER_HH_
#define SRC_SPELLSBEARER_HH_

#include <string>
#include <map>
#include <vector>

#include <boost/unordered_map.hpp>

extern "C"
{
#include <lua.h>
}

typedef std::map<std::string, int> active_spell;

class SpellsBearer
{
private:
	boost::unordered_map<std::string, int> _active_spells;
	std::vector<int> _additional_walkable_icons;

public:
	void add_active_spell(std::string, int);
	// The string contains the name of the player, if any, whose spell effects need to be reversed when the spell duration has run out.
	void decrease_spells(lua_State*, std::string = "");

	// Return those icons the spell bearer can additionally walk to due to the effect of a magic spell.
	std::vector<int> get_additional_walkable_icons();
	void add_icon_to_walkable(int);
	void remove_icon_from_walkable(int);
};

#endif
