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

#ifndef PLAYLIST_HH
#define PLAYLIST_HH

#include <list>
#include <memory>
#include <string>
#include "soundsample.hh"

class Playlist
{
public:
  void add_wav(std::string);
  void stop_wav(std::string);
  void clear();
  int size();
  bool has_sample(std::string);
  std::list< std::shared_ptr<SoundSample> >::iterator begin();
  std::list< std::shared_ptr<SoundSample> >::iterator end();
  void clear_stopped();
  void pause();

  static Playlist& Instance();
  
private:
  Playlist();
  std::list< std::shared_ptr<SoundSample> > _list;
  std::string samples_path;
};

#endif
