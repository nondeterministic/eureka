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

enum ICON_FLAGS {
  FULLY_TRANS = 0,         // Fully transparent
  SEMI_TRANS = 2,
  NOT_TRANS = 4,           // Not transparent
  WALK_FULLSPEED = 8,      // Fully passable
  WALK_SLOW = 16,
  WALK_NOT = 32,           // Impassable (by foot)
  WATER = 64,
  DEEP_WATER = 128,
  POISON = 256,
  ANIMATE = 512            // Human or animal
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
  int flags() const;
  void set_flags(int);
  void add_flags(int);
  void rm_flags(int);
  void set_next_anim(int);
  int next_anim();
  std::string sound_effect();
  void set_sound_effect(std::string);
  
protected:
  unsigned _icon;
  std::string _name;
  int _flags;
  int _next_anim;
  std::string _seffect;
};

#endif
