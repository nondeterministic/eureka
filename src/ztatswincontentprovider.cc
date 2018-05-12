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

#include "ztatswincontentprovider.hh"
#include "util.hh"

#include <iostream>
#include <vector>
#include <sstream>

using namespace std;

ZtatsWinContentProvider::ZtatsWinContentProvider()
{
}

/// Can be called multiple times in succession to load multiple pages of content that can be cycled with left-/right-arrow-keys.

void ZtatsWinContentProvider::add_content_page(vector<StringAlignmentTuple> single_page_content_tuples)
{
	_content_pages.push_back(single_page_content_tuples);
}

const vector<vector<StringAlignmentTuple>>& ZtatsWinContentProvider::get_pages()
{
	return _content_pages;
}

string ZtatsWinContentProvider::to_string()
{
	stringstream ss;

	for (auto content_page: _content_pages)
		for (auto line: content_page)
			ss << line.get<0>() << "\n"; // We ignore alignment in string output.

	return ss.str();
}

void ZtatsWinContentProvider::clear()
{
	for (auto page: _content_pages)
		page.clear();
	_content_pages.clear();
}
