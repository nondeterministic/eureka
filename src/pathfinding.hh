/*
 * pathfinding.h
 *
 *  Created on: Apr 4, 2015
 *      Author: baueran
 */

#ifndef SRC_PATHFINDING_H_
#define SRC_PATHFINDING_H_

#include <iostream>
#include <utility>
#include <memory>
#include <cstdlib>

#include "map.hh"

class PathFinding
{
private:
	int _width, _height;
	int**  _all_paths;
	bool** _visited;

public:
	PathFinding(Map*);
	~PathFinding();
	std::pair<unsigned,unsigned> follow_party(unsigned , unsigned , unsigned , unsigned );
	int shortest_path(int, int, unsigned, unsigned);
};

#endif /* SRC_PATHFINDING_H_ */
