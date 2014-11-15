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
#include <memory>

class GameState
{
protected:
	std::vector<std::shared_ptr<IndoorsMap>> _maps;
	GameState();

public:
	static GameState& Instance();
	virtual ~GameState();
	bool save(std::string = "");
	bool load(std::string = "");
	void add_map(std::shared_ptr<IndoorsMap>);
	std::shared_ptr<IndoorsMap> get_map(std::string);
};

#endif /* GAMESTATE_HH_ */
