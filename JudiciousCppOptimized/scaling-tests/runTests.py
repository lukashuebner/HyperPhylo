#!/usr/bin/env python3

import config
from subprocess import check_output
import re
import os
import sys

runtime_pattern = re.compile("Runtime: (\d+)ms")

print("scaling,sites,k,threads,runtime")
sys.stdout.flush()

def num_threads_generator():
    num_threads = config.MIN_THREADS
    while num_threads <= config.MAX_THREADS:
        yield num_threads
        num_threads *= config.THREAD_FACTOR

# Strong scaling
my_env = os.environ.copy()
for num_threads in num_threads_generator():
    my_env["OMP_NUM_THREADS"] = str(num_threads)
    num_sites = config.STRONG_SCALING_NUM_SITES
    file_name = "%s/supermatrix_subsample_single_partiton_%d.repeats" % (config.REPEATS_DIR, num_sites)
    output = check_output([config.JUDICIOUS_BIN + "/JudiciousCpp", file_name, str(config.K)], env=my_env)
    runtime_line = output.split("\n".encode())[-2].decode()
    runtime = int(re.match(runtime_pattern, runtime_line).group(1))
    print("strong,%d,%d,%d,%d" % (num_sites, config.K, num_threads, runtime))
    sys.stdout.flush()

# Weak scaling
my_env = os.environ.copy()
for num_threads in num_threads_generator():
    my_env["OMP_NUM_THREADS"] = str(num_threads)
    num_sites = num_threads * config.WEAK_SCALING_SITES_PER_CORE
    file_name = "%s/supermatrix_subsample_single_partiton_%d.repeats" % (config.REPEATS_DIR, num_sites)
    output = check_output([config.JUDICIOUS_BIN + "/JudiciousCpp", file_name, str(config.K)], env=my_env)
    runtime_line = output.split("\n".encode())[-2].decode()
    runtime = int(re.match(runtime_pattern, runtime_line).group(1))
    print("weak,%d,%d,%d,%d" % (num_sites, config.K, num_threads, runtime))
    sys.stdout.flush()
