#include <iostream>
#include <sstream>
#include <set>
#include <algorithm>
#include <boost/algorithm/cxx11/all_of.hpp>
#include <boost/range/algorithm/set_algorithm.hpp>

#include "Hypergraph.h"
#include "twiddle.h"


Hypergraph getHypergraphFromPartitionFile(const std::string &filepath) {
    // TODO
    std::set<uint32_t> hypernodes;
    std::set<hElem> hyperedges;
    Hypergraph hypergraph(hypernodes, hyperedges);
    return hypergraph;
}

/**
 * Returns all possible (hyperedges.size() over cmPlusD) combinations of hyperedges.
 *
 * @param hyperedges The set of hyperedges to combine.
 * @param cmPlusD the number of elements in one combination.
 * @return The set of combinations of hyperedges.
 */
std::set<std::set<hElem>> generateT(const std::set<hElem> &hyperedges, uint64_t cmPlusD) {
    // TODO Try our "only calculate fitting elements of T" trick
    return getAllCombinations(hyperedges, (int) hyperedges.size(), (int) cmPlusD);
}

/**
 * Returns the set S containing one element for each element in t. The element s contains
 * each element of E that is a subset of the element in t.
 *
 * @param t The set of all combinations of hyperedges of a specific size.
 * @param e The set e as described in generateE.
 * @return The set S
 */
std::set<sElem> generateS(const std::set<std::set<hElem>> &t, const std::set<eElem> &e) {
    std::set<sElem> s;

    // For all sets in t create a set in s
    for (const std::set<hElem> &currentT : t) {
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
 * @param t The set of combinations of hyperedges with a specific number of hyperedges per element.
 * @param e The set E as described in generateE.
 * @return The found minimal set. The size of the minimal set is the value k.
 */
std::set<std::set<eElem>> minimumKAndD(const std::set<std::set<hElem>> &t, const std::set<eElem> &e) {
    std::set<sElem> s = generateS(t, e);
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

    for (size_t d = 0; d < m - cm; d++) {
        std::set<sElem> sStar = minimumKAndD(generateT(hypergraph.getHyperEdges(), cm + d), e);
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

    // Hypergraph hypergraph = getHypergraphFromPartitionFile(filepath);

    // Hardcoded sample hypergraph
    std::set<uint32_t> hypernodes { 1, 2, 3, 4, 5, 6 };
    std::set<hElem> hyperedges {
        {1, 2, 3},
        {4, 5},
        {2, 6},
        {3, 5, 6}
    };
    Hypergraph hypergraph(hypernodes, hyperedges);
    std::set<std::set<uint32_t>> partitions = partition(k, hypergraph);

    printDDF(k, partitions);

    return 0;
}
