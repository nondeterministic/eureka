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

#include <utility>
#include <iostream>
#include <memory>
#include <map>

#include <libxml++/libxml++.h>

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include "eureka.hh"
#include "party.hh"
#include "jimmylock.hh"
#include "luaapi.hh"
#include "luawrapper.hh"

void Party::set_coords(int x, int y)
{
	this->x = x; this->y = y;
}

void Party::set_coords(std::pair<int, int> coords)
{
	this->x = coords.first; this->y = coords.second;
}

std::pair<int, int> Party::get_coords()
{
  std::pair<int, int> coords;
  coords.first = this->x; coords.second = this->y;
  return coords;
}

Party::Party()
{
	rounds_intoxicated = 0;
	x = 0; y = 0;
	_gold = 0;
	_food = 0;
	_guard = -1;
	_jlocks = 0;
	is_resting = false;
	is_in_combat = false;
	_magic_light_radius = 0;
	prev_x = -1;
	prev_y = -1;
}

Party& Party::Instance()
{
  static Party inst;
  return inst;
}

int Party::jimmylock_count()
{
	return _jlocks;
}

void Party::add_jimmylock()
{
	inventory()->add(new JimmyLock());
	_jlocks++;
}

void Party::rm_jimmylock()
{
	if (_jlocks <= 0) {
		std::cerr << "ERROR: Tried to remove jimmy lock from inventory although there doesn't seem to be one.\n";
		return;
	}

	JimmyLock l;
	inventory()->remove(l.name());
	_jlocks--;
}

int Party::gold()
{
  return _gold;
}

void Party::set_gold(int g)
{
  _gold = g;
}

int Party::food()
{
  return _food;
}

void Party::set_food(int g)
{
  _food = g;
}

void Party::store_outside_coords()
{
	prev_x = x;
	prev_y = y;
//	prev_indoors = _indoors;
//	prev_map_name = _map_name;
}

bool Party::restore_outside_coords()
{
	if (prev_x >= 0 && prev_y >= 0) {
		x = prev_x;
		y = prev_y;
		std::cout << "OLD COORDS RESTORED: " << x << ", " << y << "\n";
		return true;
	}

	std::cerr << "WARNING: party.cc: No previous or initial coordinates for current map found.\n";
	return false;

//  _indoors = prev_indoors;
//  _map_name = prev_map_name;
}

bool Party::indoors()
{
  return _indoors;
}

void Party::set_indoors(bool mode)
{
  _indoors = mode;
}

void Party::set_map_name(const char* name)
{
  _map_name = name;
}

std::string Party::map_name()
{
  return _map_name;
}

void Party::add_player(PlayerCharacter player)
{
  _players.push_back(player);
  std::cout << "INFO: party.cc: Added player " << player.name() << " to roaster.\n";
}

PlayerCharacter* Party::get_player(int number)
{
  int i = 0;
  for (auto player = Party::Instance().party_begin(); player != Party::Instance().party_end(); player++, i++)
    if (i == number)
      return &(*player);
  return NULL;
}

PlayerCharacter* Party::get_guard()
{
	if (_guard >= 0 && _guard < party_size() && get_player(_guard)->condition() != DEAD)
		return get_player(_guard);
	else {
		std::cerr << "WARNING: Trying to get player " << _guard << " as guard, but is either dead or out of bounds.\n";
		return NULL;
	}
}

void Party::set_guard(int g)
{
	if (g >= 0 && g < party_size() && get_player(g)->condition() != DEAD)
		_guard = g;
	else {
		std::cerr << "WARNING: Trying to set player " << g << " to be guard, but either dead or out of bounds.\n";
		_guard = -1;
	}
}

void Party::set_guard(PlayerCharacter* p)
{
	for (int i = 0; i < party_size(); i++) {
		if (p->name() == get_player(i)->name()) {
			set_guard(i);
			return;
		}
	}

	std::cerr << "WARNING: Trying to set player " << p->name() << " to be guard, but is probably dead.\n";
	_guard = -1;
}

void Party::unset_guard()
{
	_guard = -1;
}

PlayerCharacter* Party::get_player(std::string name)
{
  for (auto player = Party::Instance().party_begin(); player != Party::Instance().party_end(); player++)
    if (player->name() == name)
      return &(*player);
  return NULL;
}

// Returns the number of alive party members
unsigned Party::party_alive()
{
  unsigned i = 0;
  for (auto player = Party::Instance().party_begin(); player != Party::Instance().party_end(); player++)
    if (player->hp() > 0)
      i++;
  return i;
}

/**
 * Convert party to XML document so that it can be saved to disk for save-game.
 *
 * TODO: It seems I cannot return a node as it somehow auto-deletes itself,
 * even if used within a shared_ptr. So I return a string which then needs to
 * be parsed again, alas.
 */

std::string Party::to_xml()
{
	xmlpp::Document xml_doc;
	xmlpp::Element* partyNd = xml_doc.create_root_node("party");

	// Add general party stuff
	partyNd->add_child("x")->add_child_text(std::to_string(x));
	partyNd->add_child("y")->add_child_text(std::to_string(y));
	partyNd->add_child("map")->add_child_text(map_name());
	partyNd->add_child("indoors")->add_child_text(indoors()? "1":"0");
	partyNd->add_child("gold")->add_child_text(std::to_string(gold()));
	partyNd->add_child("jimmylocks")->add_child_text(std::to_string(jimmylock_count()));
	partyNd->add_child("food")->add_child_text(std::to_string(food()));

	// Add players
	xmlpp::Element* playersEl = partyNd->add_child("players");
	for (auto &player: _players) {
		xmlpp::Element* playerEl(playersEl->add_child("player"));
		playerEl->set_attribute("name", player.name());

		playerEl->add_child("profession")->add_child_text(professionToString.at(player.profession()));
		playerEl->add_child("ep")->add_child_text(std::to_string(player.ep()));
		playerEl->add_child("hp")->add_child_text(std::to_string(player.hp()));
		playerEl->add_child("hpm")->add_child_text(std::to_string(player.hpm()));
		playerEl->add_child("sp")->add_child_text(std::to_string(player.sp()));
		playerEl->add_child("spm")->add_child_text(std::to_string(player.spm()));
		playerEl->add_child("str")->add_child_text(std::to_string(player.str()));
		playerEl->add_child("luck")->add_child_text(std::to_string(player.luck()));
		playerEl->add_child("dxt")->add_child_text(std::to_string(player.dxt()));
		playerEl->add_child("wis")->add_child_text(std::to_string(player.wis()));
		playerEl->add_child("charr")->add_child_text(std::to_string(player.charr()));
		playerEl->add_child("iq")->add_child_text(std::to_string(player.iq()));
		playerEl->add_child("end")->add_child_text(std::to_string(player.end()));
		playerEl->add_child("sex")->add_child_text(player.sex()? "1":"0");
		playerEl->add_child("race")->add_child_text(std::to_string(player.race()));
		playerEl->add_child("level")->add_child_text(std::to_string(player.level()));

		if (player.weapon() != NULL)
			playerEl->add_child("weapon")->add_child_text(player.weapon()->luaName());
		else
			playerEl->add_child("weapon");

		if (player.shield() != NULL)
			playerEl->add_child("shield")->add_child_text(player.shield()->luaName());
		else
			playerEl->add_child("shield");
	}

	// Add inventory
	xmlpp::Element* invEl = partyNd->add_child("inventory");
	for (unsigned i = 0; i < Party::Instance().inventory()->size(); i++) {
		Item* item = inventory()->get_item(i);

		// Skip jimmy locks as these are written and read separately, not as part of the inventory
		JimmyLock l;
		if (item->name() == l.name())
			continue;

		xmlpp::Element* itemEl(invEl->add_child("item"));

		itemEl->set_attribute("how_many", std::to_string(Party::Instance().inventory()->how_many_at(i)));
		itemEl->add_child_text(item->luaName());
	}

	return xml_doc.write_to_string_formatted().c_str();
}

std::vector<PlayerCharacter>::iterator Party::party_begin()
{
  return _players.begin();
}

std::vector<PlayerCharacter>::iterator Party::party_end()
{
  return _players.end();
}

int Party::party_size()
{
  return _players.size();
}

Inventory* Party::inventory()
{
  return &_inv;
}

// Return max carrying capacity in stones.

int Party::max_carrying_capacity()
{
	int max_weight = 0;

	// First, calculate in KGs, strong ppl can carry 30, less strong 20, and weak ones 10
	for (auto &player: _players) {
		if (player.str() >= 12)
			max_weight += 30;
		else if (player.str() >= 7)
			max_weight += 20;
		else
			max_weight += 10;
	}

	return max_weight / 6.35;
}

// Sets the light radius around the party to radius for duration turns.

void Party::set_magic_light_radius(int radius)
{
	_magic_light_radius = radius;
}

int Party::light_radius()
{
	int radius = _magic_light_radius;

	for (PlayerCharacter &player: _players) {
		if (player.weapon() != NULL) {
			if (player.weapon()->light_radius() > 0)
				radius = max(radius, player.weapon()->light_radius());
		}

		// Check for glowing shields, spells, etc.
	}

	return radius;
}
