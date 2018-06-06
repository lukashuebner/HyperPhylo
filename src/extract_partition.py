#!/usr/bin/env python

import argparse


def parse_arguments():
    parser = argparse.ArgumentParser(description="Extract one single partition from a repeats file",
                                     epilog="If none of the optional arguments is supplied, the first partition "
                                            "(partition0) is extracted as a default (equally to -n 0)")

    parser.add_argument('repeats_file', nargs=1, help="the input file from which you want to extract one partition")
    choice = parser.add_mutually_exclusive_group()
    choice.add_argument('-n', '--number', nargs=1, help="specify the number of the partition you want to extract, e.g. "
                                                        "1 for the second partition (partition_1)")
    choice.add_argument('-l', '--largest', action='store_true', help="extract the largest partition")
    choice.add_argument('-s', '--smallest', action='store_true', help="extract the smallest partition")

    args = parser.parse_args()
    return args


def get_partitions_from_repeats_file(file):
    """Read the repeats file and return a 2D array that contains the partitions (each partition is an array of which
    each element is an internal node)"""
    file_lines = open(file, 'r').readlines()
    file_lines = file_lines[1:]  # Remove the first line that shows # of partitions and # of internal nodes

    partitions = []

    cur_partition = None
    for line in file_lines:
        if line.startswith('partition'):
            cur_partition = cur_partition + 1 if cur_partition is not None else 0
            partitions.append([])
        else:
            partitions[cur_partition].append(line.strip('\n'))

    return partitions


def extract_largest_partition(partitions):
    largest_len = max([len(p[0]) for p in partitions])
    return [p for p in partitions if len(p[0]) == largest_len][0]


def extract_smallest_partition(partitions):
    smallest_len = min([len(p[0]) for p in partitions])
    return [p for p in partitions if len(p[0]) == smallest_len][0]


def extract_partition_number(partitions, n):
    return partitions[n]


def extract_partition(partitions, args):
    partition = None

    if args.largest:
        partition = extract_largest_partition(partitions)
    elif args.smallest:
        partition = extract_smallest_partition(partitions)
    else:
        n = int(args.number[0]) if args.number is not None else 0
        partition = extract_partition_number(partitions, n)

    return partition


def print_partition(partition):
    for n in partition:
        print(n)


def main():
    args = parse_arguments()
    partitions = get_partitions_from_repeats_file(args.repeats_file[0])
    partition = extract_partition(partitions, args)
    print_partition(partition)


if __name__ == "__main__":
    main()
