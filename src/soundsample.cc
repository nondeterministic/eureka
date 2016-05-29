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
	// _chan = -1 ; // TODO: is this a good init value? -1 is an error in
	_chan = 1 ;  // TODO: is this a good init value? -1 is an error in
	             // Mix_PlayChannel, so it should be, i.e., any other
	             // value will be the actual channel.
	boost::filesystem::path samples_path((std::string)DATADIR);
	samples_path = samples_path / PACKAGE_NAME / "data" / World::Instance().get_name() / "sound";
	boost::filesystem::path file = samples_path / "walk.wav";
	walk_wav = Mix_LoadWAV(file.c_str());
	file = samples_path /  "hit.wav";
	hit_wav = Mix_LoadWAV(file.c_str());
	file = samples_path / "foe_hit.wav";
	foe_hit_wav = Mix_LoadWAV(file.c_str());
	other_wav = NULL;
	// _vol = 24;
	_vol = 128; // TODO: I used to think it a good idea to make background noises more quiet (ie vol = 24) Not so sure anymore...
}

SoundSample::~SoundSample()
{
	Mix_FreeChunk(walk_wav);
	Mix_FreeChunk(hit_wav);
	Mix_FreeChunk(foe_hit_wav);
	if (other_wav != NULL)
		Mix_FreeChunk(other_wav);
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
	boost::filesystem::path filepath(filename);
	other_wav = Mix_LoadWAV(filepath.string().c_str());
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
		std::cerr << "WARNING: Cannot play file '" << _filename << "'. Check soundsample.cc.\n";
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
