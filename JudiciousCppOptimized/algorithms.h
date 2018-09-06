#ifndef JUDICIOUSCPPOPTIMIZED_ALGORITHMS_H
#define JUDICIOUSCPPOPTIMIZED_ALGORITHMS_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <set>
#include <algorithm>
#include <cassert>
#include <unordered_set>

#include <boost/range/algorithm/set_algorithm.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <unordered_map>

#include "Hypergraph.h"

#define DEBUG 0
#define DEBUG_PROGRESS 1
#define DEBUG_VERBOSE 2

#define DETERMINISM
//#define FAKE_DETECTION

#if DEBUG > 0
#define DEBUG_LOG(level, message) do { if (DEBUG >= (level)) std::cout << (message) << std::flush; } while (0)
#else
#define DEBUG_LOG(level, message)
#endif

typedef std::tuple<size_t, size_t, size_t> distTriple;

// elems of the set S: maps a combination to the covered elements in E
struct sElem {
    boost::dynamic_bitset<> combination;
    // Elements of the e set in the current iteration that are covered by this combination.
    mutable std::set<size_t> coveredEElems;
    // Elements of the original e set that are covered by this combination.
    mutable std::set<size_t> coveredE0Elems;

    sElem() = default;

    // Move combination set
    explicit sElem(boost::dynamic_bitset<> combination) : combination(std::move(combination)) {
    }

    friend bool operator== (const sElem &lhs, const sElem &rhs) {
        return lhs.combination == rhs.combination;
    }

    friend bool operator!= (const sElem &lhs, const sElem &rhs) {
        return !(lhs == rhs);
    }

    friend bool operator< (const sElem &lhs, const sElem &rhs) {
        if (lhs.coveredEElems.size() == rhs.coveredEElems.size()) {
            return lhs.combination < rhs.combination;
        }

        return lhs.coveredEElems.size() < rhs.coveredEElems.size();
    }

    friend bool operator> (const sElem &lhs, const sElem &rhs) {
        return rhs < lhs;
    }

    friend bool operator<= (const sElem &lhs, const sElem &rhs) {
        return !(lhs > rhs);
    }

    friend bool operator>= (const sElem &lhs, const sElem &rhs) {
        return !(lhs < rhs);
    }
};

// elems of the set E: sets of hyperedges for each hypernode that contain a hypernode and set S*, which
// is just the set E of the next round
struct eElem {
    boost::dynamic_bitset<> combination;
    // Elements of the original e set that are covered by this combination.
    std::set<size_t> coveredE0Elems;

    // Move combination set
    explicit eElem(boost::dynamic_bitset<> combination, std::set<size_t> coveredE0Elems) :
            combination(std::move(combination)),
            coveredE0Elems(std::move(coveredE0Elems)) {
    }

    // Convert from sElem
    explicit eElem(sElem original) {
        combination = original.combination;
        coveredE0Elems = original.coveredE0Elems;
    }

    friend bool operator== (const eElem &lhs, const eElem &rhs) {
        return lhs.combination == rhs.combination;
    }

    friend bool operator!= (const eElem &lhs, const eElem &rhs) {
        return !(lhs == rhs);
    }
};

namespace std {
    template <> struct hash<sElem> {
        size_t operator()(const sElem & S) const {
            return boost::hash_value(S.combination.m_bits);
        }
    };
}

template<typename T, typename A>
inline bool containedIn(std::vector<T, A> const &vector, T element) {
    return std::find(vector.begin(), vector.end(), element) != vector.end();
}

Hypergraph getHypergraphFromPartitionFile(const std::string &filepath, int partitionNumber);

void partition(const Hypergraph &hypergraph, const std::set<size_t> &setOfKs);

void printDDF(size_t k, const std::vector<std::vector<size_t>> &partitions);


#endif //JUDICIOUSCPPOPTIMIZED_ALGORITHMS_H
