#ifndef SDLWINDOW_HH
#define SDLWINDOW_HH

#include <string>
#include <vector>
#include <SDL.h>

class SDLWindow
{
protected:
  Uint32 rmask, gmask, bmask, amask;
  SDL_Surface* _win;
  SDL_Surface* _interior;
  SDL_Surface* _console;
  SDL_Surface* _ztats;
  SDL_Surface* _mini_win;
  SDL_Surface* _tiny_win;
  int _x, _y, _w, _h;
  std::vector<SDL_Surface*> _vec_frameicons;
  static const int _frame_icon_size = 16;

  SDLWindow();
  ~SDLWindow();

public:
  static SDLWindow& Instance (void);

  int init(int = 0, int = 0, int = 0, Uint32 = 0);
  void close();
  SDL_Surface* get_drawing_area_SDL_surface();
  SDL_Surface* get_console_SDL_surface();
  SDL_Surface* get_ztats_SDL_surface();
  SDL_Surface* get_mini_win_SDL_surface();
  SDL_Surface* get_tiny_win_SDL_surface();
  SDL_Surface* get_SDL_surface();
  int draw_frame(int, int);
  int blit_interior();
  int blit_console();
  int blit_ztats();
  int blit_mini_win();
  int blit_tiny_win();
  int scroll_console(int, int = 5);
  int create_console_surface();
  int create_ztats_surface();
  int create_mini_win_surface();
  int create_tiny_win_surface();
  int frame_icon_size();
};

class SurfaceNULL
{
protected: 
  std::string err;
public: 
  SurfaceNULL(std::string s) { err = s; }
  std::string print() const { return err; }
};

#endif
