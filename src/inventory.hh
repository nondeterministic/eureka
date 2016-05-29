#ifndef INVENTORY_HH
#define INVENTORY_HH

#include "item.hh"
#include "eureka.hh"

#include <list>
#include <memory>
#include <vector>
#include <map>

class Inventory
{
private:
  std::map<std::string, std::vector<Item*>> _items;

public:
  Inventory();
  ~Inventory();
  int weight();
  Item* get_item(int);
  int how_many_at(int);
  std::vector<Item*>* get(int);
  std::map<std::string, int> list_wearables();
  std::map<std::string, int> list_all();
//  std::vector<line_tuple> to_line_tuples(std::map<std::string, int>&);
  void add(Item*);
  void remove(std::string);
  int remove_all(std::string);
  unsigned size();
  unsigned number_items();
  void add_all(Inventory&);
  void remove_all();
};

#endif
