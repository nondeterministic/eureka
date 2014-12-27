#ifndef __SIMPLICISSIMUS_HH
#define __SIMPLICISSIMUS_HH

#include "weapon.hh"
#include "shield.hh"
#include "profession.hh"

#include <string>
#include <memory>

#include <boost/unordered_map.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/assign/list_of.hpp>

enum Alignment
{
  LEFTALIGN,
  CENTERALIGN,
  RIGHTALIGN
};

typedef boost::tuple<std::string, Alignment> line_tuple;

//extern boost::unordered_map< std::string, Weapon > weapons_map;
//extern boost::unordered_map< std::string, Shield > shields_map;

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
    (SHEPHERD, "SHEPHER")
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
