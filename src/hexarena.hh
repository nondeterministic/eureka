#ifndef HEXARENA_HH
#define HEXARENA_HH

#include <SDL.h>
#include <utility>
#include <memory>
#include "map.hh"
#include "arena.hh"

enum SIGN {
  GRTR,  // >
  LOWR,  // <
  GEQ,   // >=
  LEQ    // <=
};

class HexArena : public Arena
{
public:
  HexArena(std::shared_ptr<Map>);
  ~HexArena();

  // Overriding virtual methods
  void show_grid();
  void show_map(int = 0, int = 0);
  std::shared_ptr<Map> get_map() const;
  void set_offset(int, int);
  Offsets move(int);
  Offsets determine_offsets();

  void set_SDL_surface(SDL_Surface*);
  SDL_Rect get_tile_coords(int, int) const;
  void screen_to_map(int, int, int&, int&);
  void map_to_screen(int, int, int&, int&);
  int put_tile(int, int, SDL_Surface* = NULL);
  int put_tile_hex(int, int, SDL_Surface* = NULL);
  void get_center_coords(int&, int&);
  std::pair<int, int> show_party(int = -1, int = -1);
  void update();
  bool adjacent(int, int, int, int);

  int get_screen_x(int) const;
  int get_screen_y(int) const;
  unsigned tile_size() const;

protected:
  Offsets offsets();
  bool in_los(int, int, int, int);
  int corner_tile_uneven_offset(void) const;

  // only used inside los:
  void swap(int&, int&);
  bool cond_cmp(int, int, int);

  int _corner_tile_uneven_offset;
  unsigned _width, _height;
  SDL_Surface* _clipped_surf;
};

#endif
