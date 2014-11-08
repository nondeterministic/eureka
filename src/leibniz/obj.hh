// ----------------------------------------------------------------
// leibniz
//
// Copyright (c) 2009  Andreas Bauer <baueran@gmail.com>
//
// This is not free software.  See COPYING for further information.
// ----------------------------------------------------------------

#ifndef _OBJ_HH
#define _OBJ_HH

#include <string>

// enum OBJ_TYPE
//   {
//     OBJ_TYPE_WEAPON,
//     OBJ_TYPE_ARMOUR,
//     OBJ_TYPE_PERSON,
//     OBJ_TYPE_MONSTER,
//     OBJ_TYPE_GROUND,
//     OBJ_TYPE_WALL,
//     OBJ_TYPE_DOOR,
//     OBJ_TYPE_RING,
//     OBJ_TYPE_FOOD,
//     OBJ_TYPE_SCROLL,
//   };

class Obj
{
public:
  Obj();
  ~Obj();
  void set_icon(unsigned);
  unsigned get_icon();
  void set_name(const char*);
  std::string get_name();

protected:
  unsigned _icon;
  std::string _name;
};

#endif
