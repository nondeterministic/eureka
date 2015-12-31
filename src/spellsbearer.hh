#ifndef SRC_SPELLSBEARER_HH_
#define SRC_SPELLSBEARER_HH_

#include <string>
#include <map>
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

public:
	void add_active_spell(std::string, int);
	// The string contains the name of the player, if any, whose spell effects need to be reversed when the spell duration has run out.
	void decrease_spells(lua_State*, std::string = "");
};

#endif
