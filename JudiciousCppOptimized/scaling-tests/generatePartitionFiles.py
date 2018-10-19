#!/usr/bin/env python3

MIN_SITES = 10000
STEP = MIN_SITES
MAX_SITES = 400000

for num_sites in range(MIN_SITES, MAX_SITES + 1, STEP):
    f = open("supermatrix_subsample_single_partiton_%d.partitions" % num_sites, "w")
    f.write("DNA, all = 1-%d" % num_sites)
    f.close()

