#!/usr/bin/env python3

import subprocess
import re
import os
from datetime import datetime
import time
import tempfile
import shlex
import sys

check_only = False
output_file = "result.ddf"

cmdlines = []

input_files_single_partition = [
    "../datasets/extracted/59-s.repeats",
    "../datasets/extracted/404-s.repeats",
    "../datasets/extracted/128-s.repeats",
    "../datasets/extracted/59-l.repeats",
    "../datasets/extracted/404-l.repeats",
    "../datasets/extracted/128-l.repeats",
    "../datasets/59_single.repeats",
    "../datasets/404_single.repeats",
    "../datasets/128_single.repeats",
]

input_files_multiple_partitions = [
    "../datasets/59.repeats",
    "../datasets/404.repeats",
    "../datasets/128.repeats",
]

rccc_path = "../RepeatsCounter/RepeatsCounter/build/RepeatsCounter"

block_numbers = [2, 4, 8, 12, 16, 24, 32, 48, 64]

# Generate cmdlines for naive
for input_file in input_files_single_partition:
    m = re.search(".*/(.*)\.repeats", input_file)
    if not m:
        print("Mismatch in filename!", file=sys.stderr)
        exit(1)

    cmdlines.append([
        "./very_naive_split.py {} {}".format(input_file, ",".join(str(x) for x in block_numbers)),
        "split",
        input_file,
        "naive_{}".format(m.group(1).replace("-", ""))
    ])

# Generate cmdlines for JudiciousCpp
for input_file in input_files_single_partition:
    m = re.search(".*/(.*)\.repeats", input_file)
    if not m:
        print("Mismatch in filename!", file=sys.stderr)
        exit(1)

    cmdlines.append([
        "../JudiciousCppOptimized/build/JudiciousCpp {} {}".format(input_file, ",".join(str(x) for x in block_numbers)),
        "split",
        input_file,
        "judicious_{}".format(m.group(1).replace("-", ""))
    ])

# Generate cmdlines for RDDA
for input_file in input_files_multiple_partitions:
    m = re.search(".*/(.*)\.repeats", input_file)
    if not m:
        print("Mismatch in filename!", file=sys.stderr)
        exit(1)

    for numberOfBlocks in block_numbers:
        cmdlines.append([
            "../RepeatsCounter/RDDA/build/rdda {} {} {}".format(input_file, numberOfBlocks, output_file),
            "file",
            input_file,
            "rdda_{}_{}.rcccount".format(m.group(1), numberOfBlocks)
        ])

folder_name = datetime.now().strftime("%y-%m-%d_%H-%M-%S")
os.mkdir("../results/{}".format(folder_name))

for cmdline in cmdlines:
    print("Running \"{}\"...".format(cmdline[0]), end="", flush=True)

    process = subprocess.Popen(shlex.split(cmdline[0]), stdout=subprocess.PIPE, universal_newlines=True)
    if check_only:
        time.sleep(1)
        process.terminate()
        stdout, _ = process.communicate()
        print(stdout)
        continue

    stdout, _ = process.communicate()

    if cmdline[1] is "split":
        outputs = {}
        currentOutput = ""
        output_nick = ""
        for line in stdout.split("\n"):
            if re.match("\d+", line) and int(line) in block_numbers:
                if currentOutput:
                    outputs[output_nick] = currentOutput
                currentOutput = line
                output_nick = line
            else:
                currentOutput += "\n"
                currentOutput += line
        if currentOutput:
            outputs[output_nick] = currentOutput

        for output_nick, output in outputs.items():
            with tempfile.NamedTemporaryFile("w") as ddf_file:
                ddf_file.write(output)
                ddf_file.flush()
                filename = "../results/" + folder_name + "/" + cmdline[3] + "_" + output_nick + ".rcccout"
                subprocess.call([rccc_path, cmdline[2], ddf_file.name], stdout=open(filename, "w"))
    elif cmdline[1] is "file":
        filename = "../results/" + folder_name + "/" + cmdline[3]
        subprocess.call([rccc_path, cmdline[2], output_file], stdout=open(filename, "w"))
        os.remove(output_file)

    print(" Done")
