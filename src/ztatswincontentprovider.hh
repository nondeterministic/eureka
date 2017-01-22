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

#ifndef ZTATSWINCP_HH
#define ZTATSWINCP_HH

#include "util.hh"

#include <vector>
#include <string>
#include <utility> // std::pair
#include <iostream>

using namespace std;

class ZtatsWinContentProvider
{
public:
	ZtatsWinContentProvider();
	void add_content_page(vector<StringAlignmentTuple>);
	const vector<vector<StringAlignmentTuple>>& get_pages();
	string to_string();
	void clear();

protected:
	vector<vector<StringAlignmentTuple>> _content_pages;
};

/// Selection of content (unlike display only) can only ever have a single content page to select from, not multiple ones.
/// So it's create_content_page() instead pf add_content_page(), which could have been called multiple times, and we do not want that.
///
/// Also, I ran into this one: http://stackoverflow.com/questions/8752837/undefined-reference-to-template-class-constructor
/// So I moved the entire implementation of the class into the header file.

template <class T>
class ZtatsWinContentSelectionProvider
{
public:
	ZtatsWinContentSelectionProvider()
	{
	}

	void add_content_page(vector<pair<StringAlignmentTuple, T>> page)
	{
		vector<StringAlignmentTuple> tmp_page_string_items;
		clear(); // Should not be necessary, but this way, we can overwrite existing content pages by letting the user call the function multiple times.

		for (auto page_item: page) {
			tmp_page_string_items.push_back(page_item.first);
			_linked_objects.push_back(page_item.second);
		}

		if (tmp_page_string_items.size() > 0 && _linked_objects.size() > 0)
			_basic_content_provider.add_content_page(tmp_page_string_items);
		else if (tmp_page_string_items.size() != _linked_objects.size())
			cerr << "ERROR: ztatswincontentprovider.cc: create_content_page() failed due to unequal size of items and objects.\n";
		else
			cout << "INFO: ztatswincontentprovider.cc: create_content_page() has both items and item objects of size 0. This is in all likelyhood OK and not a bug.\n";
	}

	vector<pair<StringAlignmentTuple, T>> get_page()
	{
		vector<pair<StringAlignmentTuple, T>> tmp_content_page;

		// Unlike normal ContentProvider, the selection one only supports exactly ONE page of content along with linked objects!
		if (_basic_content_provider.get_pages().size() == 1) {
			if (_basic_content_provider.get_pages()[0].size() != _linked_objects.size())
				cerr << "ERROR: ztatswincontentprovider.cc: Internal error: linked objects must be equally many as item strings (2).\n";
			else {
				for (unsigned i = 0; i < _basic_content_provider.get_pages()[0].size(); i++) {
					StringAlignmentTuple item_string = _basic_content_provider.get_pages()[0][i];
					tmp_content_page.push_back(pair<StringAlignmentTuple, T>(item_string, _linked_objects[i]));
				}
			}
		}

		return tmp_content_page;
	}

	vector<StringAlignmentTuple> get_page_strings_only()
	{
		vector<StringAlignmentTuple> tmp_content_page;

		// Unlike normal ContentProvider, the selection one only supports exactly ONE page of content along with linked objects!
		if (_basic_content_provider.get_pages().size() == 1) {
			for (unsigned i = 0; i < _basic_content_provider.get_pages()[0].size(); i++) {
				StringAlignmentTuple item_string = _basic_content_provider.get_pages()[0][i];
				tmp_content_page.push_back(item_string);
			}
		}

		return tmp_content_page;
	}

	string to_string()
	{
		return _basic_content_provider.to_string();
	}

	void clear()
	{
		_linked_objects.clear();
		_basic_content_provider.clear();
	}

protected:
	ZtatsWinContentProvider _basic_content_provider;
	vector<T>               _linked_objects;
};

#endif
