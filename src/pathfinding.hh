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
	unsigned _width, _height;
	bool** _visited;

public:
	PathFinding(Map*);
	~PathFinding();
	std::pair<unsigned,unsigned> follow_party(unsigned ox, unsigned oy, unsigned px, unsigned py);
	int has_path(unsigned ox, unsigned oy, unsigned px, unsigned py);
};

#endif /* SRC_PATHFINDING_H_ */
