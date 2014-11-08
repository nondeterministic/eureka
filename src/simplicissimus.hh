#ifndef __SIMPLICISSIMUS_HH
#define __SIMPLICISSIMUS_HH

#include "weapon.hh"
#include "shield.hh"
#include <string>
#include <memory>
#include "boost/unordered_map.hpp"
#include "boost/tuple/tuple.hpp"

enum Alignment
{
  LEFTALIGN,
  CENTERALIGN,
  RIGHTALIGN
};

typedef boost::tuple<std::string, Alignment> line_tuple;

extern boost::unordered_map< std::string, Weapon > weapons_map;
extern boost::unordered_map< std::string, Shield > shields_map;

#endif
