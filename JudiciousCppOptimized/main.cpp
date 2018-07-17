#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <set>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>

#include "Hypergraph.h"
#include "algorithms.h"


/**
 * Extract a set of k's from the string that is input on the command line.
 * E.g., get {2, 4, 8} from "2,4,8"
 */
std::set<size_t> getKSetFromKString(std::string kString) {
    std::set<size_t> kSet;

    if (boost::contains(kString, ",")) {  // Input contains multiple elements
        std::vector<std::string> splitKString;
        boost::split(splitKString, kString, boost::is_any_of(","));

        for (auto s : splitKString) {
            size_t cur;
            std::stringstream str(s);
            str >> cur;
            kSet.insert(cur);
        }
    } else {  // Input contains only one element
        size_t k;
        std::stringstream str(kString);
        str >> k;
        kSet.insert(k);
    }

    return kSet;
}

int main(int argc, char **argv) {
    std::string filepath;
    int partitionNumber = 0;
    std::set<size_t> kSet;

    // Parse arguments
    if (argc == 3 || argc == 4) {
        filepath = argv[1];
        std::string k_string(argv[2]);
        kSet = getKSetFromKString(k_string);

        for (size_t k : kSet) {
            if (k < 2) {
                std::cout << "The number of CPUs k can't be smaller than 2" << std::endl;
                return 1;
            }
        }

        if (argc == 4) {
            std::string pn_string(argv[3]);
            std::stringstream pstr(pn_string);
            pstr >> partitionNumber;
        }
    } else {
        std::cout << "Usage: " << argv[0] << " partition_file k1[,k2[,k3...]] [partition_number]" << std::endl;
        return 1;
    }

    Hypergraph hypergraph = getHypergraphFromPartitionFile(filepath, partitionNumber);
    partition(hypergraph, kSet);

    return 0;
}
