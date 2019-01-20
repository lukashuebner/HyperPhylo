#!/usr/bin/env python3

import argparse
import os
import re


# ALGORITHMS = ['judicious', 'naive']
ALGORITHMS = ['rdda', 'hybrid']
MAX = '8192'
VALUES_OF_K = ['2', '4', '8', '12', '16', '24', '32', '48', '64', '96', '128', '160', '200', '256', '384', '512',
               '768', '1024', '1536', '2048', '3072', '4096', MAX]


def parse_args():
    parser = argparse.ArgumentParser(description='Take a bunch of rcccount files and create a csv that contains all '
                                                 'the results\n'
                                                 'in an ordered fashion such that one can create cute graphs from '
                                                 'it.\n\n'
                                                 'To use this script, all rcccount result files have to be located in '
                                                 'one\n'
                                                 'directory and must comply with the following naming scheme:\n'
                                                 '  <algorithm>_<repeats_name>_<value_of_k>.rcccount\n'
                                                 'e.g. for the repeats file 404.repeats, files must be named like '
                                                 'this:\n'
                                                 '  rdda_404_2.rcccount\n'
                                                 '  rdda_404_4.rcccount\n'
                                                 '  ...\n'
                                                 '  hybrid_404_8192.rcccount\n',
                                     formatter_class=argparse.RawDescriptionHelpFormatter)

    parser.add_argument('repeats_file', nargs=1, help='the repeats file for which you want to check rccc results')
    parser.add_argument('results_folder', nargs=1, help='the folder where all rcccount result files are located')
    parser.add_argument('repeats_name', nargs=1, help='the name of the repeats file as in the rcccount file names')
    parser.add_argument('--boxplot', action="store_true", help='changes output csv content boxplot requirement')

    args = parser.parse_args()
    return args


def get_number_of_inner_nodes(repeats_file):
    lines = open(repeats_file, 'r').readlines()
    split_first_line = lines[0].split()
    number_of_inner_nodes = int(split_first_line[1])
    return number_of_inner_nodes


def get_rcccount_files(folder, repeats_name, algorithm):
    files = []
    for k in VALUES_OF_K:
        cur_filename = os.path.join(folder, algorithm + '_' + repeats_name + '_' + k + '.rcccount')
        files.append(os.path.abspath(cur_filename))
    return files


def get_lower_bounds(folder, repeats_name, number_of_inner_nodes):
    any_algorithm = ALGORITHMS[0]
    rcccount_files = get_rcccount_files(folder, repeats_name, any_algorithm)
    lower_bounds = dict(zip(VALUES_OF_K, [0] * len(VALUES_OF_K)))

    for file, k in zip(rcccount_files, VALUES_OF_K):
        lines = open(file, 'r').readlines()
        for line in lines:
            if line.startswith('Worst RCC lower bound'):
                l = line.split()
                lowbound = int(l[4])
                lower_bounds[k] = max(lowbound, number_of_inner_nodes)

    return lower_bounds


def get_all_worstrcc_results(results_folder, repeats_name):
    all_results = {k: {algorithm: 0 for algorithm in ALGORITHMS} for k in VALUES_OF_K}

    for algorithm in ALGORITHMS:
        rcccount_files = get_rcccount_files(results_folder, repeats_name, algorithm)
        for file, k in zip(rcccount_files, VALUES_OF_K):
            lines = open(file, 'r').readlines()
            for line in lines:
                if line.startswith('Worst RCC:'):
                    l = line.split()
                    worst_rcc = l[2]
                    all_results[k][algorithm] = worst_rcc

    return all_results


def print_list_as_csv_row(list):
    for i, l in enumerate(list):
        print(l, end='')
        if i != len(list) - 1:
            print(',', end='')
    print()


def print_csv(all_results, lower_bounds):
    print_list_as_csv_row(VALUES_OF_K)
    for algorithm in ALGORITHMS:
        print_list_as_csv_row([all_results[k][algorithm] for k in VALUES_OF_K])
    print_list_as_csv_row(lower_bounds.values())


def main():
    args = parse_args()
    repeats_file = args.repeats_file[0]
    results_folder = args.results_folder[0]
    repeats_name = args.repeats_name[0]

    if not args.boxplot:
        number_of_inner_nodes = get_number_of_inner_nodes(repeats_file)
        lower_bounds = get_lower_bounds(results_folder, repeats_name, number_of_inner_nodes)

        all_results = get_all_worstrcc_results(results_folder, repeats_name)
        # all_results looks like: {'2': {'algo1': <worstrcc>, 'algo2': <worstrcc>}, '4': ...}

        print_csv(all_results, lower_bounds)
    else:
        print("instance,algorithm,k,coreRCC")
        for algorithm in ALGORITHMS:
            rcccount_files = get_rcccount_files(results_folder, repeats_name, algorithm)
            for file, k in zip(rcccount_files, VALUES_OF_K):
                lines = open(file, 'r').readlines()
                for line in lines:
                    m = re.search(r"^(Core|CPU)\d+ RCC: (\d+)\s+sites: \d+\s+partitions: \d+$", line)
                    if m:
                        rcc_of_core = m.group(2)
                        print(repeats_name, algorithm, k, rcc_of_core, sep=",")


if __name__ == '__main__':
    main()

