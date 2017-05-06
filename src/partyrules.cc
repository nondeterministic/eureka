#include "party.hh"
#include "partyrules.hh"
#include "iconprops.hh"
#include "gamecontrol.hh"

bool PartyRules::walk_through_magic_field(PropertyStrength strength)
{
	GameControl& gc = GameControl::Instance();
	bool somebody_hurt = false;

	int damage = 0;
	switch (strength) {
	case PropertyStrength::None:
		return somebody_hurt;
	case PropertyStrength::Some:
		damage = gc.random(5,10);
		break;
	default: // case PropertyStrength::Full:
		damage = gc.random(10,20);
		break;
	}

	// TODO: Ignores currently any kind of protection, the party member may have against magical force fields!
	for (auto player = Party::Instance().begin(); player != Party::Instance().end(); player++) {
		if (gc.random(0,10) < 5) {
			player->add_hp(-damage);
			somebody_hurt = true;
		}
	}

	return somebody_hurt;
}
