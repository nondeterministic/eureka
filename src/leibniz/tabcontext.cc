#include <gtkmm.h>
#include <iostream>
#include "tabcontext.hh"

TabContext::TabContext()
{
  _xoffset = 0;
  _yoffset = 0;
  _icon_brush_number = 0;
}

TabContext::~TabContext()
{
  std::cout << "~TabContext()" << std::endl;
}

TabContext::TabContext(Gtk::Box* newbox)
{
  _box = newbox;
  _xoffset = 0;
  _yoffset = 0;
  _icon_brush_number = 0;
}

Gtk::Box* TabContext::get_box() const
{
  return _box;
}

void TabContext::set_box(Gtk::Box* box)
{
  _box = box;
}

// This offset is counted in terms of icons, not pixel!

unsigned TabContext::get_xoffset() const
{
  return _xoffset;
}

// This offset is counted in terms of icons, not pixel!

unsigned TabContext::get_yoffset() const
{
  return _yoffset;
}

void TabContext::set_xoffset(unsigned x)
{
  _xoffset = x;
}

void TabContext::set_yoffset(unsigned y)
{
  _yoffset = y;
}

unsigned TabContext::get_icon_brush_no(void) const
{
  return _icon_brush_number;
}

void TabContext::set_icon_brush_no(unsigned number)
{
  _icon_brush_number = number;
}
