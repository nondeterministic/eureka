// ----------------------------------------------------------------
// leibniz
//
// Copyright (c) 2009  Andreas Bauer <baueran@gmail.com>
//
// This is not free software.  See COPYING for further information.
// ----------------------------------------------------------------

#ifndef __ICONPROPS_HH
#define __ICONPROPS_HH

#include <string>

//enum ICON_FLAGS {
//  FULLY_TRANS = 0,         // Fully transparent
//  SEMI_TRANS = 2,
//  NOT_TRANS = 4,           // Not transparent
//  WALK_FULLSPEED = 8,      // Fully passable
//  WALK_SLOW = 16,
//  WALK_NOT = 32,           // Impassable (by foot)
//  WATER = 64,
//  DEEP_WATER = 128,
//  POISON = 256,
//  ANIMATE = 512            // Human or animal
//};

enum ICON_TRANS {
	IT_FULLY = 0, IT_SEMI = 1, IT_NOT = 2
};

enum ICON_WALK {
	IW_FULL = 0, IW_SLOW = 1, IW_NOT = 2
};

class IconProps
{
public:
  IconProps();
  ~IconProps();
  void set_icon(unsigned);
  unsigned get_icon();
  void set_name(const char*);
  std::string get_name();
  void set_next_anim(int);
  int next_anim();
  std::string sound_effect();
  void set_sound_effect(std::string);
  void set_light_radius(int);
  int light_radius();
  bool is_walkable();
  std::string default_lua_name();
  void set_default_lua_name(std::string);

  ICON_TRANS _trans;
  ICON_WALK _is_walkable;

  
protected:
  unsigned _icon;
  std::string _name;
  std::string _default_lua_name;
  int _flags;
  int _next_anim;
  int _radius;
  std::string _seffect;
};

#endif
