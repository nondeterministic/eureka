#include "ztatswincontentprovider.hh"
#include "ztatswin.hh"
#include "eureka.hh"

#include <iostream>
#include <vector>
#include <sstream>

using namespace std;

ZtatsWinContentProvider::ZtatsWinContentProvider()
{
}

/// Can be called multiple times in succession to load multiple pages of content that can be cycled with left-/right-arrow-keys.

void ZtatsWinContentProvider::add_content_page(std::vector<StringAlignmentTuple> single_page_content_tuples)
{
	_content_pages.push_back(single_page_content_tuples);
}

const vector<vector<StringAlignmentTuple>>& ZtatsWinContentProvider::get_pages()
{
	return _content_pages;
}

std::string ZtatsWinContentProvider::to_string()
{
	std::stringstream ss;

	for (auto content_page: _content_pages)
		for (auto line: content_page)
			ss << line.get<0>() << "\n"; // We ignore alignment in string output.

	return ss.str();
}
