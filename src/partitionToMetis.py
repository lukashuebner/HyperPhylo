#!/usr/bin/env python3
# coding: utf-8

import converter
H = converter.partition_file_to_hypergraph("simple_p1.repeats")

print(len(H[1]), len(H[0]))
for e in H[1]:
    print(" ".join([str(v) for v in e]))

