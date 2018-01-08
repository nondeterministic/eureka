// This source file is part of eureka
//
// Copyright (c) 2007-2018 Andreas Bauer <baueran@gmail.com>
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

#include <cctype>
#include <vector>
#include <string>
#include <map>
#include <cstddef>        // std::size_t
#include <iostream>

#include "util.hh"
#include "eureka.hh"

bool Util::vowel(const char c)
{
  return (toupper(c) == 'A' ||
          toupper(c) == 'E' ||
          toupper(c) == 'U' ||
          toupper(c) == 'I' ||
          toupper(c) == 'O');
}

// Splits a string as follows:
// If weapons::axe is s, then a vector containing [weapons],[axe] is returned.
// If weapons is s, then a vector containing [weapons] is returned.

std::vector<std::string> Util::splitString(std::string s, std::string delim)
{
	std::vector<std::string> result;
	return Util::Private::splitString2(s, delim, result);
}

std::vector<std::string>& Util::Private::splitString2(std::string s, std::string delim, std::vector<std::string>& result)
{
	if (s.length() == 0)
		return result;

	for (unsigned i = 0; i < s.length(); i++) {
		if (s[i] == delim[0] && s.substr(i, delim.length()) == delim) {
			result.push_back(s.substr(0, i));
			return Util::Private::splitString2(s.substr(i + delim.length()), delim, result);
		}
	}

	result.push_back(s);
	return result;
}

std::vector<StringAlignmentTuple> Util::to_StringAlignmentTuples(std::map<std::string, int>& selection)
{
  std::vector<StringAlignmentTuple> result;
  std::stringstream ss;
  int i = 1;

  for (auto ptr = selection.begin(); ptr != selection.end(); ptr++, i++) {
    ss << i << ") ";
    ss << ptr->first;
    if (ptr->second > 1)
      ss << " (" << ptr->second << ")";

    result.push_back(StringAlignmentTuple(ss.str(), LEFTALIGN));
    ss.str(""); ss.clear();
  }

  return result;
}

// In the stats list, one sees entries like "12) arrow (200)" meaning that selection 12 refers to 200 arrows.
// When one is interested only in the displayed name, arrow, then call this function here.  It will return arrow in this case.
// (Note: The input string can also be "1) arrow", i.e., without quantity!  So we differenatiate that case below.)

std::string Util::extract_name_from_ztats_list(std::string s)
{
	std::size_t f1 = s.find_first_of(')');
	std::size_t f2 = s.find_first_of('(');

	if (f1 != std::string::npos) {
		if (f2 != std::string::npos)
			return s.substr(f1 + 2, s.length() - f1 - 2);
		else
			return s.substr(f1 + 2);
	}

	return "";
}

std::string Util::capitalise_first_letter(std::string s)
{
	std::stringstream ss;

	for (unsigned i = 0; i < s.length(); i++)
		if (i == 0)
			ss << (char)toupper(s[i]);
		else
			ss << s[i];

	return ss.str();
}

std::string Util::spaces_to_underscore(std::string s)
{
	std::stringstream ss;

	for (unsigned i = 0; i < s.length(); i++)
		if (s[i] == ' ')
			ss << '_';
		else
			ss << s[i];

	return ss.str();
}
