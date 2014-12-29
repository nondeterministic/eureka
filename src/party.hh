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

  Party();

public:
  // The absolute party coordinates on a map.  Use map->screen to get
  // relative coords!
  int x, y;
  
  static Party& Instance();
  void add_player(PlayerCharacter);
  PlayerCharacter* get_player(int);
  PlayerCharacter* get_player(std::string);
  std::vector<PlayerCharacter>::iterator party_begin();
  std::vector<PlayerCharacter>::iterator party_end();
  int party_size();
  unsigned party_alive();
  void set_coords(int, int);
  void set_coords(std::pair<int, int>);
  std::pair<int, int> get_coords();
  void store_state();
  void restore_state();
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
};

#endif
