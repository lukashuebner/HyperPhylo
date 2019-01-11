#!/usr/bin/env python3


import argparse
import numpy


def print_result_line(line):
    for i in range(len(line)):
        print(line[i], end='')
        if i != len(line) - 1:
            print(',', end='')
        else:
            print()


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('csv')
    args = parser.parse_args()

    csv_file = args.csv

    csv_lines = open(csv_file, 'r').readlines()
    csv_line0 = csv_lines[0].split(',')
    csv_line1 = csv_lines[1].split(',')
    csv_line2 = csv_lines[2].split(',')
    csv_line3 = csv_lines[3].split(',')

    result_line0 = []
    result_line1 = []
    result_line2 = []
    result_line3 = [1] * len(csv_line3)

    for i in range(len(csv_line1)):
        result_line0.append(int(csv_line0[i]))
        result_line1.append(int(csv_line1[i]) / int(csv_line3[i]))
        result_line2.append(int(csv_line2[i]) / int(csv_line3[i]))

    np_result_line0 = numpy.array(result_line0)
    np_result_line1 = numpy.array(result_line1)
    np_result_line2 = numpy.array(result_line2)
    inds = np_result_line2.argsort()
    sorted_result_line0 = np_result_line0[inds]
    sorted_result_line1 = np_result_line1[inds]
    sorted_result_line2 = np_result_line2[inds]

    for line in [sorted_result_line0, sorted_result_line1, sorted_result_line2, result_line3]:
        print_result_line(line)


if __name__ == '__main__':
    main()
