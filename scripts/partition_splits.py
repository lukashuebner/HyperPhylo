#!/usr/bin/env python3


import argparse
import math
import functools
import subprocess
from enum import Enum

JUDICIOUS_EXE = '../JudiciousPartitioning/cmake-build-debug/JudiciousCpp'


class InternalNode:
    sites = [] # Stores the repeat class for each site

    def __init__(self, sites):
        assert(len(set(sites)) == max(sites) + 1)
        self.sites = sites

    def __repr__(self):
        return "InternalNode(sites: %d)" % len(self.sites)

    def number_of_repeat_classes(self, subset_of_sites=None):
        if subset_of_sites == None:
            subset_of_rcs = self.sites
        else:
            subset_of_rcs = [self.sites[idx] for idx in subset_of_sites]
        return len(set(subset_of_rcs))

    def number_of_sites(self):
        return len(self.sites)


class Partition:
    id = ""
    internal_nodes = []

    def __init__(self, id, internal_nodes):
        self.id = id
        self.internal_nodes = [InternalNode(sites) for sites in internal_nodes]

    def __repr__(self):
        return "Partition (id = %s, num_sites: %d, num_rcs: %s)" \
            % (self.id, self.get_num_sites(), self.get_number_of_repeat_classes())

    def get_num_sites(self):
        return self.internal_nodes[0].number_of_sites()

    def get_number_of_repeat_classes(self, subset_of_sites=None):
        return sum([node.number_of_repeat_classes(subset_of_sites) for node in self.internal_nodes])


def parse_args():
    parser = argparse.ArgumentParser()

    parser.add_argument('repeatsfile')
    parser.add_argument('splitsfile')

    args = parser.parse_args()
    return args


def get_partitions_from_repeats_file(repeats_file):
    """
    Get information on the number of sites and repeat classes per partition from the given repeats file.
    :param repeats_file: The repeats file for which to get the information
    :return: Dict with
        key: partition id
        value: the Partition object for that partition
    """
    def partition_iterator(repeats_file):
        class State(Enum):
            IN_HEADER = 0,
            HEADER_READ = 4,
            IN_PARTITION = 1,
            NEXT_PARTITION = 2,
            EOF = 3

        lines = open(repeats_file, 'r').read().splitlines()  # Gets rid of trailing \n

        partition_id = ""
        partition_sites = []
        partitions_found = num_partitions = 0
        num_rcs_per_site = 0
        rcs_found = 0
        sites_in_this_partition = 0
        state = State.IN_HEADER
        for line in lines:
            if state == State.IN_HEADER:
                num_partitions = int(line.split()[0])
                num_rcs_per_site = int(line.split()[1])
                state = State.HEADER_READ
            if line.startswith("partition"):
                # Is this the first partition?
                if state == State.HEADER_READ:
                    pass
                # else, close the last partition
                elif state == State.IN_PARTITION:
                    assert(rcs_found == num_rcs_per_site)
                    yield(partition_id, partition_sites)
                else:
                    assert("Failed to parse repeats file")

                state = State.NEXT_PARTITION
                partitions_found += 1
                rcs_found = 0
                split_line = line.split()
                partition_id = split_line[0]
                sites_in_this_partition = int(split_line[1])
                partition_sites = []
                state = State.IN_PARTITION
            elif state == State.IN_PARTITION:
                # One more line -> one additional repeat class per site
                rcs_found += 1
                split_line = line.split()
                assert(len(split_line) == sites_in_this_partition)
                repeat_classes = [int(rc) for rc in split_line]
                partition_sites.append(repeat_classes)
            else:
                assert("Error parsing repeats file")

        assert (rcs_found == num_rcs_per_site)
        yield (partition_id, partition_sites)
        state = State.EOF

        # Done parsing the file
        assert(state == State.EOF)
        assert(partitions_found == num_partitions)

    partitions = {}
    for (partition_id, partition_sites) in partition_iterator(repeats_file):
        partition = Partition(partition_id, partition_sites)
        partitions[partition.id] = partition
    return partitions


def calculate_rounded_proportions(split_proportions):
    rounded_proportions = []

    factor = 10
    rounded_proportions_are_ok = False
    while not rounded_proportions_are_ok:
        rounded_proportions = [round(prop * factor) for prop in split_proportions]
        factor += 10
        if all(prop >= 2 for prop in rounded_proportions):
            rounded_proportions_are_ok = True

    return rounded_proportions


def execute_judicious_partitioning(repeats_file, k, partition):
    partition_number = partition[10:]
    judicious_partitioning_call = [JUDICIOUS_EXE, repeats_file, str(k), partition_number]
    judicious_ddf = subprocess.check_output(judicious_partitioning_call, universal_newlines=True)
    return judicious_ddf


def parse_ddf(ddf):
    assignment = {}

    ddf_lines = ddf.split('\n')
    for i, line in enumerate(ddf_lines):
        if line.startswith('CPU'):
            core_name = line.split()[0]
            sites = [int(site) for site in ddf_lines[i+1].split()[2:]]
            assignment[core_name] = sites

    return assignment


def reassign_split(judicious_assignment, partition, rounded_proportions):
    """
    Take the judicious data distribution for a split partition and reassign it to the cores via bin packing.
    :param judicious_assignment: A dict like this: {"CPU1": [1, 3, 37, 42], "CPU2": ...} containing as keys the CPU
    names and as values a list of the sites Judicious Partitioning has assigned to that CPU.
    :param partition: The partition object of the partition being split
    :param rounded_proportions: A list containing the rounded split proportions. The sum of these proportions is k.
    :return: new dict {"CPU1": [1, 3, 37, 42], "CPU2": ...}, such that the split is closer to rounded_proportions
    """
    # Calculate the number of repeat classes that are currently assigned to each CPU
    def get_rcs_by_cpu(assignment, partition):
        return {
            cpu: partition.get_number_of_repeat_classes(assignment[cpu]) for cpu in assignment
        }

    rcs_by_cpu_judicious = get_rcs_by_cpu(judicious_assignment, partition)
    num_rcs_in_judicious_partitions = sum(rcs_by_cpu_judicious.values())
    assert(num_rcs_in_judicious_partitions >= partition.get_number_of_repeat_classes())

    # Calculate, how many repeat classes we want to have on every CPU
    # The new number of CPUs is *smaller* than the number of CPUs currently used
    target_rcs_by_cpu = {
        "CPU" + str(cpu_id):
            rounded_proportions[cpu_id] / sum(rounded_proportions) * partition.get_number_of_repeat_classes()
        for cpu_id in range(len(rounded_proportions))
    }

    # Start assigning sites to CPUs. All the sites of one CPU in the given assignment will be assigned to exactly one
    # CPU of the new assignment. This means we are only agglomerating CPUs! This guarantees, that the sum of repeat
    # classes over all CPUs after this operation will be at most as large as before this operation.
    new_assignment = { cpu: [] for cpu in target_rcs_by_cpu }

    # Iterate over the CPUs, sorted by the number of repeat-classes they have in decreasing order
    for old_cpu, _ in sorted(rcs_by_cpu_judicious.items(), key=lambda kv: kv[1], reverse=True):
        # Which target CPU is missing the most rcs to hit its rc-goal? Assign the sites of the current (old) CPU to it
        # We are trying to hit a moving target here. As CPUs are agglomerated, the number of total repeat classes
        # decreases. This means we have to recalculate the rc-goal of every CPU in every iteration. TODO
        rcs_by_cpu_current = get_rcs_by_cpu(new_assignment, partition)
        missing_rcs_by_cpu = { cpu: target_rcs_by_cpu[cpu] - rcs_by_cpu_current[cpu] for cpu in new_assignment }
        cpu_missing_most_rcs = max(missing_rcs_by_cpu.items(), key=lambda kv: kv[1])[0]
        new_assignment[cpu_missing_most_rcs].extend(judicious_assignment[old_cpu])

    # Do some sanity checks and return
    assert(sum(get_rcs_by_cpu(new_assignment, partition).values()) >=
           partition.get_number_of_repeat_classes())
    assert(sum(get_rcs_by_cpu(new_assignment, partition).values()) <=
           sum(get_rcs_by_cpu(judicious_assignment, partition).values()))
    return new_assignment


def print_data_distribution_as_ddf(data_distribution):
    data_distribution = {int(key[4:]): data_distribution[key] for key in data_distribution.keys()}

    print(len(data_distribution))
    for key in sorted(data_distribution):
        value = data_distribution[key]
        print('CPU' + str(key), len(value))
        for partition_name, sites in value.items():
            print(partition_name, len(sites), end=' ')
            for site in sorted(sites):
                print(site, end=' ')
            print()


def main():
    args = parse_args()
    repeats_file = args.repeatsfile
    splits_file = args.splitsfile

    partitions = get_partitions_from_repeats_file(repeats_file)

    data_distribution = {}
    # Format of this: {'Core0': {'partition_0': [0,1,2,3]}} --> Core0 gets assigned the sites 0-4 from partition_0

    # Parse splits file
    splits_file_lines = open(splits_file, 'r').readlines()

    for i, line in enumerate(splits_file_lines):
        split_line = line.split()
        if line.startswith('partition'):
            partition = split_line[0]
            number_of_cores = int(split_line[1])

            if number_of_cores > 1:  # partition is split
                core_proportions = {}
                for j in range(1, number_of_cores+1):
                    cur = splits_file_lines[i+j].split()
                    core_proportions[cur[0]] = cur[1]

                split_proportions = [float(prop) for prop in core_proportions.values()]
                rounded_proportions = calculate_rounded_proportions(split_proportions)
                k = sum(rounded_proportions)

                judicious_ddf = execute_judicious_partitioning(repeats_file, k, partition)

                # Parse output and fit it into the data_distribution
                judicious_assignment = parse_ddf(judicious_ddf)
                cur_partition_obj = partitions[partition]
                new_assignment = reassign_split(judicious_assignment, cur_partition_obj, rounded_proportions)

                cores = []
                for j in range(1, number_of_cores+1):
                    cur_core = splits_file_lines[i+j].split()[0]
                    cores.append(cur_core)

                list_of_the_sites = []
                for sites in new_assignment.values():
                    list_of_the_sites.append(sites)

                assert len(cores) == len(list_of_the_sites)

                for core, sites in zip(cores, list_of_the_sites):
                    if core in data_distribution:
                        data_distribution[core][partition] = sites
                    else:
                        data_distribution[core] = {partition: sites}

            else:  # partition is not split
                core = splits_file_lines[i+1].split()[0]
                partition_size = partitions[split_line[0]].get_num_sites()
                list_of_the_sites = [j for j in range(partition_size)]

                if core in data_distribution:
                    data_distribution[core][partition] = list_of_the_sites
                else:
                    data_distribution[core] = {partition: list_of_the_sites}

    print_data_distribution_as_ddf(data_distribution)


if __name__ == "__main__":
    main()
