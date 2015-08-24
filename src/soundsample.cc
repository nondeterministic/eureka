/*
 * File:   SoundSample.cc
 * Author: baueran
 *
 * Created on 1 January 2013, 2:15 PM
 */

#include <SDL_mixer.h>
#include <string>
#include <iostream>
#include <algorithm>

#include "soundsample.hh"
#include "config.h"

SoundSample::SoundSample()
{
  _filename = "";
  init();
}

SoundSample::SoundSample(std::string filename)
{
  _filename = filename;
  init();
}

void SoundSample::init()
{
  // _chan = -1 ; // TODO: is this a good init value? -1 is an error in
  _chan = 1 ; // TODO: is this a good init value? -1 is an error in
	           // Mix_PlayChannel, so it should be, i.e., any other
	           // value will be the actual channel.
  std::string file = (std::string)DATADIR + "/" + (std::string)SAMPLES_PATH + "walk.wav";
  walk_wav = Mix_LoadWAV(file.c_str());
  file = (std::string)DATADIR + "/" + (std::string)SAMPLES_PATH + "hit.wav";
  hit_wav = Mix_LoadWAV(file.c_str());
  file = (std::string)DATADIR + "/" + (std::string)SAMPLES_PATH + "foe_hit.wav";
  foe_hit_wav = Mix_LoadWAV(file.c_str());
  other_wav = NULL;
  _vol = 24;
}

SoundSample::~SoundSample()
{
  Mix_FreeChunk(walk_wav);
  Mix_FreeChunk(hit_wav);
  Mix_FreeChunk(foe_hit_wav);
  if (other_wav != NULL)
    Mix_FreeChunk(other_wav); 
  std::cout << "~SoundSample: " << _filename << "\n";   
}

// Toggle all audio

void SoundSample::toggle()
{
	if (_audio_on) {
		Mix_Pause(_chan);
		Mix_PauseMusic();
		_audio_on = false;
	}
	else {
		Mix_Resume(_chan);
		Mix_ResumeMusic();
		_audio_on = true;
	}
}

void SoundSample::play(int loop)
{
	play(_filename, loop);
}

void SoundSample::play(std::string filename, int loop)
{
	_filename = filename;
	std::cout << "PLAYING " << _filename << std::endl;
	other_wav = Mix_LoadWAV(_filename.c_str());
	play_chunk(other_wav, loop);
	_audio_on = true;
}

// Play some standard, predefined samples (see enum above)

void SoundSample::play(SampleType t, int loop)
{
	  switch (t) {
	  case WALK:
		play_chunk(walk_wav, loop);
		break;
	  case HIT:
		play_chunk(hit_wav, loop);
		break;
	  case FOE_HIT:
		play_chunk(foe_hit_wav, loop);
		break;
	  }
}

// 0 (min) - 128 (max)
void SoundSample::set_volume(int v)
{
	_vol = std::max(0, std::min(128, v));
}

void SoundSample::set_channel(int c)
{
	_chan = c;
}

void SoundSample::play_chunk(Mix_Chunk *wav, int loop)
{
	if (wav != NULL) {
		// Max volume is 128, we want half the volume for samples!
		Mix_Volume(_chan, _vol);
		_chan = Mix_PlayChannel(-1, wav, loop);
		_audio_on = true;
	}
	else
		std::cerr << "WARNING: Cannot play sample. Check soundsample.cc.\n";

	// std::cout << "Channel: " << _chan << "\n";
}

void SoundSample::stop()
{
  if (_chan != -1) {
    Mix_ExpireChannel(_chan, 200);
    _chan = -1;
	_audio_on = false;
  }
}

std::string SoundSample::filename()
{
  return _filename;
}

bool SoundSample::stopped()
{
  return _chan == -1;
}
