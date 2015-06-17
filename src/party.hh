#ifndef PARTY_HH
#define PARTY_HH

#include <libxml++/libxml++.h>

#include <utility>
#include <string>
#include <vector>
#include <memory>

#include "playercharacter.hh"
#include "weapon.hh"
#include "inventory.hh"

class Party
{
protected:
  std::vector<PlayerCharacter> _players;
  int prev_x, prev_y;
  bool prev_indoors, _indoors;
  std::string prev_map_name, _map_name;
  Inventory _inv;
  int _gold, _food;
  int _guard;
  int _jlocks;

  Party();

public:
  // The absolute party coordinates on a map.  Use map->screen to get relative coords!
  int x, y;
  int rounds_intoxicated;
  bool is_resting;
  bool is_in_combat;

  // >0 if, e.g., torch is carried and lit.
  // int light_radius;
  
  static Party& Instance();
  int light_radius();
  void add_player(PlayerCharacter);
  PlayerCharacter* get_player(int);
  PlayerCharacter* get_player(std::string);
  std::vector<PlayerCharacter>::iterator party_begin();
  std::vector<PlayerCharacter>::iterator party_end();
  int party_size();
  unsigned party_alive();
  void set_guard(int);
  PlayerCharacter* get_guard();
  void set_guard(PlayerCharacter*);
  void unset_guard();
  void set_coords(int, int);
  void set_coords(std::pair<int, int>);
  std::pair<int, int> get_coords();
  void store_outside_coords();
  void restore_outside_coords();
  bool indoors();
  void set_indoors(bool);
  void set_map_name(const char*);
  std::string map_name();
  Inventory* inventory();
  int gold();
  void set_gold(int);
  int food();
  void set_food(int);
  std::string to_xml();
  int max_carrying_capacity();
  void rm_jimmylock();
  void add_jimmylock();
  int jimmylock_count();
};

#endif
