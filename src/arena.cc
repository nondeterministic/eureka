#include <string>
#include <iostream>
#include <memory>
#include "arena.hh"
#include "world.hh"
#include "map.hh"
#include "hexarena.hh"
#include "squarearena.hh"
#include "indoorsicons.hh"
#include "outdoorsicons.hh"

bool Offsets::operator== (Offsets a)
{
  return a.top == top && a.bot == bot && a.left == left && a.right == right;
}

bool Offsets::operator!= (Offsets a)
{
  return !(a.top == top && a.bot == bot && a.left == left && a.right == right);
}

Arena::Arena()
{
  _show_map = true; 
  _show_obj = true;
  _show_grid = true;
  _show_act = true;
  _party_is_moving = false;
  _sdl_surf = NULL;
}

Arena::~Arena()
{
}

std::shared_ptr<Arena> Arena::create(std::string type, std::string name)
{
  try {
    if (type == "indoors") {
      return std::make_shared<SquareArena>(World::Instance().get_map(name.c_str()));
    }
    else {
      return std::make_shared<HexArena>(World::Instance().get_map(name.c_str()));
    }
  }
  catch (MapNotFound& e) {
    std::cerr << "Creation of arena failed: " << e.print() << "\n";
    return NULL;
  }
  catch (std::exception& e) {
    std::cerr << "Creation of arena failed: " << e.what() << "\n";
    return NULL;
  }
}

void Arena::set_map(std::shared_ptr<Map> map)
{
	_map = map;
}

void Arena::set_show_map(bool status)
{
  _show_map = status;
}

void Arena::set_show_obj(bool status)
{
  _show_obj = status;
}

void Arena::set_show_act(bool status)
{
  _show_act = status;
}

void Arena::refresh(void)
{
  SDL_Flip(_sdl_surf);
}

void Arena::moving(bool m)
{
  _party_is_moving = m;
}

bool Arena::is_moving()
{
  return _party_is_moving;
}

void Arena::adjust_offsets(int top, int bot, int left, int right)
{
  _top_hidden += top;
  _bot_hidden += bot;
  _left_hidden += left;
  _right_hidden += right;
}

void Arena::set_offsets(unsigned top,  unsigned bot, unsigned left, unsigned right)
{
  _top_hidden = top;
  _bot_hidden = bot;
  _left_hidden = left;
  _right_hidden = right;
}
