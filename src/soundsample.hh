// This source file is part of eureka
//
// Copyright (c) 2007-2016  Andreas Bauer <baueran@gmail.com>
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

#ifndef STANDARDSOUNDSAMPLE_HH
#define	STANDARDSOUNDSAMPLE_HH

#include <SDL_mixer.h>

enum SampleType {
  WALK,
  HIT,
  FOE_HIT
};

class SoundSample {
public:
  SoundSample();
  SoundSample(std::string);
  ~SoundSample();

  void play(int = 0);
  void toggle();
  void stop();
  void set_volume(int);
  void set_channel(int);
  void play(SampleType t, int = 0);
  void play(std::string, int = 0);
  std::string filename();
  bool stopped();
  // void set_filename(std::string);

private:
  Mix_Chunk *other_wav;
  Mix_Chunk *walk_wav;
  Mix_Chunk *hit_wav;
  Mix_Chunk *foe_hit_wav;
  std::string _filename;
  void play_chunk(Mix_Chunk*, int = 0);
  void init();
  int _chan;
  int _vol;
  bool _audio_on;
};

#endif	/* SOUNDSAMPLE_HH */
