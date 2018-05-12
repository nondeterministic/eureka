// mapconv - convert XPM image files into map XML files
// 
// (HOWTO: ./aout ~/test.xpm > output.xml)
//
// Copyright (c) 2011  Andreas Bauer <a@pspace.org>
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

#include <iostream>
#include <ctype.h>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

// Define the icons as they appear inside the map files, so that we
// can map the colours of the xpm file to these defined icons.
#define DEEP_WATER    0
#define WATER         1
#define FOREST        2
#define GRASS_STONES  3
#define GRASS        23
#define MOUNTAIN     28

bool has_space(string line)
{
  for (unsigned i = 0; i < line.length(); i++)
    if (isspace(line.at(i)))
      return true;
  return false;
}

int conv(const char* filename)
{
  string line;
  ifstream file(filename);
  
  if (file.is_open()) {
    cout << "<?xml version=\"1.0\"?>\n<map>\n  <name>test</name>\n  <outdoors>true</outdoors>\n  <data>\n";

    while (file.good()) {
      getline(file, line);
      
      // The data lines don't have white space characters in them.
      if (line.length() > 0 && line.at(0) == '"' && !has_space(line)) {
	line = line.substr(1, line.length() - 2);
	cout << "    <row>";
	for (unsigned i = 0; i < line.length(); i++) {
	  if (i > 0 && i%2 == 0) {
	    // Convert string to integer value
	    stringstream nr; nr << line.at(i) << line.at(i + 1);
	    int value; nr >> value;

	    // Convert xpm colours to icon numbers
	    switch (value) {
	    case 1:
	      value = DEEP_WATER;
	      break;
	    case 2:
	      value = GRASS;
	      break;
	    case 8:
	      value = WATER;
	      break;
	    case 11:
	      value = MOUNTAIN;
	      break;
	    case 27:
	      value = FOREST;
	      break;
	    case 37:
	      value = GRASS_STONES;
	      break;
	    }

	    cout << value;
	    // It is important to not have a trailing white space
	    // character, or the XML parser crashes.  (Weird, if you
	    // ask me.)
	    if (i < line.length() - 2)
	      cout << " ";
	  }
	}
	cout << "</row>" << endl;
      }
    }

    cout << "  </data>\n</map>" << endl;
    file.close();
  }

  return 0;
}

int main (int argc, char* argv[])
{
  conv(argv[1]);
}
