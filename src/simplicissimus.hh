#ifndef __SIMPLICISSIMUS_HH
#define __SIMPLICISSIMUS_HH

#include "weapon.hh"
#include "shield.hh"
#include "profession.hh"
#include "soundsample.hh"

#include <string>
#include <memory>

#include <boost/unordered_map.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/algorithm/string.hpp>

enum Alignment
{
  LEFTALIGN,
  CENTERALIGN,
  RIGHTALIGN
};

typedef boost::tuple<std::string, Alignment> line_tuple;

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
