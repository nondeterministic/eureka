#include <iostream>

#include "gamerules.hh"
#include "gamecharacter.hh"

int GameRules::bonus(int value)
{
	if (value >= 19)
		return 4;
	else if (value >= 17)
		return 3;
	else if (value >= 15)
		return 2;
	else if (value >= 12)
		return 1;
	else if (value <= 3)
		return -3;
	else if (value <= 5)
		return -2;
	else if (value <= 8)
		return -1;
	else
		return 0;
}

// Determine armour class of creature, monster or player alike

int GameRules::armour_class(GameCharacter* gc)
{
	int ac = gc->base_ac() -
			bonus(gc->dxt()) -
			(gc->shield()? gc->shield()->protection() : 0) -
			(gc->armour()? gc->armour()->protection() : 0) -
			(gc->armour_head()? gc->armour_head()->protection() : 0) -
			(gc->armour_feet()? gc->armour_feet()->protection() : 0) -
			(gc->armour_hands()? gc->armour_hands()->protection() : 0);
	return ac;
}
