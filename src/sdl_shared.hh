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

// See https://blog.galowicz.de/2016/02/21/automatic_resource_release_with_sdl/

#ifndef __SDL_SHARED
#define __SDL_SHARED

#include <memory> // shared_ptr
#include <SDL2/SDL.h>

static void SDL_DelRes(SDL_Window   *r) { SDL_DestroyWindow(r);   }
static void SDL_DelRes(SDL_Renderer *r) { SDL_DestroyRenderer(r); }
static void SDL_DelRes(SDL_Texture  *r) { SDL_DestroyTexture(r);  }
static void SDL_DelRes(SDL_Surface  *r) { SDL_FreeSurface(r);     }

template <typename T>
std::shared_ptr<T> sdl_shared(T *t) {
    return std::shared_ptr<T>(t, [](T *t) { SDL_DelRes(t); });
}

#endif
