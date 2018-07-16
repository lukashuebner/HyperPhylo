//
// Created by lukas on 7/16/18.
//

#ifndef JUDICIOUSCPPOPTIMIZED_ALGORITHMS_H
#define JUDICIOUSCPPOPTIMIZED_ALGORITHMS_H

Hypergraph getHypergraphFromPartitionFile(const std::string &filepath, int partitionNumber);

std::vector<std::vector<uint32_t>> partition(size_t n, const Hypergraph &hypergraph);

void printDDF(size_t k, const std::vector<std::vector<uint32_t>> &partitions);


#endif //JUDICIOUSCPPOPTIMIZED_ALGORITHMS_H
