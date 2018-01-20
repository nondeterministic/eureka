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

#include "pathfinding.hh"
#include "gamecontrol.hh"

#include <iostream>
#include <utility>
#include <memory>
#include <cstdlib>
#include <queue>

PathFinding::PathFinding(Map* map) :  _gc(GameControl::Instance()), _map(map)
{
	// TODO: This offset of 100 is necessary, because the algorithm progresses by assigning x and y offsets to tiles
	// which might then refer to coordinates that are outside of the map area.  If this isn't added, then the destructor
	// crashes.  Alternatively, I could simply have paid more attention inside the algorithm.
	// But this whole algorithm/file is a hack and needs replacing some day...  :-(

	_width = _map->width() + 100;
	_height = _map->height() + 100;
}

// TODO: We pass Map too in case we want to put this in a separate class later...

std::pair<unsigned,unsigned> PathFinding::follow_party(unsigned ox, unsigned oy, unsigned px, unsigned py)
{
	int curr_sp = 0;
	int prev_sp = -2;
	unsigned new_x = ox;
	unsigned new_y = oy;

	for (int xoff = -1; xoff < 2; xoff++) {
		for (int yoff = -1; yoff < 2; yoff++) {
			if (_gc.walkable(ox + xoff, oy + yoff)) {
				curr_sp = shortest_path(ox + xoff, oy + yoff, px, py);
				if (prev_sp == -2) // If it's the initial path, initialise prev_sp
					prev_sp = curr_sp + 1;
				if (curr_sp <= prev_sp && curr_sp >= 0) {
					prev_sp = curr_sp;
					new_x   = ox + xoff;
					new_y   = oy + yoff;
				}
			}
		}
	}

	if (curr_sp >= 0)
		return std::make_pair(new_x, new_y);
	return std::make_pair(ox, oy);
}

int PathFinding::shortest_path(int ox, int oy, unsigned px, unsigned py)
{
	int max_length = 3000000;  // This is returned, when no plan is found, i.e. return an "infinitely"/unattractively long path.

	if (px >= _width || py >= _height)
		return max_length;

	std::queue<NodeDist> queue;
	bool** visited;

	// Setup
	visited = new bool*[_height];
	for (unsigned i = 0; i < _height; i++) {
		visited[i] = new bool[_width];

		for (unsigned j = 0; j < _width; j++)
			visited[i][j] = false;
	}

	NodeDist startNode;
	startNode.x = ox;
	startNode.y = oy;
	startNode.dist = 0;

	queue.push(startNode);
	visited[ox][oy] = true;

	while (queue.size() > 0) {
		NodeDist node = queue.front();
		queue.pop();

		if (abs(node.x - (int)px) == 1 && abs(node.y - (int)py) == 1) {
			destroy(visited);
			return node.dist + 1;
		}

		if (node.x == (int)px && node.y == (int)py) {
			destroy(visited);
			return node.dist;
		}

		for (int xoff = -1; xoff < 2; xoff++) {
			for (int yoff = -1; yoff < 2; yoff++) {
				// No diagonal movement!

				if (visited[node.x + xoff][node.y + yoff])
					continue;

				if (!_gc.walkable(node.x + xoff, node.y + yoff))
					continue;

				NodeDist newNode;
				newNode.x = node.x + xoff;
				newNode.y = node.y + yoff;
				newNode.dist = 1 + node.dist;

				queue.push(newNode);
				visited[newNode.x][newNode.y] = true;
			}
		}
	}

	destroy(visited);
	return max_length;
}

void PathFinding::destroy(bool** visited)
{
	for(unsigned i = 0; i < _height; i++) {
		if (visited[i] != NULL) {
			delete [] visited[i];
			visited[i] = NULL;
		}
	}
	if (visited != NULL) {
		delete [] visited;
		visited = NULL;
	}
}

//int PathFinding::shortest_path(int ox, int oy, unsigned px, unsigned py) const
//{
//	if (ox >= _width - 1 || oy >= _height - 1 || ox <= 0 || oy <= 0)
//		return -1;
//
//	if (abs(ox - (int)px) == 1 && abs(oy - (int)py) == 1)
//		return 1;
//
//	if (ox == (int)px && oy == (int)py)
//		return 0;
//
//	_visited[ox][oy] = true;
//
//	for (int xoff = -1; xoff < 2; xoff++) {
//		for (int yoff = -1; yoff < 2; yoff++) {
//			// No diagonal movement!
//			if ((xoff == -1 && yoff == 1) || (xoff == -1 && yoff == -1) || (xoff == 1 && yoff == -1) || (xoff == 1 && yoff == 1))
//				continue;
//
//			if (_gc.walkable(ox + xoff, oy + yoff)) {
//				if (_visited[ox + xoff][oy + yoff])
//					paths.push_back(1 + all_paths(ox + xoff,oy + yoff));
//				else
//					paths.push_back(1 + shortest_path(ox + xoff, oy + yoff, px, py));
//			}
//		}
//	}
//
//	if (paths.size() > 0)
//		_all_paths[ox][oy] = *std::min_element(paths.begin(), paths.end());
//
//	return all_paths(ox,oy);
//}
