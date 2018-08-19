#include "Hypergraph.h"
#include "algorithms.h"
#include "Helper.h"

// Key is the index of the element e that maps to the minimal distance element
// Value is a pair where
// * the first value is the index of the element if e with a minimal distance to the key element
// * the second value is the minimal distance
std::unordered_map<size_t, std::pair<size_t, size_t>> minimalDistances;

/**
 * Parse a partition file and create its hypergraph.
 * @param filepath The path to the partition file.
 * @param partitionNumber The number of the partition that should be extracted from the file. 0 is the first partition.
 * When in doubt, use 0 :)
 * @return The hypergraph that represents the partition described in the input file.
 */
Hypergraph getHypergraphFromPartitionFile(const std::string &filepath, int partitionNumber) {
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

    // Fill the hypernodes
    for (uint32_t j = 0; j < numberOfSites; j++) {
        hypernodes.push_back(j);
    }

    // Fill the hyperedges
    for (std::vector<uint32_t> curLine : partition) {  // Traverse all lines (rows in the partition)
        // Can have a maximum of numberOfSites repeat classes per row:
        for (uint32_t curRepeatClass = 0; curRepeatClass < numberOfSites; curRepeatClass++) {
            hElem curHyperedge;
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

    Hypergraph hypergraph(hypernodes, hyperedges);
    return hypergraph;
}


void printDDF(size_t k, const std::vector<std::vector<uint32_t>> &partitions) {
    std::cout << k << std::endl;
    size_t partitionCounter = 1;
    for (const std::vector<uint32_t> &partition : partitions) {
        std::cout << "CPU" << partitionCounter++ << " 1" << std::endl;
        std::cout << "partition_0 " << partition.size();
        for (uint32_t hypernode : partition) {
            std::cout << " " << hypernode;
        }
        std::cout << std::endl;
    }
}

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

    DEBUG_LOG(DEBUG_PROGRESS, "Generating S with only size >= 2 elements\n");

    // TODO maybe figure out max size and reserve?
    std::unordered_set<sElem> s;

    minimalDistances.clear();

    // Run over all possible pairs in E and check if they build a possible combination
    #pragma omp parallel for schedule(dynamic)
    for (size_t firstEidx = 0; firstEidx < e.size(); firstEidx++) {
        for (size_t secondEidx = firstEidx + 1; secondEidx < e.size(); secondEidx++) {
            DEBUG_LOG(DEBUG_VERBOSE, "First element " + std::to_string(firstEidx + 1) + ", second element " + std::to_string(secondEidx + 1) + "\r");

            const eElem &firstE = e[firstEidx];
            const eElem &secondE = e[secondEidx];

            // Calculate distance
            size_t distance = (firstE ^ secondE).count();

            // Add the representation if it is a valid cm + d combination
            assert(distance >= 2);
            if (distance == 2) {
                assert(firstE != secondE);
                boost::dynamic_bitset<> combination = firstE | secondE;
                assert(combination.count() == cmPlusD);

                sElem newS(combination);
                newS.coveredEElems.insert(firstEidx);
                newS.coveredEElems.insert(secondEidx);
                #pragma omp critical
                s.insert(newS);
            } else { // else, add to the table of minimal distances
                #pragma omp critical
                {
                    // Store the mapping a --> b
                    auto result = minimalDistances.insert(
                            std::make_pair(firstEidx, std::make_pair(secondEidx, distance)));

                    // If adding was not successful, there is already an entry for that element of e
                    if (!result.second) {
                        // If the currently stored minimal distance is greater than this one, replace it
                        if (result.first->second.second > distance) {
                            result.first->second.first = secondEidx;
                            result.first->second.second = distance;
                        }
                    }

                    // Store the mapping b --> a
                    auto resultReverse = minimalDistances.insert(
                            std::make_pair(secondEidx, std::make_pair(firstEidx, distance)));

                    // If adding was not successful, there is already an entry for that element of e
                    if (!resultReverse.second) {
                        // If the currently stored minimal distance is greater than this one, replace it
                        if (resultReverse.first->second.second > distance) {
                            resultReverse.first->second.first = firstEidx;
                            resultReverse.first->second.second = distance;
                        }
                    }
                }
            }
        }
    }

    // Run over E and check for each element if it fits into one of the generated combinations
    // TODO This will obviously also add the initial elements of e that created the combination, so maybe we can try to avoid that?
    DEBUG_LOG(DEBUG_VERBOSE, "\n");
    for (size_t eidx = 0; eidx < e.size(); eidx++) {
        DEBUG_LOG(DEBUG_VERBOSE, "Fitting element " + std::to_string(eidx + 1) + "\r");
        for (const sElem &currentS : s) {
            if ((e[eidx] & currentS.combination) == e[eidx]) {
                currentS.coveredEElems.insert(eidx);
            }
        }
    }

    DEBUG_LOG(DEBUG_VERBOSE, "\n");
    DEBUG_LOG(DEBUG_PROGRESS, "Size: " + std::to_string(s.size()) + "\n");

    return std::vector<sElem>(s.begin(), s.end());
}

/**
 * Finds the minimal subset of the set S that is covering all of the set E.
 *
 * @param e The set E to cover.
 * @param s The set S as input.
 * @return The found minimal subset.
 */
std::vector<boost::dynamic_bitset<>> findMinimalSubset(const std::vector<eElem> &e, std::vector<sElem> &s) {
    DEBUG_LOG(DEBUG_PROGRESS, "Searching for minimal subset");

    std::set<size_t> alreadyCovered;
    std::vector<boost::dynamic_bitset<>> minimalSubset;
    minimalSubset.reserve(e.size());

    // Only for determinism. Not actually needed.
#ifdef DETERMINISM
    std::sort(s.begin(), s.end());
#endif

#if DEBUG >= DEBUG_VERBOSE
    std::set<size_t> uniques;
    for (const sElem &currentS : s) {
        uniques.insert(currentS.coveredEElems.begin(), currentS.coveredEElems.end());
    }
    DEBUG_LOG(DEBUG_VERBOSE, "The >= 2 covering s elements cover " + uniques.size() + " unique elements of e";
#endif

    // As long as not all of e is covered, i.e. alreadyCovered and E differ
    // Because of how the loop works (adding diffsets), alreadyCovered and E are the same when they have the same number of elements
    while (alreadyCovered.size() != e.size()) {
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

        // If there is no longest diffset, we need to fill with combinations that cover only one element of E
        if (longestDiffset.empty()) {
            break;
        }

        // Add all elements of the found longest diffset to the already covered elements of e
        std::copy(longestDiffset.begin(), longestDiffset.end(), std::inserter(alreadyCovered, alreadyCovered.end()));

        // add found longest diffset combination to the resulting minimal subset
        minimalSubset.push_back(combinationOfLongestDiffset);
    }

    int counter = 0;
    DEBUG_LOG(DEBUG_VERBOSE, "Elements not covered yet: " + std::to_string(e.size() - alreadyCovered.size()));

    // Fill up coverage if needed
    if (alreadyCovered.size() != e.size()) {
        for (size_t eidx = 0; eidx < e.size(); eidx++) {
            // If the element of e is not already covered, generate a coverage element for it
            if (!alreadyCovered.count(eidx)) {
                counter++;
                // Get the minimalDistances entry for this element of e
                std::unordered_map<size_t,std::pair<size_t, size_t>>::const_iterator elementIterator = minimalDistances.find(eidx);
                assert(elementIterator != minimalDistances.end());
                assert(elementIterator->second.second > 2);

                boost::dynamic_bitset<> combination = e[eidx];
                const eElem &otherElement = e[elementIterator->second.first];

                // Flip a bit that makes the combination approach towards the element that is closest to the combination
                // by flipping a bit to 1 that is already a one in the other element
                for (size_t i = 0; i < otherElement.size(); i++) {
                    if (otherElement[i] == 1 && combination[i] == 0) {
                        combination[i] = true;
                        break;
                    }
                }

                // Mark all uncovered elements of e that are covered by the created filler as covered
                for (size_t checkEidx = 0; checkEidx < e.size(); checkEidx++) {
                    if ((e[checkEidx] & combination) == e[checkEidx]) {
                        assert(checkEidx >= eidx && "I didn't expect this to happen, there should be no coverage introduced to earlier elements");
                        alreadyCovered.insert(checkEidx);
                    }
                }

                // Sanity Check: Does this element cover an element e that isn't already covered by a combination of other minimal subset elements?
            #ifndef NDEBUG
                boost::dynamic_bitset<> coveredSites;
                bool test = true;
                for (const boost::dynamic_bitset<> &current : minimalSubset) {
                    if (!coveredSites.capacity()) {
                        assert(test);
                        test = false;
                        coveredSites = boost::dynamic_bitset<>(current.size());
                    }

                    coveredSites |= current;
                    if ((coveredSites & combination) == combination) {
                        std::cerr << "Combination: " << combination << " is fully covered by elements already in the minimal subset" << std::endl;
                        std::cerr << "Combination contains of elements with " << e[eidx].count() << " and " << otherElement.count() << " ones." << std::endl;
                        std::cerr << "This occured in filling element " << counter << std::endl;
                        assert(false);
                    }
                }
            #endif

                // Insert the covering element
                minimalSubset.push_back(combination);
            }
        }
    }

    DEBUG_LOG(DEBUG_PROGRESS, ", Size: " + std::to_string(minimalSubset.size()) + "\n");

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
    DEBUG_LOG(DEBUG_PROGRESS, "Running minKD\n");
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
std::vector<eElem> generateE(const Hypergraph &hypergraph, bool generateDuplicates) {
    DEBUG_LOG(DEBUG_PROGRESS, "Generating E");
    const std::vector<uint32_t> &hypernodes = hypergraph.getHypernodes();
    std::vector<hElem> hyperedges = hypergraph.getHyperEdges();
    std::reverse(hyperedges.begin(), hyperedges.end());

    std::vector<boost::dynamic_bitset<>> e;
    e.reserve(hypernodes.size());

    for (uint32_t hnode : hypernodes) {
        eElem curE;
        curE.reserve(hyperedges.size());
        for (const hElem &hedge : hyperedges) {
            // If hypernode is in hyperedge set 1, else 0
            curE.push_back(std::find(hedge.begin(), hedge.end(), hnode) != hedge.end());
        }

        if (generateDuplicates) {
            e.push_back(curE);
        } else {
            // If element wasn't found (there is no duplicate), insert it
            if (std::find(e.begin(), e.end(), curE) == e.end()) {
                e.push_back(curE);
            }
        }
    }

    DEBUG_LOG(DEBUG_PROGRESS, " Size: " + std::to_string(e.size()) + "\n");

    return e;
}

/**
 * Partitions the input hypergraph.
 *
 * @param n number of CPUs (argument 'k' of the program call).
 * @param hypergraph the hypergraph to partition.
 * @return The resulting partitions as set of hypernode sets.
 */
void partition(const Hypergraph &hypergraph, const std::set<size_t> &setOfKs) {
    DEBUG_LOG(DEBUG_PROGRESS, "Hyperedges: " + std::to_string(hypergraph.getHyperEdges().size()) + " Hypernodes: " + std::to_string(hypergraph.getHypernodes().size()) + "\n");

    // Generate set E according to the paper
    std::vector<eElem> originalE = generateE(hypergraph, true);
    std::vector<eElem> e = generateE(hypergraph, false);

    // calulate hyperdegree of the hypergraph
    // We assume, that all hypernodes have the same degree
    size_t cm = e[0].count();

#ifndef NDEBUG
    for (const eElem &curE : e) {
        assert(curE.count() == cm);
    }
#endif

    // get hyperedge count of the hypergraph
    size_t m = hypergraph.getHyperEdges().size();
    // TODO uncomment
    //assert(m == cm * hypergraph.getHypernodes().size());

    DEBUG_LOG(DEBUG_PROGRESS, "Hyperdegree: " + std::to_string(cm) + "\n");

    std::vector<size_t> listOfKs(setOfKs.begin(), setOfKs.end());

    // Can skip the first cycle because that results in E = S* anyway
    for (size_t d = 1; d < m - cm; d++) {
        DEBUG_LOG(DEBUG_PROGRESS, "Running with cm+d " + std::to_string(cm + d) + "\n");
        std::vector<boost::dynamic_bitset<>> sStar = minimumKAndD(cm + d, e);

    #ifndef NDEBUG
        size_t numberOfOnes = sStar[0].count();
        for (const boost::dynamic_bitset<> &currentSStarElem : sStar) {
            assert(currentSStarElem.count() == numberOfOnes);
        }
    #endif

        size_t k = sStar.size();
        if (!listOfKs.empty()) {
            // Replace e with sStar
            e = sStar;
        } else {
            return;
        }

        size_t element;
        element = listOfKs.back();
        while (element >= k) {
            // Extract partitions
            std::vector<std::vector<uint32_t>> partitions;
            std::vector<uint32_t> hypernodes = hypergraph.getHypernodes();

            std::vector<uint32_t> assignedHypernodes;
            assignedHypernodes.reserve(hypernodes.size());

        #ifdef FAKE_DETECTION
            size_t numberOfFakes = 0;
            std::vector<size_t> fakes;
        #endif

            for (const boost::dynamic_bitset<> &currentS : sStar) {
                std::vector<uint32_t> partition;
                partition.reserve(hypernodes.size());

            #ifdef FAKE_DETECTION
                boost::dynamic_bitset<> expectedCombination(originalE[0].size());
            #endif

                // If element e & element s == element e of the original set E, then the vertex is part of
                // the partition that the element s represents because the vertex is fully contained in this partition
                for (size_t i = 0; i < originalE.size(); i++) {
                    // Additionally check if a node already is in a partition, if yes don't assign it again
                    if ((originalE[i] & currentS) == originalE[i]
                        && std::find(assignedHypernodes.begin(), assignedHypernodes.end(), hypernodes[i]) == assignedHypernodes.end()) {
                        partition.push_back(hypernodes[i]);
                        assignedHypernodes.push_back(hypernodes[i]);
                    }

                #ifdef FAKE_DETECTION
                    // If the original E element is contained in the current element of S, OR it into the expected combination.
                    if ((originalE[i] & currentS) == originalE[i]) {
                        expectedCombination |= originalE[i];
                    }
                #endif
                }

            #ifdef FAKE_DETECTION
                // Check for fake elements. A fake element is one that doesn't equal the result of ORing together
                // all contained original e elements. It is only relevant if it was actually used for a partition though.
                assert(!(expectedCombination != currentS && expectedCombination.count() == currentS.count()));
                // TODO REMOVE IF
                if (expectedCombination != currentS && expectedCombination.count() == currentS.count()) {
                    std::cerr << "bitsets have same number of ones, but are different." << std::endl;
                }

                if (expectedCombination != currentS) {
                    assert(currentS.count() >= expectedCombination.count() && "The partition can't be smaller than the expected combination.");
                    // TODO REMOVE IF
                    if (currentS.count() < expectedCombination.count()) { std::cerr << "NOPE" << std::endl; exit(1); }

                    // Check if only the partition bitstreams has ones where the expected combination has zeros and not the other way round.
                    for (size_t i = 0; i < currentS.size(); i++) {
                        // TODO REMOVE IF
                        if (!currentS[i] && expectedCombination[i]) { std::cerr << "SHOULDN'T HAPPEN LOL" << std::endl; exit(1); }
                        assert(!(!currentS[i] && expectedCombination[i]) && "The partition has a zero where the expected partition has a one, impossible!");
                    }

                    numberOfFakes += currentS.count() - expectedCombination.count();
                    fakes.push_back(partitions.size() + 1);
                }
            #endif

            #ifndef NDEBUG
                if (partition.empty()) {
                    std::cout << "Not used for partitioning at all: " << currentS << std::endl;
                }
            #endif

                assert(!partition.empty() && "A minimal subset element wasn't used for partitioning at all.");
                partitions.push_back(partition);
            }

        #ifdef FAKE_DETECTION
            if (numberOfFakes) {
                std::cout << "\033[0;31mNumber of fake elements: " << numberOfFakes << " (";
                for (size_t current : fakes) {
                    std::cout << "CPU" << current << " ";
                }
                std::cout << "\b)" << "\033[0m" << std::endl;
            }
        #endif

            // If there are no elements in sStar left but there are not enough partitions yet, fill with empties
            if (partitions.size() < element) {
                for (size_t i = partitions.size(); i < element; i++) {
                    partitions.emplace_back();
                }
            }

            assert(partitionsContainAllVertices(hypergraph, partitions));
            printDDF(element, partitions);
            listOfKs.pop_back();

            // All partitionings found, exiting
            if (listOfKs.empty()) {
                return;
            }

            element = listOfKs.back();
        }
    }

#if DEBUG > 0
    std::stringstream s;
    for (size_t current : listOfKs) {
        s << current << " ";
    }
#endif
    DEBUG_LOG(DEBUG_PROGRESS, "Missed ks: " + s.str());
    assert(false && "Couldn't find a working partitioning. This should never happen!");
}
