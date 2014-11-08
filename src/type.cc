//
//
// Copyright (c) 2010  Andreas Bauer <baueran@gmail.com>
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

#include "type.hh"
#include "sdltricks.hh"
#include <iostream>
#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include <map>
#include <utility>
#include <iostream>

using namespace std;

Type::Type() 
{
  // Is true, when text on grey background is active, i.e., to display a cursor, etc.
  _select = false; 

  _w = 8;   // Character width
  _h = 16;  // Character height

  _ptr_charset_surf = NULL;
}

Type::~Type()
{
  // std::cout << "~Type()\n";

  if (_ptr_charset_surf)
    SDL_FreeSurface(_ptr_charset_surf);

  for (map<int, SDL_Surface*>::iterator iter = _map_chars.begin(); iter != _map_chars.end(); iter++)
    SDL_FreeSurface(iter->second);
  _map_chars.clear();
}

void Type::col_printch(SDL_Surface* surf, int c, int x, int y, SDL_Color bgcol, SDL_Color fgcol)
{
  printch(surf, c, x, y, &bgcol, &fgcol);
}

void Type::printch(SDL_Surface* surf, int c, int x, int y, SDL_Color* bgcol, SDL_Color* fgcol)
{
  if (!surf)
    return;

  // Define default colours for printing.
  SDL_Color std_bgcol, std_fgcol;
  
  if (bgcol == NULL) {
    std_bgcol.r = 0; std_bgcol.g = 0; std_bgcol.b = 0;
    bgcol = &std_bgcol;
  }

  if (fgcol == NULL) {
    std_fgcol.r = 253; std_fgcol.g = 253; std_fgcol.b = 253;
    fgcol = &std_fgcol;
  }

  Uint32 amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  amask = 0x000000ff;
#else
  amask = 0xff000000;
#endif

  int ascii = c;
  SDL_Surface* charSurf = _map_chars[ascii];

  if (charSurf != NULL) {
    if (x == -1 && y == -1) {
      // Set background colour of printch.
      SDL_FillRect(surf, NULL, SDL_MapRGBA(charSurf->format, bgcol->r, bgcol->g, bgcol->b, amask));
      SDL_BlitSurface(charSurf, NULL, surf, NULL);
    }
    else {
      SDL_Rect dstRect;
      dstRect.x = x;
      dstRect.y = y;
      dstRect.w = _w;
      dstRect.h = _h;

      // Set background colour of printch.
      SDL_FillRect(surf, &dstRect, SDL_MapRGBA(charSurf->format, bgcol->r, bgcol->g, bgcol->b, amask));
      SDL_BlitSurface(charSurf, NULL, surf, &dstRect);
    }

    // Set foreground colour, if non-standard one is required.
    if (fgcol->r != 253 || fgcol->g != 253 || fgcol->b != 253) {
      SDL_Color old_fgcol;
      old_fgcol.r = 252; old_fgcol.g = 252; old_fgcol.b = 252;
      SDLTricks::Instance().replace_col(surf, old_fgcol, *fgcol, NULL);
    }
  }
  else
    std::cerr << "Warning: charSurf == NULL inside type.cc for ASCII code: " << c << std::endl;
}

int Type::char_width()
{
  return _w;
}

int Type::char_height()
{
  return _h;
}

void Type::set_select(bool new_select)
{
  _select = new_select;
}

void Type::toggle_select()
{
  _select = !_select;
}
