#ifndef ____UTIL_HH
#define ____UTIL_HH

#include <vector>
#include <string>
#include <map>
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
