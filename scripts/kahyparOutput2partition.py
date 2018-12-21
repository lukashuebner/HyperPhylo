#!/usr/bin/env python3
import very_naive_split
import sys

k = int(sys.argv[2])
split = [[] for x in range(0, k)]
with open(sys.argv[1], "r") as file:
    for idx, line in enumerate(file):
        split[int(line)].append(idx)

very_naive_split.print_split(split, k)
