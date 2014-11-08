#ifndef ZTATSWIN_HH
#define ZTATSWIN_HH

#include <SDL.h>
#include <iostream>

#include "sdlwindowregion.hh"
#include "type.hh"
#include "charset.hh"
#include "simplicissimus.hh"

class ZtatsWin : public SDLWindowRegion
{
public:
  static ZtatsWin& Instance();
  void update_player_list();
  void highlight_lines(int, int);
  void unhighlight_lines(int, int);
  void unhighlight_all();
  void ztats_player(int);
  int select_player();
  void set_lines(std::vector<line_tuple>);
  void scroll(int player = -1);
  int select_item();
  std::vector<int> select_items();

protected:
  ZtatsWin();

  void build_ztats_player(int, int = 0);
  std::vector<line_tuple> lines;
  void swap_colours(int, int, SDL_Color, SDL_Color);

  SDL_Color highlight_colour, standard_bgcolour;
};

#endif
