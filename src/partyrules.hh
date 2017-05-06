#ifndef __PARTYRULES_HH
#define __PARTYRULES_HH

#include "party.hh"
#include "iconprops.hh"

class PartyRules
{
public:
	bool walk_through_magic_field(PropertyStrength);
	bool walk_through_poison_field(PropertyStrength);
};

#endif
