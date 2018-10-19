#!/usr/bin/env python3

MSACONVERTER_BIN = "."
SUPERMATRIX_DIR = "."
TREEFILE_DIR = "."
PARTITIONS_DIR = "partitions-files"
REPEATS_DIR = "repeats-files"

from subprocess import call
from os import listdir

for file_name in listdir(PARTITIONS_DIR):
    if file_name.endswith(".partitions"):
        output_file_name = file_name.split('.')[0] + ".repeats"
        call([MSACONVERTER_BIN + "/convert",
              SUPERMATRIX_DIR + "/supermatrix_C_nt2.fas",
              PARTITIONS_DIR + "/" + file_name,
              TREEFILE_DIR + "/supermatrix_C_nt2.newick",
              REPEATS_DIR + "/" + output_file_name
        ])



