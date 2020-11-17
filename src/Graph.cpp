/*                                                                                                                                                                                                                    * Graph.cpp                                                                                                                                                                                                          * Copyright (c) 2020 Yasuo Tabei All Rights Reserved.                                                                                                                                                                *                                                                                                                                                                                                                    * This program is free software: you can redistribute it and/or modify                                                                                                                                               * it under the terms of the GNU General Public License as published by                                                                                                                                               * the Free Software Foundation, either version 3 of the License, or                                                                                                                                                  * (at your option) any later version.                                                                                                                                                                                *                                                                                                                                                                                                                    * This program is distributed in the hope that it will be useful,                                                                                                                                                    * but WITHOUT ANY WARRANTY; without even the implied warranty of                                                                                                                                                     * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                                                                                                                                                      * GNU General Public License for more details.                                                                                                                                                                       *                                                                                                                                                                                                                    * You should have received a copy of the GNU General Public License                                                                                                                                                  * along with this program.  If not, see <http://www.gnu.org/licenses/>.                                                                                                                                              */


#include "WLKernel.hpp"

using namespace std;

namespace GRAPHKERNEL {

template <class T, class Iterator> 
void tokenize (const char *str, Iterator iterator) 
{
  std::istrstream is (str, std::strlen(str));
  std::copy (std::istream_iterator <T> (is), std::istream_iterator <T> (), iterator);
}

int Graph::read(ifstream &ifs) {
  vector<string> result;
  char line[1024];
  clear();
  while (true) {
    unsigned int pos = ifs.tellg();
    if (!ifs.getline(line, 1024)) break;
    if (strlen(line) == 0) break;
    result.clear();
    tokenize<string>(line, back_inserter(result));
    if (result.empty()) {
      // do nothing
    } else if (result[0] == "t") {
      if (!empty()) {
	ifs.seekg(pos, ios_base::beg);
	break;
      }
    } else if (result[0] == "v" && result.size() >= 3) {
      unsigned int id = atoi(result[1].c_str());
      this->resize(id + 1);
      (*this)[id].vlabel = atoi(result[2].c_str());
      //      cout << id << " " << (*this)[id].vlabel << endl;
    } else if (result[0] == "e" && result.size() >= 4) {
      int from   = atoi(result[1].c_str());
      int to     = atoi(result[2].c_str());
      int elabel = atoi(result[3].c_str());
      if ((int)size() <= from || (int)size() <= to) {
	cerr << "Format Error: define lists before edges" << endl;
	return 1;
      }
      (*this)[from].push(from, to, elabel);
      (*this)[to].push(to, from, elabel);
    }
  }

  return 0;
}

}
