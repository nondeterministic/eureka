// ----------------------------------------------------------------
// leibniz
//
// Copyright (c) 2009  Andreas Bauer <baueran@gmail.com>
//
// This is not free software.  See COPYING for further information.
// ----------------------------------------------------------------

#include "obj.hh"

Obj::Obj()
{
  _icon = 0;
}

Obj::~Obj()
{
}

void Obj::set_icon(unsigned no)
{
  _icon = no;
}

unsigned Obj::get_icon(void)
{
  return _icon;
}
