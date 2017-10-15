// This source file is part of eureka
//
// Copyright (c) 2007-2017 Andreas Bauer <baueran@gmail.com>
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
#include <sstream>
#include <algorithm>
#include <vector>
#include <map>
#include <memory>

#include <boost/foreach.hpp>
#include <boost/unordered_map.hpp>

#include <SDL2/SDL.h>

#include "sdl_shared.hh"
#include "attackers.hh"
#include "creature.hh"
#include "sdlwindow.hh"

Attackers::Attackers()
{
}

Attackers::~Attackers()
{
}

Attackers::Attackers(const Attackers& a)
{
	for (auto e : a._enemies)
		_enemies.push_back(e);

	// Copy "boost::unordered_map<std::string, int> _enemies_count":
	BOOST_FOREACH(auto ec, a._enemies_count) {
		_enemies_count[ec.first] = ec.second;
    }

	std::cout << "ATTACKERS DEEP COPY MOTHERFUCKER!\n";
}

// Gets n-th enemy in the vector.
// You will not like calling this, when n > _enemies.size().

Creature* Attackers::get(int n)
{
	return _enemies[n].get();
}

// Returns pointer to creature at distance, distance, or NULL if no
// creature is at distance, distance.

Creature* Attackers::get_attacker(int distance)
{
	for (auto c = _enemies.begin(); c != _enemies.end(); c++) {
		if ((*c)->distance() == distance)
			return c->get();
	}

	return NULL;
}

// Removes an enemy at position enemies_offset from the
// _enemies-vector.  
// 
// This function as another very noticable SIDEFFECT: It also adjusts
// the _enemies_count hash map.  Be mindful of that!

void Attackers::remove(int enemies_offset)
{
	// TODO I should somehow check if erase failed...
	// std::cout << "Enemies before: " << _enemies.size() << "\n";
	string erased_name = _enemies.at(enemies_offset)->name();
	_enemies.erase(_enemies.begin() + enemies_offset);
	// std::cout << "Enemies  after: " << _enemies.size() << "\n";

	for (auto e = _enemies_count.begin(); e != _enemies_count.end(); e++) {
		if (e->first == erased_name) {
			if (e->second > 1) {
				e->second--;
				return;
			}
			else {
				_enemies_count.erase(e);
				return;
			}
		}
	}
}

// Returns the distance of monster with name single_name (e.g., "Orc")

int Attackers::get_distance(const string& single_name)
{
	for (unsigned i = 0; i < _enemies.size(); i++) {
		Creature* c = _enemies.at(i).get();
		if (c->name() == single_name) {
			return c->distance();
		}
	}
	throw single_name + " cannot compute distance.";
}

// Example: single_name = Orc -> Orcs
// 
// TODO: This function is REALLY inefficient!

std::string Attackers::get_plural_name(const string& single_name)
{
	for (unsigned i = 0; i < _enemies.size(); i++) {
		Creature* c = _enemies.at(i).get();
		if (c->name().compare(single_name) == 0)
			return c->plural_name();
	}
	throw single_name + " not in group of attacking monsters.";
}

std::vector< std::shared_ptr<Creature> >::iterator Attackers::begin()
{
	return _enemies.begin();
}

std::vector< std::shared_ptr<Creature> >::iterator Attackers::end()
{
	return _enemies.end();
}

const std::vector< std::shared_ptr<Creature> >* Attackers::get()
{
	return &_enemies;
}

// Returns a hash map that contains the distances of all monsters
// groupwise.  E.g., if 5 Orks at 50" and 3 Trolls at 30" attack, then
// the table will look like: < <50, Ork>, <30, Troll> >.

boost::unordered_map<int, std::string> Attackers::distances()
{
	boost::unordered_map<int, std::string> result;

	for (auto itr = _enemies.begin(); itr != _enemies.end(); itr++)
		result[(*itr)->distance()] = (*itr)->name();

	return result;
}

std::string Attackers::attacker_at(int distance)
{
	try {
		return distances().at(distance);
	}
	catch (std::out_of_range& oor) {
		return "";
	}
}

void Attackers::move(const char* name, int dist)
{
	for (auto itr = _enemies.begin(); itr != _enemies.end(); itr++) {
		std::string tmp = name;
		if (tmp == (*itr)->name())
			(*itr)->set_distance((*itr)->distance() + dist);
	}
}

boost::unordered_map<std::string, int>* Attackers::count()
{
	return &_enemies_count;
}

std::shared_ptr<SDL_Texture> Attackers::pic(SDL_Renderer* renderer)
{
	if (_enemies.size() > 0) {
		SDL_Surface* surf = _enemies.at(0)->load_img();
		// This pointer intentionally left unfreed.
		auto image(sdl_shared(SDL_CreateTextureFromSurface(renderer, surf)));
		return image;
	}
	return NULL;
}

std::string Attackers::to_string()
{
	std::stringstream ss;
	std::map<int, std::string> ordered_attackers;

	// First sort output by the enemy that is closest to the one
	// furthest away from party
	for (auto itr = _enemies_count.begin(); itr != _enemies_count.end(); itr++) {
		ordered_attackers.insert(make_pair(get_distance(itr->first), itr->first));
		// cout << "Computed distance of " << itr->first << ": " << get_distance(itr->first) << endl;
	}

	// TODO: The following loop is for testing purposes only
	//  for (auto itr = _enemies.begin(); itr != _enemies.end(); itr++)
	//    cout << "Real distance of " << (*itr)->name() << get_distance((*itr)->name()) << endl;

	if (_enemies.size() > 1) {
		ss << "a group of ";
		unsigned i = 0;
		for (auto itr = ordered_attackers.begin(); itr != ordered_attackers.end(); itr++, i++) {
			int count = _enemies_count.find(itr->second)->second;
			ss << count << " "
					<< (count > 1? get_plural_name(itr->second) : itr->second) << " ("
					<< get_distance(itr->second) << "')";
			if (i <= _enemies_count.size() - 2 && _enemies_count.size() > 2)
				ss << ", ";
			else if (i < _enemies_count.size() - 1)
				ss << " and ";
		}
	}
	else
		ss << "1 " <<_enemies.at(0)->name() << " (" << get_distance(_enemies.at(0)->name()) << "')";

	return ss.str();
}

int Attackers::size() const
{
	return _enemies.size();
}

void Attackers::add(std::shared_ptr<Creature> c)
{
	_enemies.push_back(c);
}

// Returns range of closest enemy relative to party

int Attackers::closest_range()
{
	if (_enemies.size() > 0) {
		int lowest = _enemies.at(0)->distance();

		for (auto itr = _enemies.begin(); itr != _enemies.end(); itr++)
			if ((*itr)->distance() < lowest)
				lowest = (*itr)->distance();

		return lowest;
	}
	else {
		std::cerr << "ERROR: No enemies in range at all!\n";
		return 0;
	}
}
