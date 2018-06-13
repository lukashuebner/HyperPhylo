#!/usr/bin/env python3

import argparse


def parse_arguments():
    parser = argparse.ArgumentParser(description="Convert a partition file to a hypergraph.")

    parser.add_argument('file', nargs=1, help='the input partition file')

    args = parser.parse_args()
    return args


def partition_file_to_hypergraph(file):
    """
    Convert a partition file to a hypergraph.
    :param file: the partition file
    :return: two lists: one containing the vertices = sites, and one containing the hyperedges = repeat classes
    (which are represented as lists of their vertices)
    """
    # Read file
    file_lines = open(file, 'r').readlines()
    file_lines = [line.split() for line in file_lines]
    number_of_sites = len(file_lines[0])

    # Initialize
    vertices = [i for i in range(number_of_sites)]
    hyperedges = []

    # Fill the hyperedges one by one
    for i in range(len(file_lines)):  # Traverse all lines (rows in the partition file)
        for cur_repeat_class in range(number_of_sites):  # Can have a maximum of number_of_sites repeat classes per row
            cur_hyperedge = []
            for j in range(number_of_sites):  # Traverse all sites (columns in the partition file)
                if int(file_lines[i][j]) == cur_repeat_class:
                    cur_hyperedge.append(j)
            if cur_hyperedge:
                hyperedges.append(cur_hyperedge)
            else:  # No site contains cur_repeat_class --> none will contain any "higher" repeat class
                break

    return vertices, hyperedges


def main():
    args = parse_arguments()
    v, e = partition_file_to_hypergraph(args.file[0])
    print(v)
    print(e)


if __name__ == "__main__":
    main()
