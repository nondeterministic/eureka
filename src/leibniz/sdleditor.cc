// leibniz
//
// Copyright (c) 2009  Andreas Bauer <baueran@gmail.com>
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

#include <string>
#include <iostream>
#include "sdleditor.hh"
#include "../world.hh"
#include "../map.hh"
#include "indoorssdleditor.hh"
#include "outdoorssdleditor.hh"

SDLEditor::SDLEditor()
{
  _show_map = true; 
  _show_obj = true;
  _show_grid = true;
  _show_act = true;
}

SDLEditor::~SDLEditor()
{
}

SDLEditor* SDLEditor::create(std::string type, std::string name)
{
  try {
    if (type == "indoors") 
      return new IndoorsSDLEditor(World::Instance().get_map(name.c_str()));
    return new OutdoorsSDLEditor(World::Instance().get_map(name.c_str()));
  }
  catch(...) {
    return NULL;
  }
}

void SDLEditor::set_show_map(bool status)
{
  _show_map = status;
}

void SDLEditor::set_show_obj(bool status)
{
  _show_obj = status;
}

void SDLEditor::set_show_act(bool status)
{
  _show_act = status;
}

void SDLEditor::refresh(void)
{
  SDL_Flip(_sdl_surf);
}
