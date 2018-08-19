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

#include <boost/range/algorithm/set_algorithm.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <unordered_map>

#include "Hypergraph.h"

#define DEBUG 1
#define DEBUG_PROGRESS 1
#define DEBUG_VERBOSE 2

#define DETERMINISM

#if DEBUG > 0
#define DEBUG_LOG(level, message) do { if (DEBUG >= (level)) std::cout << (message) << std::flush; } while (0)
#else
#define DEBUG_LOG(level, message)
#endif

#define FAKE_DETECTION

Hypergraph getHypergraphFromPartitionFile(const std::string &filepath, int partitionNumber);

void partition(const Hypergraph &hypergraph, const std::set<size_t> &setOfKs);

void printDDF(size_t k, const std::vector<std::vector<uint32_t>> &partitions);


#endif //JUDICIOUSCPPOPTIMIZED_ALGORITHMS_H
