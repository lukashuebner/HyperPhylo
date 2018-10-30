#!/usr/bin/env python3

import config
from subprocess import check_output, call
import re
import os
import sys
import itertools
import argparse

runtime_pattern = re.compile("Runtime: (\d+)ms")

def generate_partitions_file(num_sites):
    file_name = "%s/supermatrix_subsample_single_partiton_%d.partitions" % (config.PARTITIONS_DIR, num_sites)
    if not os.path.isfile(file_name):
        f = open(file_name, "w")
        f.write("DNA, all = 1-%d" % num_sites)
        f.close()

def generate_repeats_file(num_sites):
    base_file_name = "supermatrix_subsample_single_partiton_%d" % num_sites
    partitions_file_path = config.PARTITIONS_DIR + "/" + base_file_name + ".partitions"
    repeats_file_path = config.REPEATS_DIR + "/" + base_file_name + ".repeats"
    if not os.path.isfile(repeats_file_path):
        call([config.MSACONVERTER_BIN + "/convert",
              config.SUPERMATRIX_DIR + "/supermatrix_C_nt2.fas",
              partitions_file_path,
              config.TREEFILE_DIR + "/supermatrix_C_nt2.newick",
              repeats_file_path
        ])

def strong_tests_generator(num_threads_list):
    for num_threads in num_threads_list:
        yield ("strong", num_threads, config.STRONG_SCALING_NUM_SITES, config.K)

def weak_tests_generator(num_threads_list):
    for num_threads in num_threads_list:
        num_sites = num_threads * config.WEAK_SCALING_SITES_PER_CORE
        yield ("weak", num_threads, num_sites, config.K)

def measure_runtime(num_threads, num_sites, k):
    my_env = os.environ.copy()
    my_env["OMP_NUM_THREADS"] = str(num_threads)
    file_name = "%s/supermatrix_subsample_single_partiton_%d.repeats" % (config.REPEATS_DIR, num_sites)
    output = check_output([config.JUDICIOUS_BIN + "/JudiciousCpp", file_name, str(k)], env=my_env)
    runtime_line = output.split("\n".encode())[-2].decode()
    runtime = int(re.match(runtime_pattern, runtime_line).group(1))
    return runtime

def run_tests(dry_run, num_threads_list, printer):
    test_params = itertools.chain(weak_tests_generator(num_threads_list), strong_tests_generator(num_threads_list))
    for (scaling, num_threads, num_sites, k) in test_params:
        if dry_run:
            runtime = 0
        else:
            generate_partitions_file(num_sites)
            generate_repeats_file(num_sites)
            runtime = measure_runtime(num_threads, num_sites, k)
        printer.print_result(scaling, num_sites, k, num_threads, runtime)

class CSVPrinter:
    """Prints the test results in csv format"""

    _machine_id = ""
    _algorithm = ""

    def __init__(self, machine_id, algorithm, print_header):
        self._machine_id = machine_id
        self._algorithm = algorithm
        if print_header:
            self.print_header()

    def print_header(self):
        print("algorithm,scaling,machine,sites,k,threads,runtime")
        sys.stdout.flush()

    def print_result(self, scaling, num_sites, k, num_threads, runtime):
        print("%s,%s,%s,%d,%d,%d,%d" % (self._algorithm, scaling, self._machine_id, num_sites, k, num_threads, runtime))
        sys.stdout.flush()


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description = "Run scaling tests for JudiciousPartitioner.")
    parser.add_argument("-p", "--print-header", action="store_true")
    parser.add_argument("-m", "--machine-id", default="unknown")
    parser.add_argument("-a", "--algorithm", default="unknown")
    parser.add_argument("-d", "--dry-run", action="store_true")
    parser.add_argument("-t", "--nthreads", type=int, nargs="+", default=[1, 2, 4, 8])
    args = parser.parse_args()

    printer = CSVPrinter(args.machine_id, args.algorithm, args.print_header)
    run_tests(args.dry_run, args.nthreads, printer)

