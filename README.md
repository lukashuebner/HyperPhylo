# HyperPhylo
Judicious Graph Partitioning 

## Introduction
#### Hypergraphs
Hypergraphs are an extension of graphs. Each vertex is allowed to cover any subset (with at least two elements) from the set of vertices.

#### Judicious Partitioning
In judicious partitioning as in "classical" graph partitioning, the graph is divided into a given number of disjoint subgraphs (blocks) such that the number of cuts of edges is minimized. In hypergraphs an edge can be cut more than once and in our instance, each cut is counted separately.
In judicious partitioning, the maximum number of hyperedges per block is also minimized (not balanced).

## How to build & run
#### Building
Dependencies are Boost and Intel's ThreadBuildingBlocks (TBB). The programm can be build which gcc or clang. The resulting binaries built with clang had a lower runtime in our experiments.
Simply execute the following commands to build:

    cd JudiciousPartitioning
    mkdir build
    cd build
    cmake ..
    make JudiciousPartitioning

Optional flags include `-DCMAKE_CXX_FLAGS="-DNDEBUG"` to disable debugging and `-DOPENMP_ENABLED=FALSE` to disable OpenMP.

#### Running
The programm can be run as follows:

    Usage: ./JudiciousPartitioning partition_file k1[,k2[,k3...]] [partition_number]
    
Where `partition_file` is a file describing the MSA paritioning in METIS format and `partition_number` is the number of the partition to be split. A split with the respective number of block whill be computed for each given $k$.
