/*
 * WLKernel_HPP
 * Copyright (c) 2013 Yasuo Tabei All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE and * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _WLKERNEL_HPP_
#define _WLKERNEL_HPP_

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <cmath>
#include <cstring>
#include <string>
#include <iterator>
#include <fstream>
#include <strstream>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <set>
#include <stdint.h>
#include <time.h>
#include <algorithm>

namespace GRAPHKERNEL {
  
struct Edge {
  uint32_t from;
  uint32_t to;
  uint64_t elabel;
};

struct Vertex {
  uint64_t          vlabel;
  uint64_t          transLabel;
  std::vector<Edge> edge;
  void push(uint32_t from, uint32_t to, uint64_t elabel) {
    edge.resize(edge.size() + 1);
    edge[edge.size() - 1].from   = from;
    edge[edge.size() - 1].to     = to;
    edge[edge.size() - 1].elabel = elabel;
  }
  void clear() {
    vlabel=0; transLabel=0; edge.clear();
  }
};

class Graph: public std::vector<Vertex> {
 public:
  int read(std::ifstream &ifs);
  void getVertexLabels(std::vector<uint64_t> &vlabels) {
    if (this->size() <= 1U)
      return;
    vlabels.clear();
    vlabels.resize(this->size()-1);
    for (size_t i = 1; i < this->size(); i++)
      vlabels[i-1] = (*this)[i].vlabel;
  }
  
  void print() {
    for (size_t i = 1; i < this->size(); i++) {
      std::cout << i << " " << (*this)[i].vlabel << std::endl;
      /*
      for (size_t j = 0; j < (*this)[i].edge.size(); j++) {
	std::cout << j << " " << (*this)[i].edge[j].from << " " <<  (*this)[i].edge[j].to << " " << (*this)[i].edge[j].elabel << std::endl;
      }
      */
    }
    std::cout << std::endl;
  }
};

class WLKernel {
  typedef std::vector<Edge>::iterator edgeIterator;
public:
  uint32_t                        iteration;
  std::map<std::string, uint64_t> s2c;
  uint64_t                        counter;
  std::vector<Graph>              graphs;

  WLKernel() {};
  WLKernel(const WLKernel &gk);
  size_t size() { return graphs.size(); }
  
  int    readfile(const char *fname);
  int    save(std::ostream &os);
  int    load(std::istream &is);
  void   initialize();
  void   initializeLabels();
  void   initialTransfer();
  void   transferLabels();
  size_t getGraphSize() {
    return graphs.size();
  }
  
  void getVertexLabels(int i, std::vector<uint64_t> &vlabels) {
    graphs[i].getVertexLabels(vlabels);
  }
  void print() {
    for (size_t i = 0; i < graphs.size(); i++) {
      std::cout << "id:" << i << std::endl;
      graphs[i].print();
    }
    std::cout << std::endl;
  }

  size_t getValLabels() {
    return counter;
  }
};
}

#endif // _WLKERNEL_HPP_

