#include "tinywin.hh"
#include "sdlwindow.hh"
#include "miniwin.hh"
#include "ztatswin.hh"

#include <SDL.h>

using namespace std;

TinyWin::TinyWin()
{
  set_surface(SDLWindow::Instance().get_tiny_win_SDL_surface());

  SDL_Rect rect;
  rect.x = SDLWindow::Instance().get_drawing_area_SDL_surface()->w + 2 * SDLWindow::Instance().frame_icon_size() - 6;
  rect.y = MiniWin::Instance().get_surface()->h + 
    ZtatsWin::Instance().get_surface()->h +
    SDLWindow::Instance().frame_icon_size() * 3 - 6;
  rect.w = get_surface()->w;
  rect.h = get_surface()->h;
  set_position(rect);

  // Alter position of text relative to SDL surface
  _y_frame_offset = 0;
}

TinyWin& TinyWin::Instance()
{
  static TinyWin inst;
  return inst;
}
