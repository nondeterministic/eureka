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

#include <iostream>

#include "miscitem.hh"
#include "item.hh"

MiscItem::MiscItem()
{
	_has_mapobj = false;
}

MiscItem::~MiscItem()
{
}

MiscItem::MiscItem(const MiscItem& m): Item(m)
{
	_has_mapobj = m._has_mapobj;
	_mapObj = m._mapObj;
	std::cout << "DEEP COPY OF MISCITEM\n";
}

std::string MiscItem::get_lua_name()
{
	return "miscitems::" + name();
}

MapObj MiscItem::get_obj()
{
	if (!_has_mapobj)
		throw std::runtime_error("EXCEPTION: miscitem.cc: This item does not have a MapObj associated with it.");
	return _mapObj;
}

void MiscItem::set_obj(MapObj mo)
{
	_has_mapobj = true;
	_mapObj = mo;
}
