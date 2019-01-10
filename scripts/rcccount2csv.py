#!/usr/bin/env python3

# warning hacky af script

import argparse


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('files', nargs='+')
    args = parser.parse_args()
    the_files = args.files

    MAX = '8192'
    out = {'2': 0, '4': 0, '8': 0, '12': 0, '16': 0, '24': 0, '32': 0, '48': 0, '64': 0, '96': 0, '128': 0, '160': 0,
           '200': 0, '256': 0, '384': 0, '512': 0, '768': 0, '1024': 0, '1536': 0, '2048': 0, '3072': 0, '4096': 0,
           MAX: 0}

    for file in the_files:
        a = file.split('.')[0]
        split_name = a.split('_')
        cpus = split_name[-1]

        worst_rcc = 0

        lines = open(file).readlines()
        for line in lines:
            if line.startswith('Worst RCC:'):
                l = line.split()
                worst_rcc = l[2]

        if worst_rcc != 0:
            out[cpus] = worst_rcc

    for o in out:
        print(out[o], end='')
        if o != MAX:
            print(',', end='')
    print()


if __name__ == "__main__":
    main()
