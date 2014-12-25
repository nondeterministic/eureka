#ifndef ARENA_HH
#define ARENA_HH

#include <SDL.h>
#include <vector>
#include <memory>
#include <utility>
#include "map.hh"

enum LDIR {
  DIR_UP,
  DIR_DOWN,
  DIR_RIGHT,
  DIR_LEFT,
  DIR_LUP,
  DIR_LDOWN,
  DIR_RUP,
  DIR_RDOWN,
  DIR_NONE
};

class Offsets
{
public:
  int top, bot, left, right;
  bool operator== (Offsets);
  bool operator!= (Offsets);
};

class Arena
{
public:
  Arena();
  virtual ~Arena();

  // Does the return type need to be static to ensure only one object
  // is created?
  static std::shared_ptr<Arena> create(std::string, std::string);

  void adjust_offsets(int = 0, int = 0, int = 0, int = 0);
  void set_offsets(unsigned = 0, unsigned = 0, unsigned = 0, unsigned = 0);
  void set_show_map(bool = true);
  void set_show_obj(bool = true);
  void set_show_act(bool = true);
  void refresh();
  void moving(bool);
  bool is_moving();
  void set_map(std::shared_ptr<Map>);

  virtual void show_map(int = 0, int = 0) = 0;
  virtual std::shared_ptr<Map> get_map() const = 0;
  // Moves the map into a direction defined via integer argument, and
  // returns the new map offset.
  virtual Offsets move(int) = 0;
  // This function is usually called when the editor window has been
  // resized and the right and lower offsets need to be redetermined.
  virtual Offsets determine_offsets() = 0;
  virtual void set_SDL_surface(SDL_Surface*) = 0;

  // The following methods were specifically added for SqArena:
  virtual SDL_Rect get_tile_coords(int, int) const = 0;
  virtual int put_tile(int, int, SDL_Surface* = NULL) = 0;
  virtual unsigned tile_size() const = 0;
  virtual void get_center_coords(int&, int&) = 0;
  virtual std::pair<int, int> show_party(int = -1, int = -1) = 0;
  virtual void update() = 0;
  virtual void screen_to_map(int, int, int&, int&) = 0;
  virtual void map_to_screen(int, int, int&, int&) = 0;
  virtual bool adjacent(int, int, int, int) = 0;

protected:
  SDL_Surface* _sdl_surf;
  std::shared_ptr<Map> _map;
  bool _show_grid, _show_map, _show_obj, _show_act;  
  unsigned _top_hidden, _bot_hidden, _left_hidden, _right_hidden;
  bool _party_is_moving;
  std::vector<int> _drawn_icons;
};

#endif
