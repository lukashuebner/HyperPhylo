#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <set>
#include <algorithm>
#include <cassert>

#include <boost/algorithm/cxx11/all_of.hpp>
#include <boost/range/algorithm/set_algorithm.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include "Hypergraph.h"
#include "twiddle.h"


/**
 * Parse a partition file and create its hypergraph.
 * @param filepath The path to the partition file.
 * @return The hypergraph that represents the partition described in the input file.
 */
Hypergraph getHypergraphFromPartitionFile(const std::string &filepath) {
    // Read the file
    std::ifstream input_file(filepath);

    if (input_file.bad()) {
        std::cout << "Could not read " << filepath << std::endl;
        exit(1);
    }

    // We will create a 2D array containing the information from the file
    std::vector<std::vector<uint32_t>> partition;
    std::string line;
    int i = -3;
    while (std::getline(input_file, line)) {
        i++;
        // Ignore the first two lines
        if (i < 0) {
            continue;
        }

        // Fill in the 2D array
        partition.push_back({});
        std::vector<std::string> split_line;
        boost::split(split_line, line, boost::is_any_of(" "));
        for (auto s : split_line) {
            // Awkward conversion from string to uint32_t
            uint32_t curRepeatClass;
            std::istringstream iss(s);
            iss >> curRepeatClass;

            partition[i].push_back(curRepeatClass);
        }
    }

    // Now create the hypergraph from the 2D array
    std::set<uint32_t> hypernodes;
    std::set<hElem> hyperedges;

    unsigned long numberOfSites = partition[0].size();

    // Fill the hypernodes
    for (uint32_t j = 0; j < numberOfSites; j++) {
        hypernodes.insert(j);
    }

    // Fill the hyperedges
    for (std::vector<uint32_t> curLine : partition) {  // Traverse all lines (rows in the partition)
        // Can have a maximum of numberOfSites repeat classes per row:
        for (uint32_t curRepeatClass = 0; curRepeatClass < numberOfSites; curRepeatClass++) {
            hElem curHyperedge;
            for (uint32_t k = 0; k < numberOfSites; k++) {  // Traverse all sites (columns in the partition)
                if (curLine[k] == curRepeatClass) {
                    curHyperedge.insert(k);
                }
            }
            if (!curHyperedge.empty()) {
                hyperedges.insert(curHyperedge);
            } else {  // No site contains curRepeatClass --> none will contain any "higher" repeat class
                break;
            }
        }
    }

    Hypergraph hypergraph(hypernodes, hyperedges);
    return hypergraph;
}

///**
// * Returns all possible (hyperedges.size() over cmPlusD) combinations of hyperedges.
// *
// * @param hyperedges The set of hyperedges to combine.
// * @param cmPlusD the number of elements in one combination.
// * @return The set of combinations of hyperedges.
// */
//std::set<std::set<hElem>> generateT(const std::set<hElem> &hyperedges, uint64_t cmPlusD) {
//    // TODO Try our "only calculate fitting elements of T" trick
//    return getAllCombinations(hyperedges, (int) hyperedges.size(), (int) cmPlusD);
//}

/**
 * Returns the set S containing one element for each element in T. The element s contains
 * each element of E that is a subset of the element in T. T is generated in place.
 *
 * @param t cmPlusD The number of elements per combination in T.
 * @param e The set e as described in generateE.
 * @return The set S
 */
std::set<sElem> generateS(size_t cmPlusD, const std::set<eElem> &e) {
    assert(cmPlusD < INT32_MAX);
    assert(!e.empty());

    std::set<sElem> s;

    // Init generator for T
    initCombinationGenerator((int) cmPlusD);

    // For all sets in t create a set in s
    std::set<hElem> currentT = getNextCombination();
    while (!currentT.empty()) {
        sElem currentS;

        // In the set currentS are all elements of e that are a subset of currentT
        for (const eElem &currentE : e) {
            // If currentE is a subset of currentT
            if (boost::algorithm::all_of(currentE.hyperedges.begin(), currentE.hyperedges.end(),
                            [currentT](hElem element) { return currentT.count(element); } )) {
                currentS.insert(currentE);
            }
        }

        s.insert(currentS);
        currentT = getNextCombination();
    }

    return s;
}

/**
 * Finds the minimal subset of the set S that is covering all of the set E.
 *
 * @param e The set E to cover.
 * @param s The set S as input.
 * @return The found minimal subset.
 */
std::set<sElem> findMinimalSubset(const std::set<eElem> &e, const std::set<sElem> &s) {
    std::set<eElem> alreadyCovered;
    std::set<sElem> minimalSubset;

    while (alreadyCovered != e) {
        // findest longest difference set
        std::set<eElem> longestDiffset;
        for (const sElem &currentS : s) {
            std::set<eElem> diff;
            // TODO Maybe STL set_difference is faster?
            boost::range::set_difference(currentS, alreadyCovered, std::inserter(diff, diff.end()));
            if (diff.size() > longestDiffset.size()) {
                longestDiffset = diff;
            }
        }

        // Add all elements of the found longest diffset to the already covered elements of e
        std::copy(longestDiffset.begin(), longestDiffset.end(), std::inserter(alreadyCovered, alreadyCovered.end()));

        // add found longest diffset to the resulting minimal subset
        minimalSubset.insert(longestDiffset);
    }

    return minimalSubset;
}

/**
 * Runs the minimum k and d algorithm as shown in the paper.
 *
 * @param t cmPlusD The number of elements per combination in T.
 * @param e The set E as described in generateE.
 * @return The found minimal set. The size of the minimal set is the value k.
 */
std::set<std::set<eElem>> minimumKAndD(size_t cmPlusD, const std::set<eElem> &e) {
    std::set<sElem> s = generateS(cmPlusD, e);
    return findMinimalSubset(e, s);
}

/**
 * Generates a set E containing a set e_i for each hypernode i.
 * Each set e_i contains all hyperedges that contain the hypernode i.
 *
 * @param hypergraph The input hypergraph
 * @return the set E
 */
std::set<eElem> generateE(const Hypergraph &hypergraph) {
    std::set<eElem> e;

    const std::set<uint32_t> &hypernodes = hypergraph.getHypernodes();
    const std::set<hElem> &hyperedges = hypergraph.getHyperEdges();
    for (uint32_t hnode : hypernodes) {
        eElem curE;
        for (const hElem &hedge : hyperedges) {
            // If hypernode is in hyperedge
            if (hedge.count(hnode)) {
                curE.hyperedges.insert(hedge);
            }
        }
        curE.coveredHypernodes.insert(hnode);
        e.insert(curE);
    }

    return e;
}

/**
 * Partitions the input hypergraph.
 *
 * @param n number of CPUs (argument 'k' of the program call).
 * @param hypergraph the hypergraph to partition.
 * @return The resulting partitions as set of hypernode sets.
 */
std::set<std::set<uint32_t>> partition(size_t n, const Hypergraph &hypergraph) {
    // Generate set E according to the paper
    std::set<eElem> e = generateE(hypergraph);

    // calulate hyperdegree of the hypergraph
    size_t cm = 0;
    for (const eElem &curE : e) {
        size_t curSize = curE.hyperedges.size();
        if (curSize > cm) {
            cm = curSize;
        }
    }

    // get hyperedge count of the hypergraph
    size_t m = hypergraph.getHyperEdges().size();

    // Set the base set for the combinations generator to the hyperedge
    setBaseSet(hypergraph.getHyperEdges());

    for (size_t d = 0; d < m - cm; d++) {
        std::set<sElem> sStar = minimumKAndD(cm + d, e);
        size_t k = sStar.size();
        if (k > n) {
            // Create new e
            e.clear();
            std::set<uint32_t> usedHypernodes;
            // Shrink down e to only needed elements (the ones that are part of the minimal subset sStar)
            for (const sElem &currentS : sStar) {
                eElem currentE;
                for (const eElem &element : currentS) {
                    // Add hyperedges of old element into new element currentE
                    std::copy(element.hyperedges.begin(), element.hyperedges.end(),
                              std::inserter(currentE.hyperedges, currentE.hyperedges.end()));
                    // Add only hypernodes to the currentE that aren't already used in another element of the new e
                    // TODO Maybe STL set_difference is faster?
                    std::set<uint32_t> diff;
                    boost::range::set_difference(element.coveredHypernodes, usedHypernodes,
                                                 std::inserter(diff, diff.end()));
                    std::copy(diff.begin(), diff.end(),
                              std::inserter(usedHypernodes, usedHypernodes.end()));
                    std::copy(diff.begin(), diff.end(),
                              std::inserter(currentE.coveredHypernodes, currentE.coveredHypernodes.end()));
                }
                e.insert(currentE);
            }
        } else {
            // Extract partitions
            std::set<std::set<uint32_t>> partitions;
            for (std::set<eElem> s : sStar) {
                std::set<uint32_t> partition;
                for (const eElem &currentE : s) {
                    std::copy(currentE.coveredHypernodes.begin(), currentE.coveredHypernodes.end(),
                              std::inserter(partition, partition.end()));
                }
                partitions.insert(partition);
            }
            return partitions;
        }
    }

    std::cout << "Couldn't find any working partitioning. Exiting..." << std::endl;
    exit(1);
}

void printDDF(size_t k, const std::set<std::set<uint32_t>> &partitions) {
    std::cout << k << std::endl;
    size_t partitionCounter = 1;
    for (const std::set<uint32_t> &partition : partitions) {
        std::cout << "CPU " << partitionCounter++ << " 1" << std::endl;
        std::cout << "partition_0 " << partition.size() << " ";
        for (uint32_t hypernode : partition) {
            std::cout << hypernode << " ";
        }
        std::cout << std::endl;
    }
}


int main(int argc, char **argv) {
    std::string filepath;
    size_t k = 0;

    // Parse arguments
    if (argc == 3) {
        filepath = argv[1];
        std::string k_string(argv[2]);
        std::stringstream str(k_string);
        str >> k;

        if (k < 1) {
            std::cout << "The number of CPUs k can't be smaller than 1" << std::endl;
        }
    } else {
        std::cout << "Usage: " << argv[0] << " partition_file k" << std::endl;
    }

    Hypergraph hypergraph = getHypergraphFromPartitionFile(filepath);
    
    std::set<std::set<uint32_t>> partitions = partition(k, hypergraph);

    printDDF(k, partitions);

    return 0;
}
