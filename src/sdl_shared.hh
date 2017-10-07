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
