// This source file is part of eureka
//
// Copyright (c) 2007-2018 Andreas Bauer <baueran@gmail.com>
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

#include "service.hh"
#include "item.hh"
#include <iostream>

Service::Service()
{
  std::cout << "Service()\n";
  
  heal = 0;
  heal_poison = false;
  resurrect = false;
  print_after = "";
}

Service::~Service()
{
  std::cout << "~Service(): " << name() << "\n";
}

Service::Service(const Service& s): Item(s)
{
  heal = s.heal;
  heal_poison = s.heal_poison;
  resurrect = s.resurrect;
  print_after = s.print_after;
}

std::string Service::luaName()
{
  return "services::" + name();
}
