#include "party.hh"
#include <utility>
#include <iostream>

void Party::set_coords(int x, int y)
{
  this->x = x; this->y = y;
}

void Party::set_coords(std::pair<int, int> coords)
{
  this->x = coords.first; this->y = coords.second;
}

std::pair<int, int> Party::get_coords()
{
  std::pair<int, int> coords;
  coords.first = this->x; coords.second = this->y;
  return coords;
}

Party::Party()
{
  x = 0; y = 0;
  _gold = 0;
  _food = 0;
}

Party& Party::Instance()
{
  static Party inst;
  return inst;
}

int Party::gold()
{
  return _gold;
}

void Party::set_gold(int g)
{
  _gold = g;
}

int Party::food()
{
  return _food;
}

void Party::set_food(int g)
{
  _food = g;
}

void Party::store_state()
{
  prev_x = x;
  prev_y = y;
  prev_indoors = _indoors;
  prev_map_name = _map_name;
}

void Party::restore_state()
{
  x = prev_x;
  y = prev_y;
  _indoors = prev_indoors;
  _map_name = prev_map_name;
}

bool Party::indoors()
{
  return _indoors;
}

void Party::set_indoors(bool mode)
{
  _indoors = mode;
}

void Party::set_map_name(const char* name)
{
  _map_name = name;
}

std::string Party::map_name()
{
  return _map_name;
}

void Party::add_player(PlayerCharacter& player)
{
  _players.push_back(player);
  std::cout << "Added player " << player.name() << " to rooster.\n";
}

PlayerCharacter* Party::get_player(int number)
{
  int i = 0;
  for (auto player = Party::Instance().party_begin(); player != Party::Instance().party_end(); player++, i++)
    if (i == number)
      return &(*player);
  return NULL;
}

PlayerCharacter* Party::get_player(std::string name)
{
  for (auto player = Party::Instance().party_begin(); player != Party::Instance().party_end(); player++)
    if (player->name() == name)
      return &(*player);
  return NULL;
}

// Returns the number of alive party members
unsigned Party::party_alive()
{
  unsigned i = 0;
  for (auto player = Party::Instance().party_begin(); player != Party::Instance().party_end(); player++)
    if (player->hp() > 0)
      i++;
  return i;
}

std::vector<PlayerCharacter>::iterator Party::party_begin()
{
  return _players.begin();
}

std::vector<PlayerCharacter>::iterator Party::party_end()
{
  return _players.end();
}

int Party::party_size()
{
  return _players.size();
}

Inventory* Party::inventory()
{
  return &_inv;
}
