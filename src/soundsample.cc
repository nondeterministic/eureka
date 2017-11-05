// This source file is part of eureka
//
// Copyright (c) 2007-2017  Andreas Bauer <baueran@gmail.com>
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

#include <string>
#include <iostream>
#include <algorithm>

#include <boost/filesystem/path.hpp>
#include <boost/algorithm/string/case_conv.hpp> // to_upper

#include <SDL_mixer.h>

#include "world.hh"
#include "soundsample.hh"
#include "config.h"

SoundSample::SoundSample() : SoundSample("") {};

SoundSample::SoundSample(std::string filename)
{
	_filename = filename;
	_chan = -1; // Select first free channel.

	boost::filesystem::path samples_path((std::string)DATADIR);
	samples_path = samples_path / PACKAGE_NAME / "data" / World::Instance().get_name() / "sound";
	boost::filesystem::path file = samples_path / "walk.wav";
	_walk_wav = Mix_LoadWAV(file.c_str());
	file = samples_path /  "hit.wav";
	_hit_wav = Mix_LoadWAV(file.c_str());
	file = samples_path / "foe_hit.wav";
	_foe_hit_wav = Mix_LoadWAV(file.c_str());
	_other_wav = NULL;
	_music_ogg = NULL;

	_vol = sample_volume;
	_audio_on = true;
	_loop = 0;
}

SoundSample::~SoundSample()
{
	std::cout << "~Sample() for " << _filename << "\n";

	if (_chan != -1) // Otherwise ALL channels are halted for -1!
		Mix_HaltChannel(_chan);

	Mix_FreeChunk(_walk_wav);
	Mix_FreeChunk(_hit_wav);
	Mix_FreeChunk(_foe_hit_wav);
	Mix_FreeMusic(_music_ogg);
	if (_other_wav != NULL)
		Mix_FreeChunk(_other_wav);
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

void SoundSample::play()
{
	play(_filename);
}

// If loop is negative, loop infinitely.  If >= 0, loop n + 1 times.

void SoundSample::play(std::string filename)
{
	std::string filename_upper = boost::to_upper_copy(filename);

	if (filename_upper.find(".WAV") != std::string::npos) {
		_filename = filename;
		boost::filesystem::path filepath(filename);
		_other_wav = Mix_LoadWAV(filepath.c_str());
		play_chunk(_other_wav);
	}
	else if (filename_upper.find(".OGG") != std::string::npos) {
		_filename = filename;
		boost::filesystem::path filepath(filename);
		_music_ogg = Mix_LoadMUS(filepath.c_str());
		play_music(_music_ogg);
	}
	else {
		std::cout << "ERROR: soundsample.cc: Do recognise sound format of file: " << filename << ".\n";
		return;
	}

	_audio_on = true;
}

// Play some standard, predefined samples (see enum above)

void SoundSample::play_predef(SampleType t)
{
	switch (t) {
	case WALK:
		play_chunk(_walk_wav);
		break;
	case HIT:
		play_chunk(_hit_wav);
		break;
	case FOE_HIT:
		play_chunk(_foe_hit_wav);
		break;
	}
}

// 0 (min) - 128 (max)
void SoundSample::set_volume(int v)
{
	_vol = std::max(0, std::min(128, v));
}

//void SoundSample::set_channel(int c)
//{
//	_chan = c;
//}

void SoundSample::set_loop(int loop)
{
	_loop = loop;
}

// If loop is negative, loop infinitely.  If >= 0, loop n + 1 times.

void SoundSample::play_chunk(Mix_Chunk* wav, int loop)
{
	if (wav != NULL) {
		_audio_on = true;
		_chan = Mix_PlayChannel(_chan, wav, loop);
		Mix_Volume(_chan, _vol);
		_chan = -1; // Reset channel so the next time, the next free channel is used.
		// std::cout << "INFO: soundsample.cc: Playing sound chunk on channel: " << _chan << ": " << _filename << "\n";
	}
	else
		std::cerr << "WARNING: soundsample.cc: Cannot play file '" << _filename << "'.\n";
}

// If loop is negative, loop infinitely.  If >= 0, loop n + 1 times.

void SoundSample::play_music(Mix_Music* ogg, int loop)
{
	if (ogg != NULL) {
		_audio_on = true;
		_chan = Mix_PlayMusic(ogg, loop);
		Mix_Volume(_chan, _vol);
		// std::cout << "INFO: soundsample.cc: Playing music: " << _filename << ".\n";
	}
	else
		std::cerr << "WARNING: soundsample.cc: Cannot play music file '" << _filename << "'.\n";
}

void SoundSample::stop()
{
	if (_chan != -1) {
		Mix_ExpireChannel(_chan, 200);
		_audio_on = false;
		_chan = -1;
	}
}

std::string SoundSample::filename()
{
	return _filename;
}

/// Stopped != paused.  Stopped means, we can discard the sample, basically.
/// Either because it hasn't played yet at all, or because the sample is no longer
/// relevant, e.g., player walked out of radius.

bool SoundSample::stopped()
{
	return _chan == -1;
}


// *************************************************************************************************************
// ****************************** SOUNDSAMPLESONG **************************************************************
// *************************************************************************************************************

//SoundSampleSong::SoundSampleSong() : SoundSampleSong("") {};
//
//SoundSampleSong::SoundSampleSong(std::string filename) : SoundSample(filename)
//{
//	_chan = 15; // Music is always played on channel 15!
//	_vol = music_volume;
//	_loop = -1;
//}

// *************************************************************************************************************
// *************************************************************************************************************
// *************************************************************************************************************
