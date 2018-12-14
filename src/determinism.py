#!/usr/bin/env python3

import subprocess
import difflib
import sys
import psutil
from termcolor import colored

k = "2,8,16,64"
jp_path = "../JudiciousCppOptimized/cmake-build-debug/JudiciousCpp"
input_files = [
    "../datasets/extracted/59-s.repeats",
    "../datasets/extracted/128-s.repeats",
    "../datasets/extracted/404-s.repeats",
    "../datasets/extracted/59-l.repeats",
    "../datasets/59_single.repeats",
    "../datasets/extracted/404-l.repeats",
    "../datasets/extracted/128-l.repeats",
    "../datasets/404_single.repeats",
]

first_output = None

for input_file in input_files:
    print(colored("Running {}".format(input_file), "blue"))
    first_output = subprocess.check_output([jp_path, input_file, k]).decode()

    cpu_count = psutil.cpu_count(logical=False)
    for i in range(1, 101, cpu_count):
        ps = []
        for core in range(cpu_count):
            print("Run {:03d}...".format(i + core))
            sys.stdout.flush()
            ps.append(subprocess.Popen([jp_path, input_file, k], stdout=subprocess.PIPE))

        for core, p in enumerate(ps):
            output, _ = p.communicate()

            diff = difflib.Differ()
            res = list(diff.compare(output.decode().split("\n"), first_output.split("\n")))

            found = False
            for element in res:
                if "Runtime" in element:
                    continue

                if element.startswith("+"):
                    found = True
                    print(colored(element, "green"))
                elif element.startswith("-"):
                    found = True
                    print(colored(element, "red"))

            if found:
                print("Program is not deterministic, difference in run {:03d}! Exiting...".format(i + core))
                exit(1)
