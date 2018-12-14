#!/usr/bin/env python3


import argparse
import math
import functools
import subprocess


JUDICIOUS_EXE = '../JudiciousCppOptimized/cmake-build-debug/JudiciousCpp'


def parse_args():
    parser = argparse.ArgumentParser()

    parser.add_argument('repeatsfile')
    parser.add_argument('splitsfile')

    args = parser.parse_args()
    return args


def get_partition_sizes_from_repeats_file(repeats_file):
    partition_sizes = {}
    lines = open(repeats_file, 'r').readlines()
    for line in lines:
        if line.startswith('partition'):
            split_line = line.split()
            partition_sizes[split_line[0]] = int(split_line[1])
    return partition_sizes


def calculate_k(split_proportions):
    rounded_proportions = [round(prop * 10) for prop in split_proportions]
    gcd = functools.reduce(math.gcd, rounded_proportions)
    rounded_proportions = [prop//gcd for prop in rounded_proportions]
    k = sum(rounded_proportions)
    return k, rounded_proportions


def parse_ddf(ddf):
    assignment = {}

    ddf_lines = ddf.split('\n')
    for i, line in enumerate(ddf_lines):
        if line.startswith('CPU'):
            core_name = line.split()[0]
            sites = ddf_lines[i+1].split()[2:]
            assignment[core_name] = sites

    return assignment


def reassign_split(judicious_assignment, rounded_proportions):
    """
    TODO for Lukas. Take the judicious data distribution for a split partition and reassign it to the cores via bin
    packing.
    :param judicious_assignment: A dict like this: {"CPU1": [1, 3, 37, 42], "CPU2": ...} containing as keys the CPU
    names and as values a list of the sites Judicious Partitioning has assigned to that CPU.
    :param rounded_proportions: A list containing the rounded split proportions. The sum of these proportions is k.
    :return: TODO
    """
    pass


def main():
    args = parse_args()
    repeats_file = args.repeatsfile
    splits_file = args.splitsfile

    partition_sizes = get_partition_sizes_from_repeats_file(repeats_file)

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
                k, rounded_proportions = calculate_k(split_proportions)

                # Execute Judicious Partitioning
                partition_number = partition[10:]
                judicious_partitioning_call = [JUDICIOUS_EXE, repeats_file, str(k), partition_number]
                judicious_ddf = subprocess.check_output(judicious_partitioning_call, universal_newlines=True)

                # Parse output and fit it into the data_distribution
                judicious_assignment = parse_ddf(judicious_ddf)
                reassign_split(judicious_assignment, rounded_proportions)
                # TODO Fit output into data_distribution

            else:  # partition is not split
                core = splits_file_lines[i+1].split()[0]
                partition_size = partition_sizes[split_line[0]]
                list_of_the_sites = [j for j in range(partition_size)]

                if core in data_distribution:
                    data_distribution[core][partition] = list_of_the_sites
                else:
                    data_distribution[core] = {partition: list_of_the_sites}

    # TODO Print data_distribution in the proper DDF format


if __name__ == "__main__":
    main()
