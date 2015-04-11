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

	_all_paths = new int*[_height];
	for (int i = 0; i < _height; ++i) {
		_all_paths[i] = new int[_width];

		for (int j = 0; j < _width; j++)
			_all_paths[i][j] = -1;
	}

	_visited= new bool*[_height];
	for (int i = 0; i < _height; ++i) {
		_visited[i] = new bool[_width];

		for (int j = 0; j < _width; j++)
			_visited[i][j] = false;
	}
}

PathFinding::~PathFinding()
{
	for(int i = 0; i < _height; ++i)
	    delete [] _visited[i];
	delete [] _visited;

	for(int i = 0; i < _height; ++i)
	    delete [] _all_paths[i];
	delete [] _all_paths;
}

// TODO: We pass Map too in case we want to put this in a separate class later...

std::pair<unsigned,unsigned> PathFinding::follow_party(unsigned ox, unsigned oy, unsigned px, unsigned py)
{
	GameControl& gc = GameControl::Instance();

	int curr_sp = 0;
	int prev_sp = -2;
	unsigned new_x = ox;
	unsigned new_y = oy;

	for (int xoff = -1; xoff < 2; xoff++) {
		for (int yoff = -1; yoff < 2; yoff++) {
			if (gc.walkable(ox + xoff, oy + yoff)) {
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
	if (abs(ox - px) == 1 && abs(oy - py) == 1)
		return 1;

	if (ox == (int)px && oy == (int)py)
		return 0;

	if (ox >= _width - 1 || oy >= _height - 1 || ox <= 0 || oy <= 0)
		return -1;

	GameControl& gc = GameControl::Instance();
	std::vector<int> paths;

	_visited[ox][oy] = true;

	for (int xoff = -1; xoff < 2; xoff++) {
		for (int yoff = -1; yoff < 2; yoff++) {
			if (gc.walkable(ox + xoff, oy + yoff)) {
				if (_visited[ox + xoff][oy + yoff] && _all_paths[ox + xoff][oy + yoff] > -1)
					paths.push_back(1 + _all_paths[ox + xoff][oy + yoff]);
				else if (!_visited[ox + xoff][oy + yoff]) {
					int sp = shortest_path(ox + xoff, oy + yoff, px, py);
					if (sp > -1)
						paths.push_back(1 + sp);
				}
			}
		}
	}

	if (paths.size() > 0)
		_all_paths[ox][oy] = max(-1, *std::min_element(paths.begin(), paths.end()));
	else
		_all_paths[ox][oy] = -1;

	return _all_paths[ox][oy];
}
