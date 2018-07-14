//
// Created by Adrian Zapletal on 04.07.18.
//

#ifndef JUDICIOUSCPP_HYPERGAPH_H
#define JUDICIOUSCPP_HYPERGAPH_H


#include <iostream>
#include <set>
#include <vector>
#include <boost/dynamic_bitset.hpp>

// elems of the set of hyperedges: Hypernodes connected by this hyperedge
typedef std::set<uint32_t> hElem;

// elems of the set E: sets of hyperedges for each hypernode that contain a hypernode and set S*, which
// is just the set E of the next round
typedef boost::dynamic_bitset<> eElem;

// elems of the set S: maps a combination to the covered elements in E
struct sElem {
    boost::dynamic_bitset<> combination;
    std::set<size_t> coveredEElems;

    // Move combination set
    explicit sElem(boost::dynamic_bitset<> combination) : combination(std::move(combination)) {
    }

    friend bool operator== (const sElem &lhs, const sElem &rhs) {
        return lhs.combination == rhs.combination;
    }

    friend bool operator!= (const sElem &lhs, const sElem &rhs) {
        return !(lhs == rhs);
    }
};

class Hypergraph {
private:
    std::set<uint32_t> hypernodes;
    std::set<hElem> hyperedges;

public:
    Hypergraph(std::set<uint32_t> hypernodes, std::set<hElem> hyperedges) :
            hypernodes(std::move(hypernodes)),
            hyperedges(std::move(hyperedges)) {
    }

    const std::set<uint32_t> &getHypernodes() const {
        return hypernodes;
    }

    const std::set<hElem> &getHyperEdges() const {
        return hyperedges;
    }
};


#endif //JUDICIOUSCPP_HYPERGAPH_H
