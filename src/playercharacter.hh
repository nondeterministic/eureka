//
//
// Copyright (c) 2005  Andreas Bauer <baueran@gmail.com>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
// USA.

#ifndef PLAYERCHARACTER_HH
#define PLAYERCHARACTER_HH

#include "race.hh"
#include "profession.hh"
#include "gamecharacter.hh"
#include <string>

enum PlayerState {
  NORMAL_STATE, 
  DRUNK_STATE, 
  ASLEEP_STATE, 
  POSSESSED_STATE, 
  PARALYSED_STATE
};

// TODO: Add invincibility/invisible, but one can be
// invincible/invisible and possessed, for example.

class PlayerCharacter : public GameCharacter
{
private:
  PROFESSION  _prof;
  int           _ep;

public:
  PlayerCharacter();
  PlayerCharacter(const char*, int hpm = 0, int spm = 0, int str = 0, 
		  int luck = 0, int dxt = 0, int wis = 0, int charr = 0,
		  int iq = 0, int end = 0, bool sex = true,
		  RACE = HUMAN, PROFESSION = FIGHTER);
  PROFESSION profession();
  void set_profession(PROFESSION);
  int ep();
  void inc_ep(int);
};

#endif
