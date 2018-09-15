#!/usr/bin/env python3
# coding: utf-8

import sys
import converter


def main(filename):
    H = converter.partition_file_to_hypergraph(filename)

    print(len(H[1]), len(H[0]))
    for e in H[1]:
        print(" ".join([str(v + 1) for v in e]))


if __name__ == "__main__":
    main(sys.argv[1])
