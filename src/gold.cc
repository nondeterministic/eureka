#include "gold.hh"
#include "item.hh"

Gold::Gold()
{
  _name = "gold coin";
  _plural_name = "gold coins";
  _weight = 0;
}

/*
std::string Gold::luaName()
{
	return ""; // "shields::" + name();
}
*/


Gold::Gold(const Gold& g): Item(g)
{
  _name = "gold coin";
  _plural_name = "gold coins";
  _weight = 0;
}
