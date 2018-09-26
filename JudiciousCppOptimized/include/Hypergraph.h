#ifndef JUDICIOUSCPP_HYPERGAPH_H
#define JUDICIOUSCPP_HYPERGAPH_H

#include <algorithm>
#include <vector>
#include <stdint.h>

// elems of the set of hyperedges: Hypernodes connected by this hyperedge
typedef std::vector<uint32_t> hElem;

class Hypergraph {
private:
    std::vector<uint32_t> hypernodes;
    std::vector<hElem> hyperedges;

public:
    // ##### Constructors
    Hypergraph(std::vector<uint32_t> hypernodes, std::vector<hElem> hyperedges);

    // ##### Getters/Setters
    const std::vector<uint32_t> &getHypernodes() const;
    const std::vector<hElem> &getHyperEdges() const;
    const uint32_t getHypernodeDegree(uint32_t nodeId) const;
};


#endif //JUDICIOUSCPP_HYPERGAPH_H
