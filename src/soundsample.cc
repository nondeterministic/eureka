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

#include <string>
#include <iostream>
#include <algorithm>

#include <boost/filesystem/path.hpp>

#include <SDL_mixer.h>

#include "world.hh"
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
	_chan = -1;   // TODO: is this a good init value? -1 finds next best free channel...
	boost::filesystem::path samples_path((std::string)DATADIR);
	samples_path = samples_path / PACKAGE_NAME / "data" / World::Instance().get_name() / "sound";
	boost::filesystem::path file = samples_path / "walk.wav";
	walk_wav = Mix_LoadWAV(file.c_str());
	file = samples_path /  "hit.wav";
	hit_wav = Mix_LoadWAV(file.c_str());
	file = samples_path / "foe_hit.wav";
	foe_hit_wav = Mix_LoadWAV(file.c_str());
	other_wav = NULL;
	_vol = sample_volume;
	_initialised = true;
}

SoundSample::~SoundSample()
{
	std::cout << "~Sample() for " << _filename << "\n";

	if (_chan != -1) // Otherwise ALL channels are halted!
		Mix_HaltChannel(_chan);
	Mix_FreeChunk(walk_wav);
	Mix_FreeChunk(hit_wav);
	Mix_FreeChunk(foe_hit_wav);
	if (other_wav != NULL)
		Mix_FreeChunk(other_wav);
}

// Toggle all audio

void SoundSample::toggle()
{
	if (!_initialised)
		init();

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

void SoundSample::play(int loop, int volume)
{
	if (!_initialised)
		init();

	play(_filename, loop, volume);
}

// If loop is negative, loop infinitely.  If >0, loop n times.

void SoundSample::play(std::string filename, int loop, int volume)
{
	if (!_initialised)
		init();

	_filename = filename;
	set_volume(volume);
	boost::filesystem::path filepath(filename);
	other_wav = Mix_LoadWAV(filepath.string().c_str());
	play_chunk(other_wav, loop);
	_audio_on = true;
}

// Play some standard, predefined samples (see enum above)

void SoundSample::play_predef(SampleType t, int loop, int volume)
{
	if (!_initialised)
		init();

	set_volume(volume);
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
	if (!_initialised)
		init();

	_vol = std::max(0, std::min(128, v));
}

void SoundSample::set_channel(int c)
{
	if (!_initialised)
		init();

	_chan = c;
}

// If loop is negative, loop infinitely.  If >0, loop n times.

void SoundSample::play_chunk(Mix_Chunk* wav, int loop)
{
	if (!_initialised)
		init();

	if (wav != NULL) {
		_audio_on = true;
		_chan = Mix_PlayChannel(_chan, wav, loop);
		Mix_Volume(_chan, _vol);
		std::cout << "INFO: soundsample.cc: Playing sound chunk on channel: " << _chan << ": " << _filename << "\n";
	}
	else
		std::cerr << "WARNING: soundsample.cc: Cannot play file '" << _filename << "'.\n";
}

void SoundSample::stop()
{
	if (_chan != -1) {
		Mix_ExpireChannel(_chan, 200);
		_audio_on = false;
		_initialised = false;
		_chan = -1;
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
