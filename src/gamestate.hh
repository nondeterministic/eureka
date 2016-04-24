// This source file is part of Simplicissimus
//
// Copyright (c) 2007-2016  Andreas Bauer <baueran@gmail.com>
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

#ifndef GAMESTATE_HH_
#define GAMESTATE_HH_

#include "indoorsmap.hh"

#include <vector>
#include <memory>

#include <boost/filesystem.hpp>

class GameState
{
protected:
	std::vector<std::shared_ptr<IndoorsMap>> _maps;
	GameState();
	bool save_party(boost::filesystem::path);

public:
	static GameState& Instance();
	virtual ~GameState();
	bool save();
	bool load();
	void add_map(std::shared_ptr<IndoorsMap>);
	std::shared_ptr<IndoorsMap> get_map(std::string);
};

#endif /* GAMESTATE_HH_ */
