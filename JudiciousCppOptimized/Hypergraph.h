//
// Created by Adrian Zapletal on 04.07.18.
//

#ifndef JUDICIOUSCPP_HYPERGAPH_H
#define JUDICIOUSCPP_HYPERGAPH_H

#define BOOST_DYNAMIC_BITSET_DONT_USE_FRIENDS

#include <iostream>
#include <set>
#include <vector>
#include <boost/dynamic_bitset.hpp>
#include <boost/functional/hash.hpp>

// elems of the set of hyperedges: Hypernodes connected by this hyperedge
typedef std::vector<uint32_t> hElem;

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
