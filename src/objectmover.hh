#ifndef __OBJECTMOVER_HH
#define __OBJECTMOVER_HH

#include "gamecontrol.hh"
#include "mapobj.hh"
#include "party.hh"

#include <vector>
#include <utility>

class ObjectMover
{
private:
	GameControl* gc;
	Party* party;

public:
	ObjectMover();
	void move();

private:
	void do_actual_moving(MapObj*, std::vector<std::pair<int,int>>&, std::vector<std::pair<int,int>>&);
};

#endif
