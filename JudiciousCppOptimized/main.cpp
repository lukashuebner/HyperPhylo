#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "Hypergraph.h"
#include "algorithms.h"

int main(int argc, char **argv) {
    std::string filepath;
    size_t k = 0;
    int partitionNumber = 0;

    // Parse arguments
    if (argc == 3 || argc == 4) {
        filepath = argv[1];
        std::string k_string(argv[2]);
        std::stringstream str(k_string);
        str >> k;

        if (k < 1) {
            std::cout << "The number of CPUs k can't be smaller than 1" << std::endl;
            return 1;
        }

        if (argc == 4) {
            std::string pn_string(argv[3]);
            std::stringstream pstr(pn_string);
            pstr >> partitionNumber;
        }
    } else {
        std::cout << "Usage: " << argv[0] << " partition_file k [partition_number]" << std::endl;
        return 1;
    }

    Hypergraph hypergraph = getHypergraphFromPartitionFile(filepath, partitionNumber);
    std::vector<std::vector<uint32_t>> partitions = partition(k, hypergraph);

    printDDF(k, partitions);

    return 0;
}
