//
// Created by lukas on 7/16/18.
//

#ifndef JUDICIOUSCPPOPTIMIZED_ALGORITHMS_H
#define JUDICIOUSCPPOPTIMIZED_ALGORITHMS_H

#define DEBUG 1
#define DEBUG_DETERMINISM 1
#define DEBUG_PROGRESS 2
#define DEBUG_VERBOSE 3

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
