#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <tuple>
#include <limits>

#include <sys/stat.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>

#include "../JudiciousPartitioning/include/Hypergraph.h"
//#include "../JudiciousPartitioning/include/EElem.h"
//#include "../JudiciousPartitioning/include/Algorithms.h"


std::vector<std::string> splitLineAtSpaces(const std::string &line) {
    std::vector<std::string> splitLine;
    boost::split(splitLine, line, boost::is_any_of(" "));
    return splitLine;
}

uint32_t stringToUint32t(const std::string &theString) {
    uint32_t theInt;
    std::istringstream iss(theString);
    iss >> theInt;
    return theInt;
}

void printDDF(size_t k, const std::vector<std::vector<size_t>> &partitions) {
    std::cout << k << std::endl;
    size_t partitionCounter = 1;
    for (const std::vector<size_t> &partition : partitions) {
        std::cout << "CPU" << partitionCounter++ << " 1" << std::endl;
        std::cout << "partition_0 " << partition.size();
        for (size_t hypernode : partition) {
            std::cout << " " << hypernode;
        }
        std::cout << std::endl;
    }
}

/**
 * Parse a partition file and create its hypergraph.
 * @param filepath The path to the partition file.
 * @param partitionNumber The number of the partition that should be extracted from the file. 0 is the first partition.
 * When in doubt, use 0 :)
 * @return The hypergraph that represents the partition described in the input file.
 */
Hypergraph getHypergraphFromPartitionFile(const std::string &filepath, uint32_t partitionNumber) {
    // Read the file
    std::ifstream input_file(filepath);

    if (input_file.bad()) {
        std::cout << "Could not read " << filepath << std::endl;
        exit(1);
    }

    // We will create a 2D array containing the information from the file
    std::vector<std::vector<uint32_t>> partition;
    std::string line;
    bool curPartitionIsWantedPartition = false;
    uint32_t numberOfSitesFromFile = 0;
    int i = -2;
    while (std::getline(input_file, line)) {
        std::vector<std::string> splitLine = splitLineAtSpaces(line);

        if (i == -2) {  // First line
            i++;
        } else {
            if (boost::starts_with(splitLine[0], "partition")) {
                uint32_t curPartitionNumber = stringToUint32t(splitLine[0].substr(10));
                if (curPartitionNumber == partitionNumber) {
                    curPartitionIsWantedPartition = true;
                    numberOfSitesFromFile = stringToUint32t(splitLine[1]);
                    i++;
                } else {
                    curPartitionIsWantedPartition = false;
                }
            } else {
                if (curPartitionIsWantedPartition) {
                    partition.emplace_back();
                    for (const auto &s : splitLine) {
                        if (!s.empty()) {  // There can be empty elements due to too much whitespace in the input
                            // (especially at the end of the line). Ignore those, else buggy elements will be inserted.
                            uint32_t curRepeatClass = stringToUint32t(s);
                            partition[i].push_back(curRepeatClass);
                        }
                    }
                    i++;
                }
            }
        }
    }

    // Now create the hypergraph from the 2D array
    std::vector<uint32_t> hypernodes;
    std::vector<hElem> hyperedges;

    unsigned long numberOfSites = partition[0].size();
    assert(numberOfSites == numberOfSitesFromFile);


    // Fill the hypernodes
    for (uint32_t j = 0; j < numberOfSites; j++) {
        hypernodes.push_back(j);
    }

    // Fill the hyperedges
    hElem curHyperedge;
    for (std::vector<uint32_t> curLine : partition) {  // Traverse all lines (rows in the partition)
        // Can have a maximum of numberOfSites repeat classes per row:
        for (uint32_t curRepeatClass = 0; curRepeatClass < numberOfSites; curRepeatClass++) {
            curHyperedge.clear();
            for (uint32_t k = 0; k < numberOfSites; k++) {  // Traverse all sites (columns in the partition)
                if (curLine[k] == curRepeatClass) {
                    curHyperedge.push_back(k);
                }
            }
            if (!curHyperedge.empty()) {
                hyperedges.push_back(curHyperedge);
            } else {  // No site contains curRepeatClass --> none will contain any "higher" repeat class
                break;
            }
        }
    }

    assert(hypernodes.size() == numberOfSitesFromFile);

    return Hypergraph(std::move(hypernodes), std::move(hyperedges));
}


///**
// * Generates a set E containing a set e_i for each hypernode i with duplicates.
// * Each set e_i contains all hyperedges that contain the hypernode i.
// *
// * @param hypergraph The input hypergraph
// * @return the set E
// */
//std::vector<EElem> generateEWithDuplicates(const Hypergraph &hypergraph) {
//    DEBUG_LOG(DEBUG_PROGRESS, "Generating E... ");
//    DEBUG_LOG(DEBUG_VERBOSE, "\n");
//    const std::vector<uint32_t> &hypernodes = hypergraph.getHypernodes();
//    std::vector<hElem> hyperedges = hypergraph.getHyperEdges();
//    std::reverse(hyperedges.begin(), hyperedges.end());
//
//    // Set covered e element for each entry
//    std::vector<EElem> e(hypernodes.size(), EElem(hyperedges.size()));
//    for (uint32_t i = 0; i < e.size(); i++) {
//        e[i].getCoveredE0Elems().insert(i);
//    }
//
//    // Set all combinations accordingly
//    for (size_t hyperedgeIdx = 0; hyperedgeIdx < hyperedges.size(); hyperedgeIdx++) {
//        DEBUG_LOG(DEBUG_VERBOSE, "Generating entry for hyperedge ID " + std::to_string(hyperedgeIdx) + "\r");
//        for (uint32_t node : hyperedges[hyperedgeIdx]) {
//            e[node].getCombination().setBit(hyperedgeIdx);
//        }
//    }
//    DEBUG_LOG(DEBUG_VERBOSE, "\nDone.\n");
//
//    return e;
//}


void partitionMinMax(const Hypergraph& hypergraph, const size_t k, const size_t c) {
    std::vector<std::pair<std::vector<size_t>, std::vector<hElem>>> partitions(k);  // Contains the partitions + the hyperedges in those partitions
    const std::vector<uint32_t>& hypernodes = hypergraph.getHypernodes();
    const std::vector<hElem>& hyperedges = hypergraph.getHyperEdges();
    /*
    for (auto edge : hyperedges) {
        for (auto elem : edge) {
            std::cout << elem << " ";
        }
        std::cout << std::endl << std::endl;
    }
     */
//    std::vector<EElem> e = generateEWithDuplicates(hypergraph);

    for (auto hypernode : hypernodes) {
        std::vector<hElem> curHyperedges;
        for (auto edge : hyperedges) {
            if (std::find(edge.begin(), edge.end(), hypernode) != edge.end()) {
                curHyperedges.push_back(edge);
            }
        }
        // Use magic capacity bound c to get the viable partitions
        std::vector<int> viablePartitionIds;

        int least_filled_partitionId = 0;
        for (int i = 0; i < partitions.size(); i++) {
            std::pair<std::vector<size_t>, std::vector<hElem>> partition = partitions[i];
            if (partition.first.size() < partitions[least_filled_partitionId].first.size()) {
                least_filled_partitionId = i;
            }
        }

        size_t capacity_bound = partitions[least_filled_partitionId].first.size() + c;
        for (int i = 0; i < partitions.size(); i++) {
            std::pair<std::vector<size_t>, std::vector<hElem>> partition = partitions[i];
            if (partition.first.size() <= capacity_bound) {
                viablePartitionIds.push_back(i);
            }
        }

        // Find best partition
        int bestPartitionId = 0;
        size_t maxIntersectionSize = 0;
        for (int i : viablePartitionIds) {
            std::pair<std::vector<size_t>, std::vector<hElem>> partition = partitions[i];
            size_t intersection_size = 0;
            for (auto edge : curHyperedges) {
                if (std::find(partition.second.begin(), partition.second.end(), edge) != partition.second.end()) {
                    intersection_size++;
                }
            }
            if (intersection_size > maxIntersectionSize) {
                bestPartitionId = i;
                maxIntersectionSize = intersection_size;
            }
        }

        if (maxIntersectionSize == 0) {
            // Choose most empty partition
            int minPartitionId;
            size_t minPartitionSize = std::numeric_limits<size_t>::max();
            for (int i = 0; i < partitions.size(); i++) {
                auto partition = partitions[i];
                if (partition.first.size() < minPartitionSize) {
                    minPartitionId = i;
                    minPartitionSize = partition.first.size();
                }
            }
            bestPartitionId = minPartitionId;
        }

        partitions[bestPartitionId].first.push_back(hypernode);
        partitions[bestPartitionId].second.insert(partitions[bestPartitionId].second.end(), curHyperedges.begin(), curHyperedges.end());
    }

    std::vector<std::vector<size_t>> partitionsForDDF;
    for (auto partition : partitions) {
        partitionsForDDF.push_back(partition.first);
    }

    printDDF(k, partitionsForDDF);
}


int main(int argc, char** argv) {
    std::string filepath;
    uint32_t partitionNumber = 0;
    size_t k;

    // Parse arguments
    if (argc == 3 || argc == 4) {
        filepath = argv[1];

        struct stat buffer{};
        if (stat(filepath.c_str(), &buffer) != 0) {
            std::cerr << "The provided repeats file doesn't exist." << std::endl;
            return 1;
        }

        std::stringstream str(argv[2]);
        str >> k;

        if (k < 2) {
            std::cerr << "The number of CPUs k can't be smaller than 2" << std::endl;
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
    size_t c = 12;  // currently hardcoded

    partitionMinMax(hypergraph, k, c);

    return 0;
}
