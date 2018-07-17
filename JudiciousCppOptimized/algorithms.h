//
// Created by lukas on 7/16/18.
//

#ifndef JUDICIOUSCPPOPTIMIZED_ALGORITHMS_H
#define JUDICIOUSCPPOPTIMIZED_ALGORITHMS_H

Hypergraph getHypergraphFromPartitionFile(const std::string &filepath, int partitionNumber);

void partition(const Hypergraph &hypergraph, const std::set<size_t> &setOfKs);

void printDDF(size_t k, const std::vector<std::vector<uint32_t>> &partitions);


#endif //JUDICIOUSCPPOPTIMIZED_ALGORITHMS_H
