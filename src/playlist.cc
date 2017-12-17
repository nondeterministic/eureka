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

#include "playlist.hh"
#include <algorithm>
#include <iostream>
#include <string>
#include <memory>
#include <SDL2/SDL.h>

Playlist::Playlist()
{
}

Playlist& Playlist::Instance()
{
	static Playlist inst;
	return inst;
}

// Everything that is added to the playlist is automatically played.

void Playlist::add_wav(std::string filename, int volume)
{
	if (has_sample(filename))
		return;

	std::shared_ptr<SoundSample> sample(new SoundSample(filename));
	_list.push_front(sample);
	(*_list.begin())->set_loop(-1); // Loop infinitely!
	(*_list.begin())->play();
}

bool Playlist::has_sample(std::string filename)
{
	for (auto it = _list.begin(); it != _list.end(); it++)
		if ((*it)->filename() == filename)
			return true;
	return false;
}

void Playlist::stop_wav(std::string filename)
{
	for (auto it = _list.begin(); it != _list.end(); it++) {
		if ((*it)->filename() == filename) {
			(*it)->stop();
			return;
		}
	}
	std::cerr << "WARNING: playlist.cc: Tried to stop no longer needed sample " << filename << ", but failed.\n";
}

void Playlist::clear()
{
	for (auto it = _list.begin(); it != _list.end(); it++)
		(*it)->stop();
	_list.clear();
}

bool is_stopped(std::shared_ptr<SoundSample> ptr)
{
	return ptr->stopped();
}

void Playlist::clear_stopped()
{
	_list.erase(remove_if(_list.begin(), _list.end(), is_stopped), _list.end());
}

std::list< std::shared_ptr<SoundSample> >::iterator Playlist::begin()
{
	return _list.begin();
}

std::list< std::shared_ptr<SoundSample> >::iterator Playlist::end()
{
	return _list.end();
}

int Playlist::size()
{
	return _list.size();
}

void Playlist::pause()
{

}
