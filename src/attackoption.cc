// This source file is part of eureka
//
// Copyright (c) 2007-2016  Andreas Bauer <baueran@gmail.com>
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

#include "attackoption.hh"
#include "party.hh"
#include "gamecontrol.hh"
#include "soundsample.hh"
#include "luawrapper.hh"
#include "miniwin.hh"
#include "console.hh"
#include "gold.hh"

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lualib.h>
#include <lauxlib.h>
}

AttackOption::AttackOption(int pl, lua_State* ls)
{
	L = ls;
	player = Party::Instance().get_player(pl);
	target = -1;
}

AttackOption::~AttackOption()
{

}

PlayerCharacter* AttackOption::attacking_player()
{
	return player;
}

void AttackOption::set_target(int t)
{
	target = t;
}

void AttackOption::execute(Combat* combat)
{
	static SoundSample sample;
	LuaWrapper lua(L);

	// Choose weapon to attack with
	Weapon* wep = player->weapon();

	// Get the monster that is to be attacked this round
	Creature* opponent = NULL;
	int opponent_offset = 0;

	int j = 1;
	for (auto foe : *(combat->get_foes().count())) {
		if (j == target) {
			int k = 0;
			for (auto _foe = combat->get_foes().begin(); _foe != combat->get_foes().end(); _foe++, k++) {
				if (foe.first == (*_foe)->name()) {
					opponent = _foe->get(); // Opponent now points to the monster to be attacked
					opponent_offset = k;
					break;
				}
			}
		}
		j++;
	}

	if (opponent == NULL) {
		std::cerr << "ERROR: attackoption.cc: opponent == null (No monster to attack.)\n";
		return;
	}

	if (wep != NULL && opponent->distance() <= wep->range()) {
		stringstream ss;
		ss << player->name() + " swings the " + wep->name() + " at " << opponent->name();

		int temp_AC = 10; // TODO: Replace this with the actual AC of opponent!  This AC needs to be computed from weapons, dex, etc.

		if (random(1, 20) > temp_AC) {
			int damage = random(wep->dmg_min(), wep->dmg_max());
			ss << " and hits for " << damage << " points of damage.";
			if (opponent->hp() - damage > 0) {
				opponent->set_hp(opponent->hp() - damage);
				lua.push_fn_arg((double)(opponent->hp() - damage));
				lua.call_void_fn("set_hp");
			}
			else {
				ss << " killing the " << opponent->name() << ".";
				combat->get_foes().remove(opponent_offset);

				// Add experience points to player's balance
				player->inc_ep(lua.call_fn<double>("get_ep"));

				// Now add monster's items to bounty items to be collected
				// by party in case of battle victory.
				if (opponent->weapon() != NULL)
					combat->add_to_bounty(opponent->weapon());

				// Add monster's gold
				int gold_coins = lua.call_fn<double>("get_gold");
				for (int ii = 0; ii < gold_coins; ii++)
					combat->add_to_bounty(new Gold());
			}
			printcon(ss.str(), true);
			MiniWin::Instance().alarm();
			sample.play_predef(FOE_HIT);
		}
		else {
			ss << " and misses.";
			printcon(ss.str(), true);
		}
	}
	else if (wep != NULL) {
		stringstream ss;
		ss << player->name() << " tries to attack " << opponent->name() << " but cannot reach.";
		printcon(ss.str(), true);
	}
	// Attack with bare hands...
	else {
		stringstream ss;
		ss << player->name() << " lands a futile punch at " << opponent->name() << " without any noticable effect.";
		printcon(ss.str(), true);
	}
}

int AttackOption::random(int min, int max)
{
  return GameControl::Instance().random(min, max);
}

void AttackOption::printcon(const std::string s, bool wait)
{
  Charset normal_font;
  Console::Instance().print(&normal_font, s, wait);
}

DefendOption::DefendOption(int pl) : AttackOption(pl, NULL)
{
	message = ""; // see attackoption.hh
}

DefendOption::~DefendOption()
{
}

void DefendOption::execute(Combat* combat)
{
	if (message.length() > 0)
		printcon(message, true);
}
