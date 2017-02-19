#include "icons.hh"
#include "iconprops.hh"
#include <vector>
#include <iostream>

Icons::Icons()
{
}

Icons::~Icons()
{
  _icons_props.clear();
}

void Icons::reserve(unsigned size)
{
  IconProps iconprops;
  _icons_props.assign(size, iconprops);
}

void Icons::add_props(IconProps new_props)
{
  if (new_props.get_icon() < _icons_props.size())
    _icons_props[new_props.get_icon()] = new_props;
}

IconProps* Icons::get_props(int icon_no)
{
	try {
		if (icon_no > 0)
			return &_icons_props.at(icon_no);
	}
	catch (...) {
		std::cerr << "WARNING: icons.cc: get_props(" << icon_no << ") yields no sensible result.\n";
		return NULL;
	}
}
