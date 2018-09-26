#include "Hypergraph.h"

// ##### Constructors
Hypergraph::Hypergraph(std::vector<uint32_t> hypernodes, std::vector<std::vector<uint32_t>> hyperedges) :
        hypernodes(std::move(hypernodes)),
        hyperedges(std::move(hyperedges)) {
}

// ##### Getters/Setters
const std::vector<uint32_t> &Hypergraph::getHypernodes() const {
    return hypernodes;
}

const std::vector<hElem> &Hypergraph::getHyperEdges() const {
    return hyperedges;
}

const uint32_t Hypergraph::getHypernodeDegree(uint32_t nodeId) const {
    uint32_t degree = 0;
    for (auto edge: getHyperEdges()) {
        if(std::find(edge.begin(), edge.end(), nodeId) != edge.end()) {
            degree++;
        }
    }
    return degree;
}
