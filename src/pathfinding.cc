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
	// TODO: This offset of 10 is necessary, because the algorithm progresses by assigning x and y offsets to tiles
	// which might then refer to coordinates that are outside of the map area.  If this isn't added, then the destructor
	// crashes.  Alternatively, I could simply have paid more attention inside the algorithm.
	// But this whole algorithm/file is a hack and needs replacing some day...  :-(
	_width = map->width() + 10;
	_height = map->height() + 10;

	_all_paths = new int*[_height];
	for (int i = 0; i < _height; i++) {
		_all_paths[i] = new int[_width];

		for (int j = 0; j < _width; j++)
			_all_paths[i][j] = 30000000;
	}

	_visited= new bool*[_height];
	for (int i = 0; i < _height; i++) {
		_visited[i] = new bool[_width];

		for (int j = 0; j < _width; j++)
			_visited[i][j] = false;
	}
}

PathFinding::~PathFinding()
{
	for(int i = 0; i < _height; i++) {
		if (_visited[i] != NULL) {
			delete [] _visited[i];
			_visited[i] = NULL;
		}
	}
	if (_visited != NULL) {
		delete [] _visited;
		_visited = NULL;
	}

	for(int i = 0; i < _height; i++) {
		if (_all_paths[i] != NULL) {
			delete [] _all_paths[i];
			_all_paths[i] = NULL;
		}
	}
	if (_all_paths != NULL) {
		delete [] _all_paths;
		_all_paths = NULL;
	}
}

// TODO: We pass Map too in case we want to put this in a separate class later...

std::pair<unsigned,unsigned> PathFinding::follow_party(unsigned ox, unsigned oy, unsigned px, unsigned py)
{
	std::cout << "Follow: " << ox << ", " << oy << "\n";

	GameControl& gc = GameControl::Instance();

	int curr_sp = 0;
	int prev_sp = -2;
	unsigned new_x = ox;
	unsigned new_y = oy;

	for (int xoff = -1; xoff < 2; xoff++) {
		for (int yoff = -1; yoff < 2; yoff++) {
			if (gc.walkable(ox + xoff, oy + yoff)) {
				std::cout << "  Walkable: " << ox + xoff << ", " << oy + yoff << "\n";
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

// TODO: More of a safety net around _all_paths for avoiding out of bounds errors. Possibly not needed?!

int PathFinding::all_paths(int x, int y)
{
	if (x >=0 && x <= _width && y >=0 && y <= _height)
		return _all_paths[x][y];
	else {
		std::cerr << "ERROR: pathfinding.cc: Trying to access _all_paths out of bounds: " << x << ", " << y << "\n";
		exit(-1);
	}
}

int PathFinding::shortest_path(int ox, int oy, unsigned px, unsigned py)
{
	if (ox >= _width - 1 || oy >= _height - 1 || ox <= 0 || oy <= 0)
		return -1;

	if (abs(ox - (int)px) == 1 && abs(oy - (int)py) == 1)
		return 1;

	if (ox == (int)px && oy == (int)py)
		return 0;

	GameControl& gc = GameControl::Instance();
	std::vector<int> paths;

	_visited[ox][oy] = true;

	for (int xoff = -1; xoff < 2; xoff++) {
		for (int yoff = -1; yoff < 2; yoff++) {
			if (gc.walkable(ox + xoff, oy + yoff)) {
				if (_visited[ox + xoff][oy + yoff])
					paths.push_back(1 + all_paths(ox + xoff,oy + yoff));
				else
					paths.push_back(1 + shortest_path(ox + xoff, oy + yoff, px, py));
			}
		}
	}

	if (paths.size() > 0)
		_all_paths[ox][oy] = *std::min_element(paths.begin(), paths.end());

	return all_paths(ox,oy);
}

//int PathFinding::shortest_path(int ox, int oy, unsigned px, unsigned py)
//{
//	if (abs(ox - (int)px) == 1 && abs(oy - (int)py) == 1)
//		return 1;
//
//	if (ox == (int)px && oy == (int)py)
//		return 0;
//
//	if (ox >= _width - 1 || oy >= _height - 1 || ox <= 0 || oy <= 0)
//		return -1;
//
//	GameControl& gc = GameControl::Instance();
//	std::vector<int> paths;
//
//	_visited[ox][oy] = true;
//
//	for (int xoff = -1; xoff < 2; xoff++) {
//		for (int yoff = -1; yoff < 2; yoff++) {
//			if (gc.walkable(ox + xoff, oy + yoff)) {
//				if (_visited[ox + xoff][oy + yoff] && all_paths(ox + xoff,oy + yoff) > -1)
//					paths.push_back(1 + all_paths(ox + xoff,oy + yoff));
//				else if (!_visited[ox + xoff][oy + yoff]) {
//					int sp = shortest_path(ox + xoff, oy + yoff, px, py);
//					if (sp > -1)
//						paths.push_back(1 + sp);
//				}
//			}
//		}
//	}
//
//	if (paths.size() > 0)
//		_all_paths[ox][oy] = max(-1, *std::min_element(paths.begin(), paths.end()));
//	else
//		_all_paths[ox][oy] = -1;
//
//	return all_paths(ox,oy);
//}
