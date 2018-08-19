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

    for(auto nodeId: hg.getHypernodes()) {
        ASSERT_EQ(hg.getHypernodeDegree(nodeId), 3);
    }
}

TEST(PartitionFileReadin, simple_repeats_p1) {
    Hypergraph hg = getHypergraphFromPartitionFile(DATASET_PTH + "simple.repeats", 1);

    ASSERT_EQ(hg.getHypernodes().size(), 4);
    ASSERT_EQ(hg.getHyperEdges().size(), 11);

    for(auto nodeId: hg.getHypernodes()) {
        ASSERT_EQ(hg.getHypernodeDegree(nodeId), 3);
    }
}

TEST(PartitionFileReadin, invalid_file) {
    ASSERT_EXIT(
        Hypergraph hg = getHypergraphFromPartitionFile(DATASET_PTH + "non_existing_file.nonono", 1),
        ::testing::ExitedWithCode(1),
        "Error: Input file does not exist!");
}

TEST(PartitionFileReadin, invalid_partition) {
    ASSERT_EXIT(
        Hypergraph hg = getHypergraphFromPartitionFile(DATASET_PTH + "simple.repeats", 2),
        ::testing::ExitedWithCode(2),
        "Error: Invalid partition!");
    ASSERT_EXIT(
            Hypergraph hg = getHypergraphFromPartitionFile(DATASET_PTH + "simple.repeats", -1),
            ::testing::ExitedWithCode(2),
            "Error: Invalid partition!");
}

/* #### Partition function
*/

bool partitionsContainAllVertices(Hypergraph hypergraph, std::vector<std::vector<uint32_t>> partitions) {
    std::vector<bool> nodeCovered(hypergraph.getHypernodes().size(), false);
    for (auto partition: partitions) {
        for (auto node: partition) {
            nodeCovered[node] = true;
        }
    }

    if(std::find(nodeCovered.begin(), nodeCovered.end(), false) != nodeCovered.end()) {
        return false;
    } else {
        return true;
    }
}

class PartitionFunction : public ::testing::TestWithParam<const char*> {};

TEST_P(PartitionFunction, allNodesAreCoveredByPartitions) {
    Hypergraph hypergraph = getHypergraphFromPartitionFile(DATASET_PTH + GetParam(), 0);
    std::vector<std::vector<uint32_t>> partitions = partition(2, hypergraph);
    ASSERT_TRUE(partitionsContainAllVertices(hypergraph, partitions));
}

INSTANTIATE_TEST_CASE_P(
    simpePartitions,
    PartitionFunction,
    ::testing::Values("simple.repeats", "simple2.repeats", "example.repeats")
);

TEST_P(PartitionFunction, bothPartitionsWork) {
    for (int k = 0; k <= 1; k++) {
        Hypergraph hypergraph = getHypergraphFromPartitionFile(DATASET_PTH + "simple2.repeats", 0);
        std::vector<std::vector<uint32_t>> partitions = partition(2, hypergraph);
        ASSERT_TRUE(partitionsContainAllVertices(hypergraph, partitions));
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}