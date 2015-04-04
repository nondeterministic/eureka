/*
 * pathfinding.cpp
 *
 *  Created on: Apr 4, 2015
 *      Author: baueran
 */

#include "pathfinding.hh"
#include "gamecontrol.hh"

#include <iostream>
#include <utility>
#include <memory>
#include <cstdlib>

PathFinding::PathFinding(Map* map)
{
	_width = map->width();
	_height = map->height();

	_visited = new bool*[_height];
	for (unsigned i = 0; i < _height; i++) {
		_visited[i] = new bool[_width];
		_visited[i] = { false };
	}
}

PathFinding::~PathFinding()
{
	for (unsigned i = 0; i < _height; i++)
		delete _visited[i];
	delete _visited;
}

// TODO: We pass Map too in case we want to put this in a separate class later...

std::pair<unsigned,unsigned> PathFinding::follow_party(unsigned ox, unsigned oy, unsigned px, unsigned py)
{
	GameControl& gc = GameControl::Instance();

	if (ox < px) {
		if (oy < py) {
			if (gc.walkable(ox + 1, oy + 1))
				std::make_pair(ox + 1, oy + 1);
		}
		else if (oy == py) {
			if (gc.walkable(ox + 1, oy + 1))
				std::make_pair(ox + 1, oy);
		}
		else {
			if (gc.walkable(ox + 1, oy - 1))
				std::make_pair(ox + 1, oy - 1);
		}
	}
}

// Is there a path from an object to the coordinates of the party?
// Returns shortest path length, if one exists, -1 otherwise.

int PathFinding::has_path(unsigned ox, unsigned oy, unsigned px, unsigned py)
{
	GameControl& gc = GameControl::Instance();

	using namespace std;

	if (ox == px && oy == py)
		return 0;
	if (abs(ox - px) == 1 && abs(oy - py) == 1)
		return 1;
	else {
		_visited[ox][oy] = true;

		return 1 + min(!_visited[ox + 1][oy + 1] && gc.walkable(ox + 1, oy + 1) && has_path(ox + 1, oy + 1, px, py),
			    		min(!_visited[ox][oy + 1] && gc.walkable(ox, oy + 1) && has_path(ox, oy + 1, px, py),
			    				min(!_visited[ox + 1][oy] && gc.walkable(ox + 1, oy) && has_path(ox + 1, oy, px, py),
			    						min(!_visited[ox - 1][oy - 1] && gc.walkable(ox - 1, oy - 1) && has_path(ox - 1, oy - 1, px, py),
			    								min(!_visited[ox][oy - 1] && gc.walkable(ox, oy - 1) && has_path(ox, oy - 1, px, py),
			    										!_visited[ox - 1][oy] && gc.walkable(ox - 1, oy) && has_path(ox - 1, oy, px, py))))));
	}
}
