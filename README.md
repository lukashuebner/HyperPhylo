# HyperPhylo
Data Distribution for Phylogenetic Inference with Site Repeats via Judicious Hypergraph Partitioning

## Introduction
#### Hypergraphs
Hypergraphs are an extension of graphs. Each vertex is allowed to cover any subset (with at least two elements) from the set of vertices.

#### Judicious Partitioning
In judicious partitioning as in "classical" graph partitioning, the graph is divided into a given number of disjoint subgraphs (blocks) such that the number of cuts of edges is minimized. In hypergraphs an edge can be cut more than once and in our instance, each cut is counted separately.
In judicious partitioning, the maximum number of hyperedges per block is minimized instead of the number of vertices per block.

## How to build & run
#### Building
Dependencies are Boost and Intel's ThreadBuildingBlocks (TBB). The programm can be build which gcc or clang. The resulting binaries built with clang had a lower runtime in our experiments.
Simply execute the following commands to build:

    cd JudiciousPartitioning
    mkdir build
    cd build
    cmake ..
    make JudiciousPartitioning

Optional flags include `-DCMAKE_CXX_FLAGS="-DNDEBUG"` to disable assertions and `-DOPENMP_ENABLED=FALSE` to disable OpenMP.

#### Running
The programm can be run as follows:

    Usage: ./JudiciousPartitioning repeats_file k1[,k2[,k3...]] [partition_number]
    
Where `partition_file` is a file describing the site repeats and `partition_number` is the number of the partition to be split. A split with the respective number of block whill be computed for each given k.

#### Repeats file format
A repeats file is generated from the partitioned MSA and a phylogenetic tree. generated from a partitioned MSA and a tree.
The repeats file starts with the number of partitions, a space, and the number of internal nodes of the treen−3).
This is followed by a blocks describing the partitions (partition blocks).
A partition block starts with the partition name, a space, and the number of sites in this partition.
Then, it contains one line per internal node of the tree.
Each element in the line corresponds to a site, and is the repeats class identifier of this site.
Elements are separated by spaces. All sites that have the same repeats class identifier belong to the same repeats class.

Example of a repeats file with two partitions and a treee with three internal nodes.
    
    2 3
    partition_0 6
    0 1 2 3 4 4
    0 0 0 1 2 3
    0 1 2 3 4 5
    partition_1 4
    0 1 2 3
    0 1 1 2
    0 1 2 3
    
#### Output format
The output describes which CPU receives which site(s) from which partition.
It contains one data block per CPU. A block starts with the CPU name, a space, and the number of partitions that CPU works on. Then, it contains one line per partition. Each line comprises the partition name, and a sequence of site identifiers, separated with spaces.
A site identifier corresponds to the index of the the site in its original input data partition, starting from 0.
Additionally, the runtime will be printed as `Runtime: xxxx ms`

Example output:

    2
    CPU1 1
    partition_0 5 0 1 2 3 4
    CPU2 2
    partition_0 1 5
    partition_1 4 0 1 2 3
    Runtime: 100 ms

