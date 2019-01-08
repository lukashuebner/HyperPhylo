#!/usr/bin/env python3

import subprocess
import sys
import os
from automation import rdda_path

if len(sys.argv) != 4:
    print("Not enough arguments, expected './hybrid.py repeats_file num_blocks output_file\n")

input_file = str(sys.argv[1])
num_blocks = str(sys.argv[2])
output_file = str(sys.argv[3])

# Run RDDA
p = subprocess.Popen([rdda_path, input_file, num_blocks, "/tmp/temp"], stdout=subprocess.PIPE, universal_newlines=True)
p.communicate()

# Run paritition_splits with temp.splits
p = subprocess.Popen(["./partition_splits.py", input_file, "/tmp/temp.splits"], stdout=subprocess.PIPE, universal_newlines=True)
stdout, _ = p.communicate()

os.remove("/tmp/temp")
os.remove("/tmp/temp.splits")

print(stdout)


