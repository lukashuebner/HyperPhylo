//
// Created by Adrian Zapletal on 04.07.18.
//

#ifndef JUDICIOUSCPP_HYPERGAPH_H
#define JUDICIOUSCPP_HYPERGAPH_H


#include <iostream>
#include <set>

// elems of the set of hyperedges: Hypernodes connected by this hyperedge
typedef std::set<uint32_t> hElem;

// elems of the set E: sets of hyperedges for each hypernode that contain a hypernode
struct eElem {
    // Hyperedges in this element of E
    std::set<hElem> hyperedges;
    // Hypernodes covered by hyperedges in this element of E
    std::set<uint32_t> coveredHypernodes;

    friend bool operator< (const eElem& lhs, const eElem& rhs) {
        return lhs.hyperedges < rhs.hyperedges;
    }

    friend bool operator> (const eElem& lhs, const eElem& rhs) {
        return rhs < lhs;
    }

    friend bool operator<= (const eElem& lhs, const eElem& rhs) {
        return !(lhs > rhs);
    }

    friend bool operator>= (const eElem& lhs, const eElem& rhs) {
        return !(lhs < rhs);
    }

    friend bool operator== (const eElem& lhs, const eElem& rhs) {
        return lhs.hyperedges == rhs.hyperedges;
    }

    friend bool operator!= (const eElem& lhs, const eElem& rhs) {
        return !(lhs == rhs);
    }
};

// elems of the set S:  contains a set s for each combination t in T containing all e that are a subset of
//                      the element t belonging to s
typedef std::set<eElem> sElem;

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
