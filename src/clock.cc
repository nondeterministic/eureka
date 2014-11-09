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

#include <utility>

#include "clock.hh"

Clock::Clock()
{
  h = 6;
  m = 55;
}

std::pair<int,int> Clock::time()
{
  return std::make_pair(h,m);
}

void Clock::set(int hours, int minutes)
{
  h = hours; m = minutes;
}

void Clock::inc(int minutes)
{
  if (m + minutes >= 60) {
    m = m + minutes - 60;
    
    if (h == 23)
      h = 0;
    else
      h++;
  }
  else
    m += minutes;
}

TOD Clock::tod()
{
  if (h >= 2 && h < 7)
    return EARLY_MORNING;
  else if (h >= 7 && h < 12)
    return MORNING;
  else if (h >= 12 && h < 14)
    return NOON;
  else if (h >= 14 && h < 16)
    return AFTERNOON;
  else if (h >= 16 && h < 21)
    return EVENING;
  else if (h >= 21 && h < 0)
    return NIGHT;
  else // if (h >= 0 && h < 2)
    return MIDNIGHT;
}

