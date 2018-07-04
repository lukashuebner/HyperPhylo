#include <iostream>
#include <sstream>
#include <set>
#include <algorithm>

#include "Hypergraph.h"


Hypergraph getHypergraphFromPartitionFile(std::string filepath) {
    // TODO
    vElem v;
    eElem e;
    Hypergraph hypergraph(v, e);
    return hypergraph;
}


std::set<eElem> generateE(Hypergraph hypergraph) {
    std::set<eElem> e;

    vElem v = hypergraph.getVertices();
    eElem h = hypergraph.getEdges();
    for (int curV : v) {
        eElem curE;
        for (vElem curH : h) {
            if (curH.find(curV) != curH.end()) {  // if curV is in curH
                curE.insert(curH);
            }
        }
        e.insert(curE);
    }

    return e;
}


std::set<std::set<eElem>> generateT(eElem hyperedges, int cmPlusD) {
    // TODO Try our "only calculate fitting elements of T" trick
    std::set<std::set<eElem>> placeholder;
    return placeholder;
}


std::set<std::set<eElem>> generateS(std::set<std::set<eElem>> t, std::set<eElem> e) {
    std::set<std::set<eElem>> s = {};
    // TODO For all sets in t create one in s where s contains all e that are a subset of t
    return s;
}


std::set<std::set<eElem>> minimumKAndD(std::set<std::set<eElem>> t, std::set<eElem> e) {
    std::set<std::set<eElem>> s = generateS(t, e);
    // TODO Find minimum covering set & return correct result here
    std::set<std::set<eElem>> placeholder;
    return placeholder;
}


eElem partition(int n, int cm, int m, std::set<eElem> e, Hypergraph hypergraph) {
    /***
     * Parameters:
     *   n: number of CPUs (argument 'k' of the program call)
     *   cm: maximum hyperdegree
     *   m: number of edges in the hypergraph
     *   e: the E set
     *   hypergraph: the hypergraph
     */
     int minMaxL = cm;
     for (int d = 0; d < m - cm; d++) {
         std::set<std::set<eElem>> sStar = minimumKAndD(generateT(hypergraph.getEdges(), cm + d), e);
         int k = sStar.size();
         if (k > n) {
             // Create new e
             e = {};
             std::set<int> used_vertices = {};
             for (std::set<eElem> s : sStar) {
                 // TODO for Peter, I guess
             }
         } else {
             minMaxL = cm + d;
             // Extract partitions
             eElem v;
             for (std::set<eElem> s : sStar) {
                 vElem v_i = {};
                 for (eElem curE : s) {
                     // TODO Add "contained vertices" here
                 }
             }
             return v;
         }
     }
}


int main(int argc, char** argv) {
    std::string filepath;
    int k = 0;

    // Parse arguments
    if (argc == 3) {
        filepath = argv[1];
        std::string k_string(argv[2]);
        std::stringstream str(k_string);
        str >> k;
    } else {
        std::cout << "Usage: " << argv[0] << " partition_file k\n";
    }

    // Hypergraph hypergraph = getHypergraphFromPartitionFile(filepath);

    // Hardcoded sample hypergraph
    vElem v {0, 1, 2, 3, 4, 5};
    eElem h {{0, 1, 2}, {3, 4}, {1, 5}, {2, 4, 5}};
    Hypergraph hypergraph(v, h);

    std::set<eElem> e = generateE(hypergraph);
    int cm = 0;
    for (eElem curE : e) {
        int curSize = curE.size();
        if (curSize > cm) {
            cm = curSize;
        }
    }
    int m = hypergraph.getEdges().size();

    partition(k, cm, m, e, hypergraph);

    return 0;
}
