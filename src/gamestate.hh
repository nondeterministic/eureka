/*
 * gamestate.hh
 *
 *  Created on: Nov 2, 2014
 *      Author: baueran
 */

#ifndef GAMESTATE_HH_
#define GAMESTATE_HH_

#include "indoorsmap.hh"
#include <vector>

class GameState
{
protected:
	std::vector<IndoorsMap> _maps;
	GameState();

public:
	static GameState& Instance();
	virtual ~GameState();
	bool save(std::string = "");
	bool load(std::string = "");
	void add_map(IndoorsMap);
	IndoorsMap* get_map(std::string);
};

#endif /* GAMESTATE_HH_ */
