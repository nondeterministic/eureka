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

#ifndef SRC_PATHFINDING_H_
#define SRC_PATHFINDING_H_

#include <utility>

#include "map.hh"
#include "gamecontrol.hh"
#include "map.hh"

class GameControl;

typedef struct {
	int x, y, dist;
} NodeDist;

class PathFinding
{
private:
	const GameControl& _gc;
	const Map* _map;
	const unsigned _width, _height;

public:
	PathFinding(Map*);
	std::pair<unsigned,unsigned> follow_party(unsigned , unsigned , unsigned , unsigned);
	int shortest_path(int, int, unsigned, unsigned);
};

#endif /* SRC_PATHFINDING_H_ */
