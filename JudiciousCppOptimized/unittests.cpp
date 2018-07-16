#include <string>
#include "gtest/gtest.h"

#include "Hypergraph.h"
#include "algorithms.h"

const std::string DATASET_PTH = "../datasets/";

/* #### Partition file read in ####
 * File format:
 * <#partitions> #<internal_nodes>
 * <partition_name> <#sites>
 * <rc_site1> <rc_site2> [...]
 */

TEST(PartitionFileReadin, simple_repeats_p0) {
    Hypergraph hg = getHypergraphFromPartitionFile(DATASET_PTH + "simple.repeats", 0);

    ASSERT_EQ(hg.getHypernodes().size(), 6);
    ASSERT_EQ(hg.getHyperEdges().size(), 15);

    /*
    for(auto node: hg.getHypernodes()) {
        ASSERT_EQ(node.size(), 3);
    }
    */
}

TEST(PartitionFileReadin, simple_repeats_p1) {
    Hypergraph hg = getHypergraphFromPartitionFile(DATASET_PTH + "simple.repeats", 1);

    ASSERT_EQ(hg.getHypernodes().size(), 4);
    ASSERT_EQ(hg.getHyperEdges().size(), 11);

    /*
    for(auto node: hg.getHypernodes()) {
        ASSERT_EQ(node.size(), 3);
    }
    */
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}