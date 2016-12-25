#ifndef ZTATSWINCP_HH
#define ZTATSWINCP_HH

#include "eureka.hh"
#include <vector>

using namespace std;

class ZtatsWinContentProvider
{
public:
  ZtatsWinContentProvider();
  void add_content_page(vector<StringAlignmentTuple>);
  const vector<vector<StringAlignmentTuple>>& get_pages();
  std::string to_string();

protected:
  vector<vector<StringAlignmentTuple>> _content_pages;
};

#endif
