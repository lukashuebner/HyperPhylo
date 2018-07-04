//
// Created by Adrian Zapletal on 04.07.18.
//

#ifndef JUDICIOUSCPP_HYPERGAPH_H
#define JUDICIOUSCPP_HYPERGAPH_H


#include <iostream>
#include <set>


typedef std::set<int> vElem;
typedef std::set<vElem> eElem;


class Hypergraph {
private:
    vElem vertices;
    eElem edges;

public:
    Hypergraph(const vElem &vertices, const eElem &edges) : vertices(vertices), edges(edges) {}

    const vElem &getVertices() const {
        return vertices;
    }

    void setVertices(const vElem &vertices) {
        Hypergraph::vertices = vertices;
    }

    const eElem &getEdges() const {
        return edges;
    }

    void setEdges(const eElem &edges) {
        Hypergraph::edges = edges;
    }
};


#endif //JUDICIOUSCPP_HYPERGAPH_H
