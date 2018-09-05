#!/usr/bin/env python3

import subprocess
import re
import os
from datetime import datetime
import tempfile

programs = {
    "naive":        "./very_naive_split.py",
    "judicious":    "../JudiciousCppOptimized/cmake-build-debug/JudiciousCpp",
}

input_files = {
    "59s":  "../datasets/extracted/59-s.repeats",
    "128s": "../datasets/extracted/128-s.repeats",
    "404s": "../datasets/extracted/404-s.repeats",
    "59l":  "../datasets/extracted/59-l.repeats",
    "128l": "../datasets/extracted/128-l.repeats",
    "404l": "../datasets/extracted/404-l.repeats",
}

rccc_path = "../RepeatsCounter/RepeatsCounter/build/RepeatsCounter"

cores = [2, 4, 8, 12, 16, 24, 32, 48, 64]

folder_name = datetime.now().strftime("%y-%m-%d_%H-%M-%S")
os.mkdir("../results/{}".format(folder_name))

for prog_nick, prog in programs.items():
    for input_nick, input_file in input_files.items():
        print("Running {} with {}...".format(prog_nick, input_nick), end="", flush=True)
        process = subprocess.Popen([prog, input_file, ",".join(str(x) for x in cores)],
                                   stdout=subprocess.PIPE,
                                   universal_newlines=True)
        stdout, _ = process.communicate()

        outputs = {}
        currentOutput = ""
        output_nick = ""
        for line in stdout.split("\n"):
            if re.match("\d+", line) and int(line) in cores:
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
                filename = "../results/{}/{}_{}_{}.rcccout".format(folder_name, prog_nick, input_nick, output_nick)
                subprocess.call([rccc_path, input_file, ddf_file.name], stdout=open(filename, "w"))
        
        print(" Done")
