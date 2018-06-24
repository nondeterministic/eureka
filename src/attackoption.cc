// This source file is part of eureka
//
// Copyright (c) 2007-2018 Andreas Bauer <a@pspace.org>
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

#include "attackoption.hh"
#include "party.hh"
#include "gamecontrol.hh"
#include "soundsample.hh"
#include "luawrapper.hh"
#include "miniwin.hh"
#include "console.hh"
#include "gold.hh"
#include "creature.hh"
#include "gamerules.hh"

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lualib.h>
#include <lauxlib.h>
}

AttackOption::AttackOption(int pl, lua_State* ls)
{
	_L = ls;
	_player = Party::Instance().get_player(pl);
	_target = -1;
}

AttackOption::~AttackOption()
{

}

PlayerCharacter* AttackOption::attacking_player()
{
	return _player;
}

void AttackOption::set_target(int t)
{
	_target = t;
}

void AttackOption::execute(Combat* combat)
{
	static SoundSample sample;
	LuaWrapper lua(_L);
	stringstream ss;
	bool enemy_is_hit = false;
	int damage = 0;

	// Choose weapon to attack with
	Weapon* wep = _player->weapon();

	// Get the monster that is to be attacked this round
	Creature* opponent = NULL;
	int opponent_offset = 0;

	int j = 1;
	for (auto foe : *(combat->get_foes().count())) {
		if (j == _target) {
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
		if (wep->range() <= 10)
			ss << _player->name() << " swings the " << wep->name() <<  " at " << opponent->name();
		else if (wep->range() > 10) {
			if (wep->ammo() == "") // No ammo needed, such as magic sword or sling
				ss << _player->name() << " attacks " << opponent->name() << " using a" << (Util::vowel(wep->name()[0])? "n " : " ") <<  wep->name() << " ";
			else { // Ammo is needed, e.g., arrow or bolt
				if (Party::Instance().inventory()->contains_item_with_substr(wep->ammo())) {
					ss << _player->name() << " attacks " << opponent->name() << " using a" << (Util::vowel(wep->name()[0])? "n " : " ") <<  wep->name() << " ";
					Party::Instance().inventory()->remove(wep->ammo(), "");
				}
				else { // No ammo for range weapon!!
					printcon(_player->name() + " attempts to fire a" + (Util::vowel(wep->name()[0])? "n " : " ") + wep->name() + ", but has no " + wep->ammo() + ".", true);
					return;
				}
			}
		}

		// TODO: Check if this works, determining the opponents right AC, etc.!
		int  enemy_ac = GameRules::armour_class(opponent);
		if ((enemy_is_hit = random(1,20) - GameRules::bonus(_player->dxt()) - GameRules::bonus(_player->luck()) < enemy_ac))
			damage = std::max(1, random(wep->dmg_min(), wep->dmg_max())  +  GameRules::bonus(_player->str())); // Cannot hit with NO damage at all!
	}
	else if (wep != NULL) {
		ss << _player->name() << " tries to attack " << opponent->name() << " but this opponent is out of reach.";
		printcon(ss.str(), true);
		return;
	}
	// Attack with bare hands...
	else if (wep == NULL && opponent->distance() <= 10) {
		ss << _player->name() << " lands an attack at " << opponent->name();

		// TODO: Check if this works, determining the opponents right AC, etc.!
		int  enemy_ac = GameRules::armour_class(opponent);
		if ((enemy_is_hit = random(1,20) - GameRules::bonus(_player->dxt()) - GameRules::bonus(_player->luck()) < enemy_ac))
			damage = std::max(1, random(1, 4)  +  GameRules::bonus(_player->str())); // Cannot hit with NO damage at all!
	}
	// Attack with bare hands and enemy out of reach...
	else {
		ss << _player->name() << " lands a futile attack at " << opponent->name() << " without any noticable effect.";
		printcon(ss.str(), true);
		return;
	}

	if (enemy_is_hit) {
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
			_player->inc_ep(lua.call_fn<double>("get_ep"));

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

int AttackOption::random(int min, int max)
{
  return GameControl::Instance().random(min, max);
}

void AttackOption::printcon(const std::string s, bool wait)
{
  Console::Instance().print(&Charset::Instance(), s, wait);
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
