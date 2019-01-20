#!/usr/bin/env python3

import subprocess
import difflib
import sys
import psutil
from termcolor import colored

k = "2,8,16,64"
jp_path_dense = "../JudiciousPartitioning/cmake-build-debug/JudiciousPartitioningDense"
jp_path_sparse = "../JudiciousPartitioning/cmake-build-debug/JudiciousPartitioningSparse"
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

first_output_dense = None
first_output_sparse = None

for input_file in input_files:
    print(colored("Running {}".format(input_file), "blue"))
    first_output_dense = subprocess.check_output([jp_path_dense, input_file, k]).decode()
    first_output_sparse = subprocess.check_output([jp_path_sparse, input_file, k]).decode()

    cpu_count = psutil.cpu_count(logical=False)
    for i in range(1, 101, cpu_count):
        ps_dense = []
        ps_sparse = []
        for core in range(cpu_count):
            print("Run {:03d}...".format(i + core))
            sys.stdout.flush()
            ps_dense.append(subprocess.Popen([jp_path_dense, input_file, k], stdout=subprocess.PIPE))
            ps_sparse.append(subprocess.Popen([jp_path_sparse, input_file, k], stdout=subprocess.PIPE))

        for core in range(cpu_count):
            output_dense, _ = ps_dense[core].communicate()
            output_sparse, _ = ps_sparse[core].communicate()

            found = False
            diff = difflib.Differ()
            res_dense = list(diff.compare(output_dense.decode().split("\n"), first_output_dense.split("\n")))
            for element in res_dense:
                if "Runtime" in element:
                    continue
                elif element.startswith(("+", "-")):
                    found = True

            if found:
                print("##### Diff DENSE:")
                for element in res_dense:
                    if "Runtime" in element:
                        continue
                    elif element.startswith("+"):
                        print(colored(element, "green"))
                    elif element.startswith("-"):
                        print(colored(element, "red"))
                    elif element.startswith("?"):
                        continue
                    else:
                        print(element)

                print("Program DENSE is not deterministic, difference in run {:03d}! Exiting...".format(i + core))
                exit(1)

            res_sparse = list(diff.compare(output_sparse.decode().split("\n"), first_output_sparse.split("\n")))
            for element in res_sparse:
                if "Runtime" in element:
                    continue
                elif element.startswith(("+", "-")):
                    found = True

            if found:
                print("##### Diff SPARSE:")
                for element in res_sparse:
                    if "Runtime" in element:
                        continue
                    elif element.startswith("+"):
                        print(colored(element, "green"))
                    elif element.startswith("-"):
                        print(colored(element, "red"))
                    elif element.startswith("?"):
                        continue
                    else:
                        print(element)

                print("Program SPARSE is not deterministic, difference in run {:03d}! Exiting...".format(i + core))
                exit(1)

            res_both = list(diff.compare(output_dense.decode().split("\n"), output_sparse.decode().split("\n")))
            for element in res_both:
                if "Runtime" in element:
                    continue
                elif element.startswith(("+", "-")):
                    found = True

            if found:
                print("##### Diff BOTH:")
                for element in res_both:
                    if "Runtime" in element:
                        continue
                    elif element.startswith("+"):
                        print(colored(element, "green"))
                    elif element.startswith("-"):
                        print(colored(element, "red"))
                    elif element.startswith("?"):
                        continue
                    else:
                        print(element)

                print("Programs don't have the same result, difference in run {:03d}! Exiting...".format(i + core))
                exit(1)
