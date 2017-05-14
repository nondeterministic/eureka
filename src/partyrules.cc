#include "party.hh"
#include "partyrules.hh"
#include "iconprops.hh"
#include "gamecontrol.hh"

/// Returns true, if upon entering the force field, some party member got hurt.  False otherwise.

bool PartyRules::walk_through_magic_field(PropertyStrength strength)
{
	GameControl& gc = GameControl::Instance();
	bool somebody_hurt = false;

	if (Party::Instance().immune_from_fields() > 0)
		return somebody_hurt;

	int damage = 0;
	switch (strength) {
	case PropertyStrength::None:
		return somebody_hurt;
	case PropertyStrength::Some:
		damage = gc.random(10,20);
		break;
	default: // case PropertyStrength::Full:
		damage = gc.random(20,30);
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

/// Returns true, if upon entering the poison icon, some party member got hurt.  False otherwise.

bool PartyRules::walk_through_poison_field(PropertyStrength strength)
{
	bool somebody_hurt = false;

	if (Party::Instance().immune_from_fields() > 0)
		return somebody_hurt;

	if (strength == PropertyStrength::None)
		return somebody_hurt;

	GameControl& gc = GameControl::Instance();

	// TODO: Ignores currently any kind of protection, the party member may have against poison fields!
	for (auto player = Party::Instance().begin(); player != Party::Instance().end(); player++) {
		if (gc.random(0,10) < 5) {
			player->set_condition(PlayerCondition::POISONED);
			somebody_hurt = true;
		}
	}

	return somebody_hurt;
}
