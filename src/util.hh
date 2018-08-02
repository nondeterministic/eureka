// This source file is part of eureka
//
// Copyright (c) 2007-2018 Andreas Bauer <a@pspace.org>
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

#ifndef ____UTIL_HH
#define ____UTIL_HH

#include <vector>
#include <string>
#include <map>

#include <lua.h>

#include "eureka.hh"

enum Alignment
{
	LEFTALIGN,
	CENTERALIGN,
	RIGHTALIGN
};

typedef boost::tuple<std::string, Alignment> StringAlignmentTuple;

namespace Util
{
	void strace();

	bool vowel(const char);
	std::vector<std::string> splitString(std::string, std::string);
	std::vector<StringAlignmentTuple> to_StringAlignmentTuples(std::map<std::string, int>&);
	std::string extract_name_from_ztats_list(std::string);
	std::string capitalise_first_letter(std::string);
	std::string spaces_to_underscore(std::string);

	namespace Private {
		std::vector<std::string>& splitString2(std::string, std::string, std::vector<std::string>&);
	}
}

#endif
