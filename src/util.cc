#include <cctype>
#include <vector>
#include <string>
#include <map>

#include "util.hh"
#include "simplicissimus.hh"

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

std::vector<line_tuple> Util::to_line_tuples(std::map<std::string, int>& selection)
{
  std::vector<line_tuple> result;
  std::stringstream ss;
  int i = 1;

  for (auto ptr = selection.begin(); ptr != selection.end(); ptr++, i++) {
    ss << i << ") ";
    ss << ptr->first;
    if (ptr->second > 1)
      ss << " (" << ptr->second << ")";

    result.push_back(line_tuple(ss.str(), LEFTALIGN));
    ss.str(""); ss.clear();
  }

  return result;
}
