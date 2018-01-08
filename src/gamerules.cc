// This source file is part of eureka
//
// Copyright (c) 2007-2018 Andreas Bauer <baueran@gmail.com>
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

#include <iostream>

#include "gamerules.hh"
#include "gamecharacter.hh"

int GameRules::bonus(int value)
{
	if (value >= 19)
		return 3;
	else if (value >= 17)
		return 2;
	else if (value >= 15)
		return 1;
	else if (value >= 12)
		return 0;
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
