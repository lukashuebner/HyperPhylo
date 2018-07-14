#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <set>
#include <algorithm>
#include <cassert>

#include <boost/range/algorithm/set_algorithm.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include "Hypergraph.h"


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
        partition.emplace_back();
        std::vector<std::string> split_line;
        boost::split(split_line, line, boost::is_any_of(" "));
        for (const auto &s : split_line) {
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
 * Returns the set S containing each combination with cmPlusD elements that derives from at least one element in E.
 * Also contains a list of elements in E that are covered by the element in S.
 *
 * @param cmPlusD The number of elements in a combination.
 * @param e The set e as described in generateE.
 * @return The set S.
 */
std::vector<sElem> generateS(size_t cmPlusD, const std::vector<eElem> &e) {
    assert(cmPlusD < INT32_MAX);
    assert(!e.empty());

    // TODO maybe figure out max size and reserve?
    std::vector<sElem> s;

    // For each element in E change each element that is a zero to a one.
    for (size_t currentEidx = 0; currentEidx < e.size(); currentEidx++) {
        const eElem &currentE = e[currentEidx];
        for (size_t i = 0; i < currentE.size(); i++) {
            if (!currentE[i]) {
                // Create the combination
                sElem newS(currentE);
                newS.combination[i] = true;

                // Insert into S if it doesn't exist already
                auto foundIterator = std::find(s.begin(), s.end(), newS);
                if (foundIterator == s.end()) {
                    newS.coveredEElems.insert(currentEidx);
                    s.push_back(newS);
                } else { // If it does already exist, only add the element of E to the covered ones
                    foundIterator->coveredEElems.insert(currentEidx);
                }
            }
        }
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
std::vector<boost::dynamic_bitset<>> findMinimalSubset(const std::vector<eElem> &e, const std::vector<sElem> &s) {
    std::set<size_t> alreadyCovered;
    std::vector<boost::dynamic_bitset<>> minimalSubset;
    minimalSubset.reserve(e.size());

    // As long as not all of e is covered, i.e. alreadyCovered and E differ
    // Because of how the loop works (adding diffsets), alreadyCovered and E are the same when they have the same number of elements
    while (alreadyCovered.size() == e.size()) {
        // findest longest difference set
        std::set<size_t> longestDiffset;
        boost::dynamic_bitset<> combinationOfLongestDiffset;
        for (const sElem &currentS : s) {
            std::set<size_t> diff;
            boost::range::set_difference(currentS.coveredEElems, alreadyCovered, std::inserter(diff, diff.end()));
            if (diff.size() > longestDiffset.size()) {
                longestDiffset = diff;
                combinationOfLongestDiffset = currentS.combination;
            }
        }

        // Add all elements of the found longest diffset to the already covered elements of e
        std::copy(longestDiffset.begin(), longestDiffset.end(), std::inserter(alreadyCovered, alreadyCovered.end()));

        // add found longest diffset combination to the resulting minimal subset
        minimalSubset.push_back(combinationOfLongestDiffset);
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
std::vector<boost::dynamic_bitset<>> minimumKAndD(size_t cmPlusD, const std::vector<eElem> &e) {
    std::vector<sElem> s = generateS(cmPlusD, e);
    return findMinimalSubset(e, s);
}

/**
 * Generates a set E containing a set e_i for each hypernode i.
 * Each set e_i contains all hyperedges that contain the hypernode i.
 *
 * @param hypergraph The input hypergraph
 * @return the set E
 */
std::vector<eElem> generateE(const Hypergraph &hypergraph) {
    const std::set<uint32_t> &hypernodes = hypergraph.getHypernodes();
    const std::set<hElem> &hyperedges = hypergraph.getHyperEdges();

    std::vector<boost::dynamic_bitset<>> e;
    e.reserve(hypernodes.size());

    for (uint32_t hnode : hypernodes) {
        eElem curE;
        curE.reserve(hyperedges.size());
        for (const hElem &hedge : hyperedges) {
            // If hypernode is in hyperedge set 1, else 0
            curE.push_back(hedge.count(hnode) == 1);
        }
        e.push_back(curE);
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
std::vector<std::vector<uint32_t>> partition(size_t n, const Hypergraph &hypergraph) {
    // Generate set E according to the paper
    std::vector<eElem> originalE = generateE(hypergraph);
    std::vector<eElem> e = originalE;


    // calulate hyperdegree of the hypergraph
    size_t cm = 0;
    for (const eElem &curE : e) {
        size_t curSize = curE.size();
        if (curSize > cm) {
            cm = curSize;
        }
    }

    // get hyperedge count of the hypergraph
    size_t m = hypergraph.getHyperEdges().size();

    // Can skip the first cycle because that results in E = S* anyway
    for (size_t d = 1; d < m - cm; d++) {
        std::vector<boost::dynamic_bitset<>> sStar = minimumKAndD(cm + d, e);
        size_t k = sStar.size();
        if (k > n) {
            // Replace e with sStar
            e = sStar;
        } else {
            // Extract partitions
            std::vector<std::vector<uint32_t>> partitions;
            for (const boost::dynamic_bitset<> &currentS : sStar) {
                std::vector<uint32_t> partition;
                std::vector<uint32_t> hypernodes(hypergraph.getHypernodes().begin(), hypergraph.getHypernodes().end());
                partition.reserve(hypernodes.size());

                // If element e & element s == element e of the original set E, then the vertex is part of
                // the partition that the element s represents because the vertex is fully contained in this partition
                for (size_t i = 0; i < originalE.size(); i++) {
                    if ((originalE[i] & currentS) == originalE[i]) {
                        partition.push_back(hypernodes[i]);
                    }
                }

            }
            return partitions;
        }
    }

    std::cout << "Couldn't find any working partitioning. Exiting..." << std::endl;
    exit(1);
}

void printDDF(size_t k, const std::vector<std::vector<uint32_t>> &partitions) {
    std::cout << k << std::endl;
    size_t partitionCounter = 1;
    for (const std::vector<uint32_t> &partition : partitions) {
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

    //Hypergraph hypergraph = getHypergraphFromPartitionFile(filepath);

//    std::set<uint32_t> V = {1, 2, 3, 4, 5, 6};
//    std::set<hElem> H = {
//            {1, 2, 3},
//    	    {4, 5},
//    	    {2, 6},
//    	    {3, 5, 6}
//    };

    std::set<uint32_t> V = {0,1,2};
    std::set<hElem> H = {
            {0,2},
            {0,1},
            {2},
            {1},
            {0,1,2}
    };

    std::vector<std::vector<uint32_t>> partitions = partition(k, Hypergraph(V, H));

//    std::vector<std::vector<uint32_t>> partitions = partition(k, hypergraph);

    printDDF(k, partitions);

    return 0;
}
