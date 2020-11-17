/*
 * gWM.cpp
 * Copyright (c) 2020 Yasuo Tabei All Rights Reserved.    
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "gWM.hpp"

using namespace std;
using namespace GRAPHKERNEL;

void gWM::buildInvertedIndex(vector<vector<uint32_t> > &invertedIndex) {
  graphKernel.initialize();
  vector<uint64_t> labels;
  size_t gSize = graphKernel.getGraphSize();
  vnums.clear();
  vnums.resize(gSize);

  graphKernel.initialTransfer();
  for (size_t id = 0; id < gSize; id++) {
    graphKernel.getVertexLabels(id, labels);
    sort(labels.begin(), labels.end());
    std::vector<uint64_t>::iterator new_end = std::unique(labels.begin(),labels.end());
    labels.erase(new_end, labels.end());
    vnums[id] += labels.size();
    for (size_t j = 0; j < labels.size(); j++) {
      if (invertedIndex.size() <= labels[j])
	invertedIndex.resize(labels[j] + 1);
      invertedIndex[labels[j]].push_back(id);
    }
  }
  for (size_t i = 0; i < iteration; i++) {
    graphKernel.transferLabels();
    for (size_t id = 0; id < gSize; id++) {
      graphKernel.getVertexLabels(id, labels);
      sort(labels.begin(), labels.end());
      std::vector<uint64_t>::iterator new_end = std::unique(labels.begin(),labels.end());
      labels.erase(new_end, labels.end());
      vnums[id] += labels.size();
      for (size_t j = 0; j < labels.size(); j++) {
	if (invertedIndex.size() <= labels[j])
	  invertedIndex.resize(labels[j] + 1);
	invertedIndex[labels[j]].push_back(id);
      }
    }
  }
}

void gWM::buildInitialIntervals(const vector<vector<uint32_t> > &invertedIndex) {
  uint64_t s = 0, e;
  intervalIndex.resize(invertedIndex.size());
  for (size_t i = 0; i < invertedIndex.size(); i++) {
    const vector<uint32_t> &ids = invertedIndex[i];
    e = s + ids.size() - 1;
    intervalIndex[i] = make_pair(s, e);
    s = e + 1;
  }
}

void gWM::converter(const vector<vector<uint32_t> > &invertedIndex, vector<uint32_t> &cids) {
  intervalIndex.resize(invertedIndex.size());
  for (size_t i = 0; i < invertedIndex.size(); i++) {
    const vector<uint32_t> &ids = invertedIndex[i];
    for (size_t i = 0; i < ids.size(); i++)
      cids.push_back(ids[i]);
  }
}

uint64_t gWM::getBit(uint64_t val, uint64_t d) {
  return ((val>>d) & 1ULL);
}

void gWM::inspectDepth(const std::vector<uint32_t> &cids) {
  for (int d = 31; d >= 0; --d) {
    for (size_t i = 0; i < cids.size(); ++i) {
      if (getBit(cids[i], d))  {
	depth = d + 1;
	return;
      }
    }
  }
  depth = 1;
}

inline float gWM::calcSimilarity(const vector<pair<uint64_t, uint64_t> > &ranges, uint32_t bit) {
  return (float)ranges.size()/(sqrt(qSize)*sqrt(vnums[bit]));
}

void gWM::rangeSearch(vector<pair<uint64_t, uint64_t> > &ranges, vector<pair<uint32_t, float> > &ids) {
  stack<pair<vector<pair<uint64_t, uint64_t> >, pair<uint32_t, uint32_t> > > stk;
  stk.push(make_pair(ranges, make_pair(0U, 0U)));

  while (!stk.empty()) {
    const vector<pair<uint64_t, uint64_t> > &tmpRanges = stk.top().first; 
    uint32_t d   = stk.top().second.first;
    uint32_t bit = stk.top().second.second;

    if (depth == d) {
      float sim;
      if ((sim = calcSimilarity(tmpRanges, bit)) >= kernelThreshold)
	ids.push_back(make_pair(bit, sim));

      stk.pop();
      continue;
    }
    else if (pruning(tmpRanges)) {
      stk.pop();
      continue;
    }
    
    rank9sel *rsdic = dic[d];
    vector<pair<uint64_t, uint64_t> > ranges0, ranges1;
    for (size_t i = 0; i < tmpRanges.size(); ++i) {
      const pair<size_t, size_t> &r = tmpRanges[i];
      uint64_t rs = rsdic->rank(r.first);
      uint64_t re = rsdic->rank(r.second + 1);
      if (rs < re)
	ranges1.push_back(make_pair(rs + zeros[d], re - 1 + zeros[d]));
      rs = r.first - rs; re = (r.second + 1) - re;
      if (rs < re)
	ranges0.push_back(make_pair(rs, re - 1));
    }
    stk.pop();
    stk.push(make_pair(ranges0, make_pair(d+1U, bit)));
    if ((bit | (1U << (depth - d - 1))) < vnums.size())
      stk.push(make_pair(ranges1, make_pair(d+1U, bit | (1U << (depth - d - 1)))));
  }
}


inline bool gWM::pruning(const vector<pair<uint64_t, uint64_t> > &ranges) {
  return (ranges.size() < lowerBound || upperBound < ranges.size());
}

void gWM::search(vector<uint64_t> &query, vector<pair<uint32_t, float> > &ids) {
  vector<pair<uint64_t, uint64_t> > ranges;

  for (size_t i = 0; i < query.size(); i++) {
    if (query[i] < intervalIndex.size()) 
      ranges.push_back(intervalIndex[query[i]]);
  }

  qSize      = query.size();
  lowerBound = float(qSize) * kernelThreshold2;
  upperBound = float(qSize) / kernelThreshold2;
  rangeSearch(ranges, ids);
}

void gWM::buildRankDictionary() {
  dic.resize(depth);
  for (uint64_t i = 0; i < depth; ++i) 
    dic[i] = new rank9sel(&bits[i][0], bits[i].size()*64);
}

void gWM::setZeros(const vector<uint32_t> &cids) {
  zeros.resize(dic.size());
  for (size_t i = 0; i < dic.size() - 1; ++i) 
    zeros[i] = cids.size() - dic[i]->rank(cids.size());
}

void gWM::searcher(const char *index, const char *name, float _kernelThreshold) {
  ifstream ifs(index);
  if (!ifs) {
    cerr << "cannot open:" << index << endl;
    exit(1);
  }
  cerr << "reading index file:" << index << endl;
  load(ifs);
  cerr << "end reading" << endl;
  ifs.close();

  cerr << "start building rank dictionary" << endl;
  buildRankDictionary();
  cerr << "end building rank dictionary" << endl;

  //  printDictionary();

  cerr << "start transferring labels" << endl;
  WLKernel queryGraphKernel(graphKernel);
  queryGraphKernel.readfile(name);
  size_t gSize = queryGraphKernel.getGraphSize();
  vector<uint64_t> labels;
  vector<vector<uint64_t> > qGraphs(gSize);

  queryGraphKernel.initialTransfer();
  for (size_t j = 0; j < gSize; j++) {
    labels.clear();
    queryGraphKernel.getVertexLabels(j, labels);
    for (size_t k = 0; k < labels.size(); k++)
      qGraphs[j].push_back(labels[k]);
  }
  for (size_t i = 0; i < iteration; i++) {
    queryGraphKernel.transferLabels();
    for (size_t j = 0; j < gSize; j++) {
      labels.clear();
      queryGraphKernel.getVertexLabels(j, labels);
      for (size_t k = 0; k < labels.size(); k++)
	qGraphs[j].push_back(labels[k]);
    }
  }
  for (size_t i = 0; i < qGraphs.size(); i++) {
    vector<uint64_t> &tmp = qGraphs[i];
    sort(tmp.begin(), tmp.end());
    std::vector<uint64_t>::iterator new_end = std::unique(tmp.begin(),tmp.end());
    tmp.erase(new_end, tmp.end());
    qGraphs[i] = tmp;
  }
  cerr << "end transferring labels" << endl;

  kernelThreshold  = _kernelThreshold;
  kernelThreshold2 = kernelThreshold*kernelThreshold;

  uint64_t resNum = 0;
  vector<double> times;
  for (size_t i = 0; i < gSize; i++) {
    vector<pair<uint32_t, float> > ids;
    double stime = clock();
    search(qGraphs[i], ids);
    double etime = clock();
    times.push_back(etime - stime);

    fprintf(stdout, "id:%ld ", i);
    for (size_t j = 0; j < ids.size(); j++)
      fprintf(stdout, "%d:%f ", ids[j].first, ids[j].second);
    fprintf(stdout, "\n");
    resNum += ids.size();
  }
  
  double total = 0.f;
  for (size_t i = 0; i < times.size(); i++)
    total += times[i];
  total /= CLOCKS_PER_SEC;
  
  double mean = total/(double)times.size();
  double dev  = 0;
  for (size_t i = 0; i < times.size(); i++) {
    dev += (times[i]/CLOCKS_PER_SEC - mean) * (times[i]/CLOCKS_PER_SEC  - mean);
  }
  dev = sqrt(dev/(double)(times.size()-1));

  fprintf(stdout, "cpu time (sec):%f\n", total);
  fprintf(stdout, "average cpu time (sec):%f\n", mean);
  fprintf(stdout, "dev cpu time:%f\n", dev);
  double averageNum = (double)resNum/(double)gSize;
  fprintf(stdout, "average # of outputs:%f\n", averageNum);
}

void gWM::load(istream &is) {
  is.read((char*)(&iteration), sizeof(iteration));
  is.read((char*)(&depth), sizeof(depth));
  {
    size_t vnumSize;
    is.read((char*)(&vnumSize), sizeof(vnumSize));
    vnums.resize(vnumSize);
    is.read((char*)(&vnums[0]), sizeof(uint32_t)*vnumSize);
  }
  {
    size_t size;
    is.read((char*)(&size), sizeof(size));
    bits.resize(size);
    for (size_t i = 0; i < size; ++i) {
      size_t width;
      is.read((char*)(&width), sizeof(width));
      bits[i].resize(width);
      is.read((char*)(&bits[i][0]), sizeof(uint64_t)*width);
    }
  }
  {
    size_t size;
    is.read((char*)(&size), sizeof(size));
    intervalIndex.resize(size);
    for (size_t i = 0; i < size; ++i) {
      uint64_t s, e;
      is.read((char*)(&s), sizeof(s));
      is.read((char*)(&e), sizeof(e));
      intervalIndex[i] = make_pair(s, e);
    }
  }
  {
    size_t size;
    is.read((char*)(&size), sizeof(size));
    zeros.resize(size);
    is.read((char*)(&zeros[0]), sizeof(uint64_t) * size);
  }
  graphKernel.load(is);
}

void gWM::save(ostream &os) {
  os.write((const char*)(&iteration), sizeof(iteration));
  os.write((const char*)(&depth), sizeof(depth));
  {
    size_t vnumSize = vnums.size();
    os.write((const char*)(&vnumSize), sizeof(vnumSize));
    os.write((const char*)(&vnums[0]), sizeof(uint32_t)*vnumSize);
  }

  {
    size_t size = bits.size();
    os.write((const char*)(&size), sizeof(size));
    for (size_t i = 0; i < size; ++i) {
      size_t width = bits[i].size();
      os.write((const char*)(&width), sizeof(width));
      os.write((const char*)(&bits[i][0]), sizeof(uint64_t)*width);
    }
  }
  {
    size_t size = intervalIndex.size();
    os.write((const char*)(&size), sizeof(size));
    for (size_t i = 0; i < size; ++i) {
      uint64_t s = intervalIndex[i].first;
      uint64_t e = intervalIndex[i].second;
      os.write((const char*)(&s), sizeof(s));
      os.write((const char*)(&e), sizeof(e));
    }
  }
  {
    size_t size = zeros.size();
    os.write((const char*)(&size), sizeof(size));
    os.write((const char*)(&zeros[0]), sizeof(uint64_t) * size);
  }
  graphKernel.save(os);
}

void gWM::makeBit(const vector<uint32_t> &v, uint64_t d, vector<uint64_t> &bits) {
  for (size_t i = 0; i < v.size(); ++i) {
    if (getBit(v[i], d))
      bits[i/64] |= (1ULL << i%64);
  }
}

void gWM::initMatrix(uint64_t width) {
  bits.resize(depth);
  for (size_t i = 0; i < depth; ++i)
    bits[i].resize(width);
}

void gWM::buildWaveletMatrix(vector<uint32_t> &cids) {
  vector<uint32_t> newCids(cids.size());
  for (uint64_t d = 0; d < depth; ++d) {
    makeBit(cids, depth - d - 1, bits[d]);
    if (d == depth - 1) break;
    uint64_t counter = 0;
    for (size_t i = 0; i < cids.size(); ++i) {
      if (!getBit(cids[i], depth - d - 1))
	newCids[counter++] = cids[i];
    }
    for (size_t i = 0; i < cids.size(); ++i) {
      if (getBit(cids[i], depth - d - 1))
	newCids[counter++] = cids[i];
    }
    for (size_t i = 0; i < cids.size(); ++i)
      cids[i] = newCids[i];
  }                                                 
}

void gWM::constructor(const char *fname, const char *oname, int _iteration) {
  iteration = _iteration;
  
  cerr << "start reading inputfile:" << fname << endl;
  graphKernel.readfile(fname);
  cerr << "end reading inputfile" << endl;

  double startTime = clock();
  double sTime = clock();
  cerr << "start building inverted index" << endl;
  std::vector<std::vector<uint32_t> > invertedIndex;
  buildInvertedIndex(invertedIndex);
  cerr << "end building inverted index" << endl;
  double eTime = clock();
  
  fprintf(stdout, "cpu time for building inverted index (sec):%f\n", (eTime - sTime)/CLOCKS_PER_SEC);
  
  sTime = clock();
  cerr << "start building initial intervals" << endl;
  buildInitialIntervals(invertedIndex);
  cerr << "end building initial intervals" << endl;

  vector<uint32_t> cids;
  cerr << "start converting invart index" << endl;
  converter(invertedIndex, cids);
  cerr << "end converting inverval index" << endl;

  cout << "length of id list:" << cids.size() << endl;

  cerr << "start inspecting height of cids" << endl;
  inspectDepth(cids);
  cerr << "end inspecting height of cids" << endl;

  cout << "depth of wavelet matrix:" << depth << endl;

  cerr << "start building wavelet matrix" << endl;
  initMatrix(cids.size()/64 + 1);
  buildWaveletMatrix(cids);
  cerr << "end building wavelet matrix" << endl;

  cerr << "start building rank dictionaries" << endl;
  buildRankDictionary();
  cerr << "end building rank dictionaries" << endl;

  cerr << "start setting zero arrays" << endl;
  setZeros(cids);
  cerr << "end setting zero arrays" << endl;

  eTime = clock();

  fprintf(stdout, "cpu time for building wavelet matrix (sec):%f\n", (eTime - sTime)/CLOCKS_PER_SEC);

  double endTime = clock();
  
  fprintf(stdout, "total construction time (sec):%f\n", (endTime - startTime)/CLOCKS_PER_SEC);

  ofstream ofs(oname);
  if (!ofs) {
    cerr << "cannot open:" << oname << endl;
    exit(1);
  }
  save(ofs);
  ofs.close();

  //  cerr << "initial interval size (byte):" << (cids.size()*32)/8 << endl;
  //  cerr << "# of labels:" << graphKernel.getValLabels() << endl;
  cout << "total memory (byte): " << getByte() << endl;
  /*
  cerr << "inverted index size (byte):" << getInvertedIndexSize() << endl;
  cerr << "interval index size (byte):" << getIntervalIndexSize() << endl;
  cerr << "vnums size (byte):" << getVnumsSize() << endl;
  cerr << "bit array size (byte):" << getBitSize() << endl;
  */
  //  cout << "overhead (byte):" << getDicSize() << endl;
}
