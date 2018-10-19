#!/usr/bin/env python3

import config
from subprocess import call
from os import listdir

for file_name in listdir(config.PARTITIONS_DIR):
    if file_name.endswith(".partitions"):
        output_file_name = file_name.split('.')[0] + ".repeats"
        call([config.MSACONVERTER_BIN + "/convert",
              config.SUPERMATRIX_DIR + "/supermatrix_C_nt2.fas",
              config.PARTITIONS_DIR + "/" + file_name,
              config.TREEFILE_DIR + "/supermatrix_C_nt2.newick",
              config.REPEATS_DIR + "/" + output_file_name
        ])



