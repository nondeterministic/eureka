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
