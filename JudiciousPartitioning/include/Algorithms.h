#ifndef JUDICIOUSCPPOPTIMIZED_ALGORITHMS_H
#define JUDICIOUSCPPOPTIMIZED_ALGORITHMS_H

#include "Hypergraph.h"
#include "Definitions.h"

#include <cstdlib>
#include <vector>
#include <string>
#include <set>

//#define DETERMINISM
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
