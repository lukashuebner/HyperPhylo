#ifndef JUDICIOUSCPPOPTIMIZED_ALGORITHMS_H
#define JUDICIOUSCPPOPTIMIZED_ALGORITHMS_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <set>
#include <algorithm>
#include <cassert>
#include <unordered_set>
#include <unordered_map>
#include <omp.h>

#include <boost/range/algorithm/set_algorithm.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <tbb/concurrent_unordered_set.h>
#include <tbb/concurrent_unordered_map.h>

#include "Hypergraph.h"

#define DEBUG 0
#define DEBUG_PROGRESS 1
#define DEBUG_VERBOSE 2

#define DETERMINISM
//#define FAKE_DETECTION

#if DEBUG > 0
#define DEBUG_LOG(level, message) do { if (DEBUG >= (level)) std::cout << (message) << std::flush; } while (0)
#else
#define DEBUG_LOG(level, message)
#endif

Hypergraph getHypergraphFromPartitionFile(const std::string &filepath, uint32_t partitionNumber);

void partition(const Hypergraph &hypergraph, const std::set<size_t> &setOfKs);

void printDDF(size_t k, const std::vector<std::vector<size_t>> &partitions);


#endif //JUDICIOUSCPPOPTIMIZED_ALGORITHMS_H
