#ifndef __SQARENA_HH
#define __SQARENA_HH

#include <SDL.h>
#include <utility>
#include <memory>
#include "map.hh"
#include "arena.hh"

class SquareArena : public Arena
{
public:
  SquareArena(std::shared_ptr<Map>);
  ~SquareArena();

  // Overriding virtual methods
  void show_grid();
  void show_map();
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
  unsigned tile_size() const;
  bool adjacent(int, int, int, int);

protected:
  Offsets offsets();
  bool in_los(int, int, int, int);

  // only used inside los:
  void swap(int&, int&);

  int _corner_tile_uneven_offset;
  unsigned _width, _height;
  SDL_Surface* _clipped_surf;

private:
  int _water_anim, _party_anim;
};

#endif
