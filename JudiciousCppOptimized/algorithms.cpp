#include "Hypergraph.h"
#include "structures.h"
#include "algorithms.h"
#include "Helper.h"
#include "tlx/thread_pool.hpp"

// Key is the index of the element e that maps to the minimal distance element
// Value is a pair where
// * the first value is the index of the element if e with a minimal distance to the key element
// * the second value is the minimal distance
tbb::concurrent_unordered_map<size_t, std::pair<size_t, size_t>> minimalDistances;

// Global Threadpool
#ifndef OMP_NUM_THREADS
    tlx::ThreadPool pool;
#else
    tlx::ThreadPool pool(OMP_NUM_THREADS);
#endif

#ifdef FAKE_DETECTION
std::vector<eElem> originalE;
#endif

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

    DEBUG_LOG(DEBUG_PROGRESS, "Generating S(>=2)... ");
    DEBUG_LOG(DEBUG_VERBOSE, "\n");

    tbb::concurrent_unordered_set<sElem, std::hash<sElem>> s;
    minimalDistances.clear();

    // Run over all possible pairs in E and check if they build a possible combination
    #pragma omp parallel for schedule(dynamic)
    for (size_t firstEidx = 0; firstEidx < e.size(); firstEidx++) {
    #if DEBUG >= DEBUG_VERBOSE && _OPENMP
        if (omp_get_thread_num() == 0) DEBUG_LOG(DEBUG_PROGRESS, "Running loop for firstEidx " + std::to_string(firstEidx) + "\r");
    #elif DEBUG >= DEBUG_VERBOSE
        if (firstEidx % 10 == 0) DEBUG_LOG(DEBUG_PROGRESS, "Running loop for firstEidx " + std::to_string(firstEidx) + "\r");
    #endif
        for (size_t secondEidx = firstEidx + 1; secondEidx < e.size(); secondEidx++) {
            const eElem &firstE = e[firstEidx];
            const eElem &secondE = e[secondEidx];

            // Calculate distance
            size_t distance = firstE.combination.calculateDistance(secondE.combination);

            // Add the representation if it is a valid cm + d combination
            assert(distance % 2 == 0 && distance >= 2 && distance <= e[0].combination.numBits);
            if (distance == 2) {
                assert(firstE != secondE);
                AlignedBitArray combination = firstE.combination | secondE.combination;
                assert(combination.countOnes() == cmPlusD);

                sElem newS(std::move(combination));
                newS.coveredEElems.insert(firstEidx);
                newS.coveredEElems.insert(secondEidx);
                newS.coveredE0Elems.insert(firstE.coveredE0Elems.begin(), firstE.coveredE0Elems.end());
                newS.coveredE0Elems.insert(secondE.coveredE0Elems.begin(), secondE.coveredE0Elems.end());

                auto result = s.insert(newS);
                // Merge together if this element already exists
                if (!result.second) {
                    result.first->coveredEElems.insert(newS.coveredEElems.begin(), newS.coveredEElems.end());
                    result.first->coveredE0Elems.insert(newS.coveredE0Elems.begin(), newS.coveredE0Elems.end());
                }
            } else { // else, add to the table of minimal distances
                // Store the mapping a --> b
                auto pair = std::make_pair(firstEidx, std::make_pair(secondEidx, distance));
                auto result = minimalDistances.insert(pair);

                // If adding was not successful, there is already an entry for that element of e
                if (!result.second) {
                    // If the currently stored minimal distance is greater than this one, replace it
                    if (result.first->second.second > distance) {
                        result.first->second.first = secondEidx;
                        result.first->second.second = distance;
                    }
                }

                // Store the mapping b --> a
                auto pairReverse = std::make_pair(secondEidx, std::make_pair(firstEidx, distance));
                auto resultReverse = minimalDistances.insert(pairReverse);

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

#ifndef NDEBUG
    // TODO I'm pretty sure this doesn't actually do something ever. Can probably be removed. For now, writes are commented out.
    // Run over E and check for each element if it fits into one of the generated combinations
    // TODO This will obviously also add the initial elements of e that created the combination, so maybe we can try to avoid that?
    DEBUG_LOG(DEBUG_VERBOSE, "\n");
    #pragma omp parallel for schedule(dynamic)
    for (size_t eidx = 0; eidx < e.size(); eidx++) {
    #if DEBUG >= DEBUG_VERBOSE && _OPENMP
        if (omp_get_thread_num() == 0) DEBUG_LOG(DEBUG_VERBOSE, "Fitting element " + std::to_string(eidx + 1) + "\r");
    #elif DEBUG >= DEBUG_VERBOSE
        if (eidx % 10 == 0) DEBUG_LOG(DEBUG_VERBOSE, "Fitting element " + std::to_string(eidx + 1) + "\r");
    #endif
        const eElem &currentE = e[eidx];
        for (const sElem &currentS : s) {
            if (currentS.combination.covers(currentE.combination)) {
//                auto result = currentS.coveredEElems.insert(eidx);
//                if (result.second) {
//                    std::cout << "THIS ACTUALLY DID SOMETHING; WOW" << std::endl;
//                    assert(false);
//                }
                if (!currentS.coveredEElems.count(eidx)) {
                    assert(false && "There was an uncovered element that is covered by a created combination, we thought that never happens.");
                }
//                size_t prev = currentS.coveredE0Elems.size();
//                currentS.coveredE0Elems.insert(currentE.coveredE0Elems.begin(), currentE.coveredE0Elems.end());
//                if (prev != currentS.coveredE0Elems.size()) {
//                    std::cout << "THIS ACTUALLY DID SOMETHING; AMAZING" << std::endl;
//                    assert(false);
//                }
            }
        }
    }
#endif

    DEBUG_LOG(DEBUG_VERBOSE, "\n");
    DEBUG_LOG(DEBUG_PROGRESS, "Size S(>=2): " + std::to_string(s.size()) + "\n");
    return std::vector<sElem>(std::make_move_iterator(s.begin()), std::make_move_iterator(s.end()));
}

/**
 * Finds the minimal subset of the set S that is covering all of the set E.
 *
 * @param e The set E to cover.
 * @param s The set S as input.
 * @return The found minimal subset.
 */
std::vector<eElem> findMinimalSubset(const std::vector<eElem> &e, std::vector<sElem> &&s) {
    DEBUG_LOG(DEBUG_PROGRESS, "Searching for minimal subset S*... ");

    std::set<size_t> alreadyCovered;
    std::set<size_t> alreadyCoveredE0;
    tbb::concurrent_vector<eElem> minimalSubset;
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
    DEBUG_LOG(DEBUG_VERBOSE, "\nS(>=2) covers " + std::to_string(uniques.size()) + " unique elements of e\n");
#endif

    // As long as not all of e is covered, i.e. alreadyCovered and E differ
    // Because of how the loop works (adding diffsets), alreadyCovered and E are the same when they have the same number of elements
    while (alreadyCovered.size() != e.size()) {
        // findest longest difference set
        std::set<size_t> longestDiffset;
        sElem sElemOfLongestDiffset;
        for (sElem &currentS : s) {
            std::set<size_t> diff;
            boost::range::set_difference(currentS.coveredEElems, alreadyCovered, std::inserter(diff, diff.end()));
            if (diff.size() > longestDiffset.size()) {
                longestDiffset = diff;
                sElemOfLongestDiffset = std::move(currentS);
            }
        }

        // If there is no longest diffset, we need to fill with combinations that cover only one element of E
        if (longestDiffset.empty()) {
            break;
        }

        // Add all elements of the found longest diffset to the already covered elements of e
        alreadyCovered.insert(longestDiffset.begin(), longestDiffset.end());

        // add found longest diffset combination to the resulting minimal subset
        // skip original e elements that are already covered by other mininmal subset elements
        std::set<size_t> allCoveredE0 = sElemOfLongestDiffset.coveredE0Elems;
        sElemOfLongestDiffset.coveredE0Elems.clear();
        boost::set_difference(allCoveredE0, alreadyCoveredE0,
                std::inserter(sElemOfLongestDiffset.coveredE0Elems, sElemOfLongestDiffset.coveredE0Elems.end()));

        // Add all newly covered original e elements in the already covered original e elements
        alreadyCoveredE0.insert(sElemOfLongestDiffset.coveredE0Elems.begin(), sElemOfLongestDiffset.coveredE0Elems.end());

        minimalSubset.push_back(eElem(std::move(sElemOfLongestDiffset)));
    }

    int counter = 0;
    DEBUG_LOG(DEBUG_VERBOSE, "Elements not covered yet: " + std::to_string(e.size() - alreadyCovered.size()) + "\n");

    // Fill up coverage if needed
    if (alreadyCovered.size() != e.size()) {
        // Convert alreadyCovered to concurrent set
        tbb::concurrent_unordered_set<size_t> alreadyCoveredConcurrent(
                std::make_move_iterator(alreadyCovered.begin()), std::make_move_iterator(alreadyCovered.end()));

        #pragma omp parallel for schedule(dynamic)
        for (size_t eidx = 0; eidx < e.size(); eidx++) {
        #if DEBUG >= DEBUG_VERBOSE && _OPENMP
            if (omp_get_thread_num() == 0) DEBUG_LOG(DEBUG_VERBOSE, "Filling element " + std::to_string(eidx) + "\r");
        #elif DEBUG >= DEBUG_VERBOSE
            if (eidx % 10 == 0) DEBUG_LOG(DEBUG_VERBOSE, "Filling element " + std::to_string(eidx) + "\r");
        #endif

            // If the element of e is not already covered, generate a coverage element for it
            if (!alreadyCoveredConcurrent.count(eidx)) {
                counter++;
                // Get the minimalDistances entry for this element of e
                auto elementIterator = minimalDistances.find(eidx);
                assert(elementIterator != minimalDistances.end());
                assert(elementIterator->second.second > 2);

                AlignedBitArray combination = e[eidx].combination;
                const AlignedBitArray &otherElement = e[elementIterator->second.first].combination;

                // Flip a bit that makes the combination approach towards the element that is closest to the combination
                // by flipping a bit to 1 that is already a one in the other element
                for (size_t i = otherElement.numInts - 1; i >= 0; i--) {
                    uint64_t rightmost = otherElement[i] & ~combination[i];
                    if (rightmost) {
                        combination[i] |= (rightmost & -rightmost);
                        break;
                    }
                    assert(i != 0);
                }

                // Mark all uncovered elements of e that are covered by the created filler as covered
                for (size_t checkEidx = 0; checkEidx < e.size(); checkEidx++) {
                    const eElem &checkEElem = e[checkEidx];
                    if (combination.covers(checkEElem.combination) && checkEidx != eidx) {
                        alreadyCoveredConcurrent.insert(checkEidx);
                    }
                }

                // Sanity Check: Does this element cover an element e (the filled one) that isn't already covered by a combination of other minimal subset elements?
//            #ifndef NDEBUG
//                std::set<size_t> coveredSites;
//                #pragma omp parallel for schedule(dynamic)
//                for (size_t i = 0; i < minimalSubset.size(); i++) {
//                    const AlignedBitArray &current = minimalSubset[i].combination;
//                    // Does it cover this element of e? If yes insert index into covered set.
//                    const AlignedBitArray &eElement = e[eidx].combination;
//                    if (current.covers(eElement)) {
//                        std::cerr << "Combination " << combination << " doesn't cover a new e element";
//                        std::cerr << ", especially the filled element is already covered by another element of the minimal subset." << std::endl;
//                        std::cerr << "Combination contains of elements with " << eElement.countOnes() << " and " << otherElement.countOnes() << " ones." << std::endl;
//                        std::cerr << "This occured in filling element " << eidx << std::endl;
//                        assert(false);
//                    }
//                }
//            #endif

                // Insert the covering element
                minimalSubset.push_back(eElem(std::move(combination), e[eidx].coveredE0Elems));
            }
        }
    }

#ifndef NDEBUG
    // Check if each original e element only occures once
    std::set<size_t> coverage;
    for (const auto &current : minimalSubset) {
        for (const auto &coveredE0Elem : current.coveredE0Elems) {
            assert(coverage.insert(coveredE0Elem).second && "An original e element is covered more than once!");
        }
    }
#endif

    DEBUG_LOG(DEBUG_VERBOSE, "\n");
    DEBUG_LOG(DEBUG_PROGRESS, "Size S*: " + std::to_string(minimalSubset.size()) + "\n");

    return std::vector<eElem>(std::make_move_iterator(minimalSubset.begin()), std::make_move_iterator(minimalSubset.end()));
}

/**
 * Runs the minimum k and d algorithm as shown in the paper.
 *
 * @param t cmPlusD The number of elements per combination in T.
 * @param e The set E as described in generateE.
 * @return The found minimal set. The size of the minimal set is the value k.
 */
std::vector<eElem> minimumKAndD(size_t cmPlusD, const std::vector<eElem> &e) {
    DEBUG_LOG(DEBUG_PROGRESS, "Running minKD\n");
    return findMinimalSubset(e, generateS(cmPlusD, e));
}

/**
 * Generates a set E containing a set e_i for each hypernode i without duplicates.
 * Each set e_i contains all hyperedges that contain the hypernode i.
 *
 * @param hypergraph The input hypergraph
 * @return the set E without duplicates
 */
std::vector<eElem> generateE(const Hypergraph &hypergraph) {
    DEBUG_LOG(DEBUG_PROGRESS, "Generating E... ");
    DEBUG_LOG(DEBUG_VERBOSE, "\n");
    const std::vector<uint32_t> &hypernodes = hypergraph.getHypernodes();
    std::vector<hElem> hyperedges = hypergraph.getHyperEdges();
    std::reverse(hyperedges.begin(), hyperedges.end());

    // Set covered e element for each entry
    std::vector<eElem> e(hypernodes.size(), eElem(hyperedges.size()));
    for (size_t i = 0; i < e.size(); i++) {
        e[i].coveredE0Elems.insert(i);
    }

    // Set all combinations accordingly
    for (size_t hyperedgeIdx = 0; hyperedgeIdx < hyperedges.size(); hyperedgeIdx++) {
        DEBUG_LOG(DEBUG_VERBOSE, "Generating entry for hyperedge ID " + std::to_string(hyperedgeIdx) + "\r");
        for (uint32_t node : hyperedges[hyperedgeIdx]) {
            e[node].combination.setBit(hyperedgeIdx);
        }
    }
    DEBUG_LOG(DEBUG_VERBOSE, "\nDone.\n");

#ifdef FAKE_DETECTION
    originalE = e;
#endif

#if DEBUG > 0
    size_t entryIdx = 0;
#endif

    DEBUG_LOG(DEBUG_VERBOSE, "Removing Duplicates...\n");
    std::set<eElem> tempSet;
    for (auto &entry : e) {
        DEBUG_LOG(DEBUG_VERBOSE, "Checking entry ID " + std::to_string(entryIdx++) + "\r");
        // Move is fine here because entry is only used again if the insert failed --> no move happened
        auto result = tempSet.insert(std::move(entry));
        if (!result.second) {
            result.first->coveredE0Elems.insert(entry.coveredE0Elems.begin(), entry.coveredE0Elems.end());
        }
    }
    std::vector<eElem> noDuplicates(std::make_move_iterator(tempSet.begin()), std::make_move_iterator(tempSet.end()));
    DEBUG_LOG(DEBUG_VERBOSE, "\nDone.\n");

    DEBUG_LOG(DEBUG_PROGRESS, "Size E: " + std::to_string(e.size()) + ", Size E(nodups): " + std::to_string(noDuplicates.size()) + "\n");
    return noDuplicates;
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
    std::vector<eElem> e = generateE(hypergraph);

    // calulate hyperdegree of the hypergraph
    // We assume that all hypernodes have the same degree
    size_t cm = e[0].combination.countOnes();

#ifndef NDEBUG
    for (const eElem &curE : e) {
        assert(curE.combination.countOnes() == cm);
    }
#endif

    // get hyperedge count of the hypergraph
    size_t m = hypergraph.getHyperEdges().size();

    DEBUG_LOG(DEBUG_PROGRESS, "Hyperdegree: " + std::to_string(cm) + "\n");

    std::vector<size_t> listOfKs(setOfKs.begin(), setOfKs.end());
    std::vector<eElem> sStar;
    // Can skip the first cycle because that results in E = S* anyway
    for (size_t d = 1; d < m - cm; d++) {
        DEBUG_LOG(DEBUG_PROGRESS, "Running with cm+d " + std::to_string(cm + d) + "\n");
        sStar = minimumKAndD(cm + d, e);

    #ifndef NDEBUG
        size_t numberOfOnes = sStar[0].combination.countOnes();
        for (const eElem &currentSStarElem : sStar) {
            assert(currentSStarElem.combination.countOnes() == numberOfOnes);
        }
    #endif

        size_t k = sStar.size();
        if (!listOfKs.empty()) {
            // Replace e with sStar
            e = std::move(sStar);
        } else {
            return;
        }

        size_t element;
        element = listOfKs.back();
        while (element >= k) {
            // Extract partitions
            std::vector<std::vector<size_t>> partitions;

        #ifdef FAKE_DETECTION
            size_t numberOfFakes = 0;
            std::vector<size_t> fakes;
        #endif

            for (const eElem &currentSStarElem : e) {
                // Convert covered elements to partition
                std::vector<size_t> partition(currentSStarElem.coveredE0Elems.begin(), currentSStarElem.coveredE0Elems.end());

            #ifndef NDEBUG
                if (partition.empty()) {
                    std::cerr << "A partition element wasn't used for partitioning at all: " << currentSStarElem.combination << std::endl;
                    assert(!partition.empty());
                }
            #endif

                partitions.push_back(std::move(partition));

            #ifdef FAKE_DETECTION
                AlignedBitArray expectedCombination(e[0].combination.numBits);

                DEBUG_LOG(DEBUG_VERBOSE, "Covers:");
                size_t currentEElemIdx = 0;
                for (const auto &currentEElem : originalE) {
                    // If the original E element is contained in the current element of S, OR it into the expected combination.
                    if ((currentEElem.combination & currentSStarElem.combination) == currentEElem.combination) {
                        DEBUG_LOG(DEBUG_VERBOSE, " " + std::to_string(currentEElemIdx));
                        expectedCombination = expectedCombination | currentEElem.combination;
                    }
                    currentEElemIdx++;
                }

                // Check for fake elements. A fake element is one that doesn't equal the result of ORing together
                // all contained original e elements. It is only relevant if it was actually used for a partition though.
                assert(!(expectedCombination != currentSStarElem.combination && expectedCombination.countOnes() == currentSStarElem.combination.countOnes()));
                if (expectedCombination != currentSStarElem.combination) {
                    assert(currentSStarElem.combination.countOnes() >= expectedCombination.countOnes() && "The partition can't be smaller than the expected combination.");
                    // Check if only the partition bitstreams has ones where the expected combination has zeros and not the other way round.
                    for (size_t i = 0; i < currentSStarElem.combination.numBits; i++) {
                        assert(!(!currentSStarElem.combination.getBit(i) && expectedCombination.getBit(i)) && "The partition has a zero where the expected partition has a one, impossible!");
                    }

                    numberOfFakes += currentSStarElem.combination.countOnes() - expectedCombination.countOnes();
                    fakes.push_back(partitions.size() + 1);
                }
            #endif
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
