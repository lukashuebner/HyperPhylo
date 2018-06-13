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
    all_repeat_classes_found = False
    cur_repeat_class = 0
    while not all_repeat_classes_found:
        cur_hyperedge = []

        for i in range(number_of_sites):  # Traverse all sites (columns in the partition file)
            for j in range(len(file_lines)):  # Traverse all lines (rows in the partition file)
                # Add sites that contain the current repeat class to the current hyperedge
                if int(file_lines[j][i]) == cur_repeat_class and i not in cur_hyperedge:
                    cur_hyperedge.append(i)

        if cur_hyperedge:
            hyperedges.append(cur_hyperedge)
            cur_repeat_class += 1
        else:
            all_repeat_classes_found = True

    return vertices, hyperedges


def main():
    args = parse_arguments()
    v, e = partition_file_to_hypergraph(args.file[0])
    print(v)
    print(e)


if __name__ == "__main__":
    main()
