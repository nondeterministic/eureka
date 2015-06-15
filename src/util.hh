#ifndef ____UTIL_HH
#define ____UTIL_HH

#include <vector>
#include <string>
#include <map>
#include "simplicissimus.hh"

namespace Util
{
  bool vowel(const char);
  std::vector<std::string> splitString(std::string, std::string);
  std::vector<line_tuple> to_line_tuples(std::map<std::string, int>&);
  std::string extract_name_from_ztats_list(std::string);

  namespace Private {
  	  std::vector<std::string>& splitString2(std::string, std::string, std::vector<std::string>&);
  }
}

#endif
