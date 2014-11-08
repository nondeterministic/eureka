#include "miniwin.hh"
#include "sdlwindow.hh"
#include "sdltricks.hh"
#include "ztatswin.hh"

#include <iostream>
#include <string>
#include <SDL.h>

using namespace std;

MiniWin::MiniWin()
{
  set_surface(SDLWindow::Instance().get_mini_win_SDL_surface());

  SDL_Rect rect;
  rect.x = SDLWindow::Instance().get_drawing_area_SDL_surface()->w + 2 * SDLWindow::Instance().frame_icon_size() - 6;
  rect.y = ZtatsWin::Instance().get_surface()->h + SDLWindow::Instance().frame_icon_size() * 2 - 6;
  rect.w = get_surface()->w;
  rect.h = get_surface()->h;
  set_position(rect);

  // Alter position of text relative to SDL surface
  _y_frame_offset = 1;
}

MiniWin& MiniWin::Instance()
{
  static MiniWin inst;
  return inst;
}

// Briefly highlights the window in red as a kind of alarm.
// Used for noting the fact that the enemy has been hit.

void MiniWin::alarm()
{
  SDL_Surface *s = _win; // SDLWindow::Instance().get_mini_win_SDL_surface();
  SDL_Surface *copy = SDL_ConvertSurface(s, s->format, s->flags);

  // cf. http://cboard.cprogramming.com/game-programming/99430-how-make-grayscale-filter.html
  SDL_LockSurface(s);
  for ( int x = 0; x != s->w; ++x ) {
    for ( int y = 0; y != s->h; ++y ) {
      Uint32 pixel = SDLTricks::Instance().getpixel(s,x,y);
      Uint8 r = 0;
      Uint8 g = 0;
      Uint8 b = 0;
      SDL_GetRGB(pixel, s->format, &r, &g, &b);
      r = g = b = (( r+g+b )/3);
      r = 0xFF;
      SDLTricks::Instance().putpixel(s,x,y, SDL_MapRGB(s->format, r,g,b));
    }
  }
  SDL_UnlockSurface(s);

  SDLWindow::Instance().blit_mini_win();
  SDL_Delay(300);
  SDL_BlitSurface(copy, NULL, s, NULL);
  SDL_FreeSurface(copy);
}

void MiniWin::surface_from_file(std::string filename)
{
  if (_tmp_surf != NULL)
    SDL_FreeSurface(_tmp_surf);

  if ((_tmp_surf = IMG_Load(filename.c_str())) == NULL)
    cerr << "Error: miniwin could not load surface.\n";

  SDL_BlitSurface(_tmp_surf, NULL, _win, NULL);
}
