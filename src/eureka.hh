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

#ifndef __SIMPLICISSIMUS_HH
#define __SIMPLICISSIMUS_HH

#include "profession.hh"
#include "config.h"

#include <string>
#include <memory>

#include <boost/unordered_map.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/path.hpp>

extern std::string             conf_world_name;
extern boost::filesystem::path conf_data_path;
extern boost::filesystem::path conf_world_path;
extern boost::filesystem::path conf_savegame_path;

#define CROSSHAIR_ICON_INDOORS    16
#define CROSSHAIR_ICON_OUTDOORS   41
#define CROSSHAIR_ICON_LUA_NAME    "temporary_icon_crosshair"

enum Emphasis
{
	NONE,
	VERY_LITTLE,
	SOME,
	STRONG,
	VERY_STRONG
};

const boost::unordered_map<Emphasis, std::string> emphasisToString =
	boost::assign::map_list_of
	(NONE, "NONE")
	(VERY_LITTLE, "VERY_LITTLE")
	(SOME, "SOME")
	(STRONG, "STRONG")
	(VERY_STRONG, "VERY_STRONG");

const boost::unordered_map<std::string, Emphasis> stringToEmphasis =
	boost::assign::map_list_of
	("NONE", NONE)
	("VERY_LITTLE", VERY_LITTLE)
	("SOME", SOME)
	("STRONG", STRONG)
	("VERY_STRONG", VERY_STRONG);

const boost::unordered_map<PROFESSION, std::string> professionToString =
	boost::assign::map_list_of
	(FIGHTER, "FIGHTER")
    (PALADIN, "PALADIN")
    (THIEF, "THIEF")
    (BARD, "BARD")
    (MAGE, "MAGE")
    (CLERIC, "CLERIC")
    (DRUID, "DRUID")
    (NECROMANCER, "NECROMANCER")
    (ARCHMAGE, "ARCHMAGE")
    (GEOMANCER, "GEOMANCER")
    (SHEPHERD, "SHEPHERD")
    (TINKER, "TINKER");

const boost::unordered_map<std::string, PROFESSION> stringToProfession =
	boost::assign::map_list_of
    ("FIGHTER", FIGHTER)
    ("PALADIN", PALADIN)
    ("THIEF", THIEF)
    ("BARD", BARD)
    ("MAGE", MAGE)
    ("CLERIC", CLERIC)
    ("DRUID", DRUID)
    ("NECROMANCER", NECROMANCER)
    ("ARCHMAGE", ARCHMAGE)
    ("GEOMANCER", GEOMANCER)
    ("SHEPHERD", SHEPHERD)
    ("TINKER", TINKER);

#endif
