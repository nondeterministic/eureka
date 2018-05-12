// This source file is part of eureka
//
// Copyright (c) 2007-2018 Andreas Bauer <a@pspace.org>
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
