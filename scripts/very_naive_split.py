#!/usr/bin/env python3

import argparse
import sys


def parse_arguments():
    parser = argparse.ArgumentParser(description="Split one partition of a MSA to k CPUs just by splitting the sites "
                                                 "into k blocks.")

    parser.add_argument('partition_file', help="the partition file you want to split")
    parser.add_argument('list_of_block_sizes', type=str,
                        help="a comma separated list of block numbers to produce output for")

    args = parser.parse_args()
    return args


def get_number_of_sites_from_partition_file(file):
    partition_lines = open(file, 'r').readlines()
    partition_lines = partition_lines[2:]
    first_partition = partition_lines[0].split()
    return len(first_partition)


def split_very_naive(n, k):
    """Split n sites into k blocks of roughly same size."""
    split = []
    splitsize = float(n) / k
    n_range = range(n)
    for i in range(k):
        split.append(n_range[int(round(i * splitsize)):int(round((i+1) * splitsize))])
    return split


def print_split(split, k):
    """Print the split in the format of a data distribution file."""
    # Already included "number of partitions" so the code can be extended more easily later (hopefully)
    number_of_partitions = 1

    print(k)
    for i in range(k):
        print('CPU' + str(i + 1), str(number_of_partitions))
        for j in range(number_of_partitions):
            print("partition_" + str(j), len(split[i]), end=' ')
            for l in split[i]:
                print(str(l), end=' ')
            print()


def main():
    args = parse_arguments()
    number_of_sites = get_number_of_sites_from_partition_file(args.partition_file)
    list_of_ks = sorted([int(x) for x in args.list_of_block_sizes.split(",")], reverse=True)

    for k in list_of_ks:
        split = split_very_naive(number_of_sites, k)
        print_split(split, k)


if __name__ == "__main__":
    main()
