#!/usr/bin/env python3
# This is a super dirty script, be careful

import argparse


def parse_arguments():
    parser = argparse.ArgumentParser()
    parser.add_argument('file', nargs=1, help='the file that contains all the results')
    args = parser.parse_args()
    return args


def get_output_file_name_template(input_file_name):
    split = input_file_name.split('.')
    split[0] += '-{}-judicious.'
    return split[0] + split[1]


def main():
    """
    Extract all the DDFs from a result file that contains many.
    """
    args = parse_arguments()
    input_file = args.file[0]

    output_file_name_template = get_output_file_name_template(input_file)

    cur_file = ''
    output_file = None
    f = open(input_file, 'r')
    for line in f.readlines():
        if not line.startswith(('C', 'p')):  # k
            cur_file = output_file_name_template.format(line.strip())
            print(cur_file)
            outout_file = open(cur_file, 'a')
            outout_file.write(line)
        else:  # "CPU" or "partition"
            outout_file.write(line)


if __name__ == "__main__":
    main()
