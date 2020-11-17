/*                                                                                                                                                                                                                                                                                                                                              * gWM.hpp                                                                                                                                                                                                                                                                                                                                   *  * Copyright (c) 2020 Yasuo Tabei All Rights Reserved.                                                                                                                                                                                                                                                                                          *                                                                                                                                                                                                                                                                                                                                              * This program is free software: you can redistribute it and/or modify                                                                                                                                                                                                                                                                         * it under the terms of the GNU General Public License as published by                                                                                                                                                                                                                                                                         * the Free Software Foundation, either version 3 of the License, or                                                                                                                                                                                                                                                                            * (at your option) any later version.                                                                                                                                                                                                                                                                                                          *                                                                                                                                                                                                                                                                                                                                              * This program is distributed in the hope that it will be useful,                                                                                                                                                                                                                                                                              * but WITHOUT ANY WARRANTY; without even the implied warranty of                                                                                                                                                                                                                                                                               * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                                                                                                                                                                                                                                                                                * GNU General Public License for more details.                                                                                                                                                                                                                                                                                                 *                                                                                                                                                                                                                                                                                                                                              * You should have received a copy of the GNU General Public License                                                                                                                                                                                                                                                                            * along with this program.  If not, see <http://www.gnu.org/licenses/>.                                                                                                                                                                                                                                                                        */

#ifndef _GWM_HPP_
#define _GWM_HPP_

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <stack>
#include <map>
#include <set>
#include <cmath>
#include <cstdlib>
#include <stdint.h>
#include <ctime>
#include <algorithm>
#include "rank9sel.h"
#include "WLKernel.hpp"

class gWM {
private:
  void     readDB(const char *fname);
  void     save(std::ostream &os);
  void     load(std::istream &is);
  void     inspectDepth(const std::vector<uint32_t> &cids);
  void     buildWaveletMatrix(std::vector<uint32_t> &cids);
  void     buildInvertedIndex(std::vector<std::vector<uint32_t> > &invertedIndex);
  void     buildRankDictionary();
  void     buildInitialIntervals(const std::vector<std::vector<uint32_t> > &invertedIndex);
  void     converter(const std::vector<std::vector<uint32_t> > &invertedIndex, std::vector<uint32_t> &cids);
  uint64_t getBit(uint64_t val, uint64_t d);
  void     makeBit(const std::vector<uint32_t> &v, uint64_t d, std::vector<uint64_t> &bits);
  void     rangeSearch(std::vector<std::pair<uint64_t, uint64_t> > &ranges, std::vector<std::pair<uint32_t, float> > &ids);
  void     search(std::vector<uint64_t> &query, std::vector<std::pair<uint32_t, float> > &ids);
  bool     pruning(const std::vector<std::pair<uint64_t, uint64_t> > &ranges);
  float    calcSimilarity(const std::vector<std::pair<uint64_t, uint64_t> > &ranges, uint32_t bit);
  void     setZeros(const std::vector<uint32_t> &cids);
  void     initMatrix(uint64_t width);
public:
  void     constructor(const char *fname, const char *oname, int _iteration);
  void     searcher(const char *index, const char *name, float _kernelThreshold);

  size_t   getVnumsSize() {
    return  (vnums.size() * 32)/8;
  }
  uint64_t getByte() {
    uint64_t memory = 0;
    for (size_t i = 0; i < bits.size(); ++i) {
      memory += bits[i].size() * sizeof(uint64_t);
      memory += dic[i]->bit_count()/8;
    }
    memory += zeros.size() * sizeof(uint64_t);
    memory += vnums.size() * sizeof(uint32_t);
    return memory;
  }
private:
  uint32_t                                    iteration;
  float                                       kernelThreshold;
  float                                       kernelThreshold2;
  float                                       lowerBound;
  float                                       upperBound;
  uint64_t                                    depth;
  std::vector<std::pair<uint64_t, uint64_t> > intervalIndex;
  GRAPHKERNEL::WLKernel                       graphKernel;
  size_t                                      qSize;
  std::vector<uint32_t>                       vnums;
  std::vector<std::vector<uint64_t> >         bits;
  std::vector<rank9sel*>                      dic;
  std::vector<uint64_t>                       zeros;
};

#endif // _GWM_HPP_
