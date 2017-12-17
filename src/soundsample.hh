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

/// Helper for playing short sound samples.

class SoundSample
{
public:
  SoundSample();
  SoundSample(std::string);
  virtual ~SoundSample();

  void toggle();
  void stop();
  void set_volume(int);
  void set_loop(int);
  void play();
  void play(std::string);
  void play_predef(SampleType t);
  bool stopped();
  std::string filename();

  static int const default_loop  = 0;
  static int const sample_volume = 100;
  static int const music_volume  = 182;

protected:
  Mix_Music*  _music_ogg;
  Mix_Chunk*  _other_wav;
  Mix_Chunk*  _walk_wav;
  Mix_Chunk*  _hit_wav;
  Mix_Chunk*  _foe_hit_wav;
  std::string _filename;
  int  _chan;
  int  _vol;
  int  _loop;
  bool _audio_on;

  void play_chunk(Mix_Chunk*, int = default_loop);
  void play_music(Mix_Music*, int = -1);
  // void set_channel(int);
};

/// SoundSample is more or less meant to play short samples only.
/// Long music pieces should be handled in a specialised variant of it:
///
/// (Technically speaking, this class is the same as its base, but makes sure
/// all long tracks use the same channel, so that only ever 1 song can play
/// at most at a time.)

//class SoundSampleSong : public SoundSample
//{
//public:
//	SoundSampleSong();
//	SoundSampleSong(std::string);
//};

#endif	/* SOUNDSAMPLE_HH */
