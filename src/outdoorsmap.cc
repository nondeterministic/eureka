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

#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include "outdoorsmap.hh"

OutdoorsMap::OutdoorsMap(const char* newname)
{
  set_name(newname);
}

OutdoorsMap::OutdoorsMap(unsigned columns, unsigned rows)
{
  std::vector<unsigned> row;

  for (unsigned i = 0; i < rows; i++) {
    for (unsigned i = 0; i < columns; i++)
      row.push_back(0);
    _data.push_back(row);
    row.clear();
  }
  
  _modified = true;
};

OutdoorsMap::~OutdoorsMap()
{
}

bool OutdoorsMap::is_within_visible_bounds(int x, int y)
{
	return x < 1 ||
		   x >= (int)width() - 4 ||
		   y >= (int)height() - 4 ||
		   y < 1;
}

bool OutdoorsMap::is_outdoors() const
{
  return true;
}

int OutdoorsMap::get_tile(unsigned x, unsigned y)
{
	if (y%2 == 0)
		y = y/2;
	else
		y = (y-1)/2;

	if (x < width() && y < height()) {
		return (_data[y])[x];
	}

	// std::cerr << "ERROR: outdoorsmap.cc: get_tile(" << x << ", " << y << ") is out of map bounds (" << _data[0].size() << "x" << _data.size() << ").\n";
	return -1;
}

int OutdoorsMap::set_tile(unsigned x, unsigned y, unsigned icon_no)
{
	if (y%2 == 0)
		y = y/2;
	else
		y = (y-1)/2;

	if (_data.size() <= 0)
		std::cerr << "WARNING: outdoorsmap.cc: _data.size() <= 0.\n";

	if (y < _data.size() && x < (_data[0]).size()) {
		(_data[y])[x] = icon_no;
		_modified = true;
		return 0;
	}
	return -1;
}

void OutdoorsMap::expand_map(int top, int bot, int right, int left)
{
  expand_map_data(top, bot, right, left);

  // Now check/adjust actions...
  for (auto curr_act = _actions.begin(); curr_act != _actions.end(); curr_act++) {
    unsigned old_x = (*curr_act)->get_x();
    unsigned old_y = (*curr_act)->get_y();
    
    int new_y = (int)old_y + 2*top;
    int new_x = (int)old_x + left;
    
    if (new_x >= 0  && new_y >= 0 && new_x < (int)width() && new_y < (int)height()) {
		(*curr_act)->set_x((unsigned)new_x);
		(*curr_act)->set_y((unsigned)new_y);
    }
    else {
    	// delete *curr_act;
      _actions.erase(curr_act);
      curr_act--;
    }
  }
}
