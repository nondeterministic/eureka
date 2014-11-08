//
//
// Copyright (c) 2005  Andreas Bauer <baueran@gmail.com>
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

#ifndef CLOCK_HH
#define CLOCK_HH

#include <utility>

// Time of day
enum TOD {
  EARLY_MORNING,
  MORNING,
  NOON,
  AFTERNOON,
  EVENING,
  NIGHT,
  MIDNIGHT
};

class Clock
{
private:
  int h, m;

public:
  Clock();
  std::pair<int, int> time();
  void inc(int);
  TOD tod();
  void set(int, int);
};

#endif

