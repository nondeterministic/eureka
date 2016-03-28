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

#ifndef __SDLWINDOWREGION_HH
#define __SDLWINDOWREGION_HH

#include <iostream>
#include <SDL.h>

#include <string>

#include "simplicissimus.hh"
#include "charset.hh"
#include "sdlwindow.hh"

class SDLWindowRegion
{
protected:
  Charset font;
  SDL_Surface* _win;
  SDL_Surface* _last_surface;
  int _y_frame_offset, _x_frame_offset, _inter_line_padding;
  Uint32 rmask, gmask, bmask, amask;
  SDL_Rect _pos;

  SDLWindowRegion()
  {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif

    // Used for positioning characters inside the region
    _x_frame_offset = 2;
    _y_frame_offset = 4;
    _inter_line_padding = 2;
  }

  ~SDLWindowRegion()
  {
    if (_last_surface != NULL) {
      SDL_FreeSurface(_last_surface);
      _last_surface = NULL;
    }
  }

public:

  void set_position(SDL_Rect pos)
  {
    _pos = pos;
  }

  void set_surface(SDL_Surface* s)
  {
    _win = s;

    if (_win == NULL) {
      std::cerr << "ERROR: sdlwindowregion.hh: _win == NULL.\n";
      exit(EXIT_FAILURE);
    }

    _last_surface = SDL_CreateRGBSurface(SDL_HWSURFACE,
                                         _win->w,
                                         _win->h,
                                         32,
                                         rmask, gmask, bmask,
                                         0);
  }

  void display_surf(SDL_Surface* s)
  {
    // Save existing display for later
    save_surf();
    
    if (!s)
      std::cerr << "INFO: sdlwindowregion.hh: surface s is null (harmless).\n";
    
    if (SDL_BlitSurface(s, NULL, _win, NULL) != 0)
      std::cerr << "INFO: sdlwindowregion.hh: miniwin.cc::display_surf failed (harmless).\n";
  }

  SDL_Surface* get_surface()
  {
    return _win;
  }

  void printch(int x_pos, int y_pos, int c)
  {
    font.printch(get_surface(), c, x_pos + _x_frame_offset, y_pos + _y_frame_offset);
  }

  void print(int x_pos, int y_pos, const std::string s, Alignment align = LEFTALIGN)
  {
    for (unsigned i = 0, j = 0; i < s.length(); i++, j++) {
      if (align == LEFTALIGN)
        font.printch(get_surface(), s[i], x_pos + _x_frame_offset + j * font.char_width(), y_pos + _y_frame_offset);
      else if (align == RIGHTALIGN)
        print(get_surface()->w - s.length() * font.char_width() - _x_frame_offset, y_pos, s);
      else
        print((int)((get_surface()->w - s.length() * font.char_width()) / 2), y_pos, s);
    }

    blit();
  }

  void println(int line, const std::string s, Alignment align = LEFTALIGN)
  {
    if (line < get_surface()->h / font.char_height())
      print(0, line * font.char_height() + _inter_line_padding * line, s, align);
  }

  int blit()
  {
    SDLWindow& swin = SDLWindow::Instance();
    
    if (SDL_BlitSurface(_win, NULL, swin.get_SDL_surface(), &_pos) == 0) {
      SDL_Flip(swin.get_SDL_surface());
      return 0;
    }
    else
      return -1;
  }

  void save_surf()
  {
    SDL_BlitSurface(_win, NULL, _last_surface, NULL);  
  }

  void display_last()
  {
    SDL_BlitSurface(_last_surface, NULL, _win, NULL);
  }
  
  void clear()
  {
    SDL_FillRect(_win, NULL, SDL_MapRGB(_win->format, 0, 0, 0));
    blit();
  }

};

#endif
