#ifndef JUDICIOUSCPP_HYPERGAPH_H
#define JUDICIOUSCPP_HYPERGAPH_H

#define BOOST_DYNAMIC_BITSET_DONT_USE_FRIENDS

#include <iostream>
#include <set>
#include <vector>
#include <boost/dynamic_bitset.hpp>
#include <boost/functional/hash.hpp>
#include "structures.h"

// elems of the set of hyperedges: Hypernodes connected by this hyperedge
typedef std::vector<uint32_t> hElem;

// elems of the set S: maps a combination to the covered elements in E
struct sElem {
    AlignedBitArray combination;
    // Elements of the e set in the current iteration that are covered by this combination.
    mutable std::set<size_t> coveredEElems;
    // Elements of the original e set that are covered by this combination.
    mutable std::set<size_t> coveredE0Elems;

    sElem() = default;

    // Move combination set
    explicit sElem(AlignedBitArray combination) : combination(combination) {
    }

    bool operator==(const sElem &rhs) const {
        return this->combination == rhs.combination;
    }

    bool operator!=(const sElem &rhs) const {
        return !(*this == rhs);
    }

    bool operator<(const sElem &rhs) const {
        if (this->coveredEElems.size() == rhs.coveredEElems.size()) {
            return this->combination < rhs.combination;
        }

        return this->coveredEElems.size() < rhs.coveredEElems.size();
    }

    bool operator>(const sElem &rhs) const {
        return rhs < *this;
    }

    bool operator<=(const sElem &rhs) const {
        return !(*this > rhs);
    }

    bool operator>=(const sElem &rhs) const {
        return !(*this < rhs);
    }
};


// elems of the set E: sets of hyperedges for each hypernode that contain a hypernode and set S*, which
// is just the set E of the next round
struct eElem {
    AlignedBitArray combination;
    // Elements of the original e set that are covered by this combination.
    std::set<size_t> coveredE0Elems;

    // Move combination set
    explicit eElem(AlignedBitArray combination, std::set<size_t> coveredE0Elems) :
        combination(combination),
        coveredE0Elems(std::move(coveredE0Elems)) {
    }

    // Convert from sElem
    explicit eElem(sElem original) : combination(original.combination), coveredE0Elems(original.coveredE0Elems) {
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
        size_t operator()(const sElem &s) const {
            return std::hash<AlignedBitArray>{}(s.combination);
        }
    };
}

class Hypergraph {
private:
    std::vector<uint32_t> hypernodes;
    std::vector<hElem> hyperedges;

public:
    Hypergraph(std::vector<uint32_t> hypernodes, std::vector<hElem> hyperedges) :
        hypernodes(std::move(hypernodes)),
        hyperedges(std::move(hyperedges)) {
    }

    const std::vector<uint32_t> &getHypernodes() const {
        return hypernodes;
    }

    const std::vector<hElem> &getHyperEdges() const {
        return hyperedges;
    }

    const uint32_t getHypernodeDegree(uint32_t nodeId) const {
        uint32_t degree = 0;
        for (auto edge: getHyperEdges()) {
            if(std::find(edge.begin(), edge.end(), nodeId) != edge.end()) {
                degree++;
            }
        }
        return degree;
    }
};


#endif //JUDICIOUSCPP_HYPERGAPH_H
