#!/usr/bin/env python3

import config

for num_sites in range(config.MIN_SITES, config.MAX_SITES + 1, config.STEP):
    f = open("%s/supermatrix_subsample_single_partiton_%d.partitions" % (config.PARTITIONS_DIR, num_sites), "w")
    f.write("DNA, all = 1-%d" % num_sites)
    f.close()

