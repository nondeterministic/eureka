/* 
 * File:   SoundSample.hh
 * Author: baueran
 *
 * Created on 1 January 2013, 2:15 PM
 */

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
  virtual ~SoundSample();

  void play(int = 0);
  void stop();
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
};

#endif	/* SOUNDSAMPLE_HH */

